#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/toolbox/args.h>
#include <furi_hal_bt_hid.h>
#include <storage/storage.h>
#include "bad_ble_script.h"
#include <dolphin/dolphin.h>

#include <bt/bt_service/bt.h>

#define HID_BT_KEYS_STORAGE_PATH EXT_PATH("apps/Tools/.bt_hid.keys")

#define TAG "BadBle"
#define WORKER_TAG TAG "Worker"
#define FILE_BUFFER_LEN 16

#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)

#define BADBLE_ASCII_TO_KEY(script, x) \
    (((uint8_t)x < 128) ? (script->layout[(uint8_t)x]) : HID_KEYBOARD_NONE)

typedef enum {
    WorkerEvtToggle = (1 << 0),
    WorkerEvtEnd = (1 << 1),
    WorkerEvtConnect = (1 << 2),
    WorkerEvtDisconnect = (1 << 3),
} WorkerEvtFlags;

typedef enum {
    LevelRssi122_100,
    LevelRssi99_80,
    LevelRssi79_60,
    LevelRssi59_40,
    LevelRssi39_0,
    LevelRssiNum,
    LevelRssiError = 0xFF,
} LevelRssiRange;

/**
 * Delays for waiting between HID key press and key release
*/
const uint8_t bt_hid_delays[LevelRssiNum] = {
    30, // LevelRssi122_100
    25, // LevelRssi99_80
    20, // LevelRssi79_60
    17, // LevelRssi59_40
    14, // LevelRssi39_0
};

struct BadBleScript {
    BadBleState st;
    FuriString* file_path;
    uint32_t defdelay;
    uint16_t layout[128];
    FuriThread* thread;
    uint8_t file_buf[FILE_BUFFER_LEN + 1];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;
    FuriString* line;

    FuriString* line_prev;
    uint32_t repeat_cnt;

    File* debug_file;
    Bt* bt;
};

typedef struct {
    char* name;
    uint16_t keycode;
} DuckyKey;

static const DuckyKey ducky_keys[] = {
    {"CTRL-ALT", KEY_MOD_LEFT_CTRL | KEY_MOD_LEFT_ALT},
    {"CTRL-SHIFT", KEY_MOD_LEFT_CTRL | KEY_MOD_LEFT_SHIFT},
    {"ALT-SHIFT", KEY_MOD_LEFT_ALT | KEY_MOD_LEFT_SHIFT},
    {"ALT-GUI", KEY_MOD_LEFT_ALT | KEY_MOD_LEFT_GUI},
    {"GUI-SHIFT", KEY_MOD_LEFT_GUI | KEY_MOD_LEFT_SHIFT},

    {"CTRL", KEY_MOD_LEFT_CTRL},
    {"CONTROL", KEY_MOD_LEFT_CTRL},
    {"SHIFT", KEY_MOD_LEFT_SHIFT},
    {"ALT", KEY_MOD_LEFT_ALT},
    {"GUI", KEY_MOD_LEFT_GUI},
    {"WINDOWS", KEY_MOD_LEFT_GUI},

    {"DOWNARROW", HID_KEYBOARD_DOWN_ARROW},
    {"DOWN", HID_KEYBOARD_DOWN_ARROW},
    {"LEFTARROW", HID_KEYBOARD_LEFT_ARROW},
    {"LEFT", HID_KEYBOARD_LEFT_ARROW},
    {"RIGHTARROW", HID_KEYBOARD_RIGHT_ARROW},
    {"RIGHT", HID_KEYBOARD_RIGHT_ARROW},
    {"UPARROW", HID_KEYBOARD_UP_ARROW},
    {"UP", HID_KEYBOARD_UP_ARROW},

    {"ENTER", HID_KEYBOARD_RETURN},
    {"BREAK", HID_KEYBOARD_PAUSE},
    {"PAUSE", HID_KEYBOARD_PAUSE},
    {"CAPSLOCK", HID_KEYBOARD_CAPS_LOCK},
    {"DELETE", HID_KEYBOARD_DELETE},
    {"BACKSPACE", HID_KEYPAD_BACKSPACE},
    {"END", HID_KEYBOARD_END},
    {"ESC", HID_KEYBOARD_ESCAPE},
    {"ESCAPE", HID_KEYBOARD_ESCAPE},
    {"HOME", HID_KEYBOARD_HOME},
    {"INSERT", HID_KEYBOARD_INSERT},
    {"NUMLOCK", HID_KEYPAD_NUMLOCK},
    {"PAGEUP", HID_KEYBOARD_PAGE_UP},
    {"PAGEDOWN", HID_KEYBOARD_PAGE_DOWN},
    {"PRINTSCREEN", HID_KEYBOARD_PRINT_SCREEN},
    {"SCROLLLOCK", HID_KEYBOARD_SCROLL_LOCK},
    {"SPACE", HID_KEYBOARD_SPACEBAR},
    {"TAB", HID_KEYBOARD_TAB},
    {"MENU", HID_KEYBOARD_APPLICATION},
    {"APP", HID_KEYBOARD_APPLICATION},

    {"F1", HID_KEYBOARD_F1},
    {"F2", HID_KEYBOARD_F2},
    {"F3", HID_KEYBOARD_F3},
    {"F4", HID_KEYBOARD_F4},
    {"F5", HID_KEYBOARD_F5},
    {"F6", HID_KEYBOARD_F6},
    {"F7", HID_KEYBOARD_F7},
    {"F8", HID_KEYBOARD_F8},
    {"F9", HID_KEYBOARD_F9},
    {"F10", HID_KEYBOARD_F10},
    {"F11", HID_KEYBOARD_F11},
    {"F12", HID_KEYBOARD_F12},
};

static const char ducky_cmd_comment[] = {"REM"};
static const char ducky_cmd_id[] = {"ID"};
static const char ducky_cmd_delay[] = {"DELAY "};
static const char ducky_cmd_string[] = {"STRING "};
static const char ducky_cmd_defdelay_1[] = {"DEFAULT_DELAY "};
static const char ducky_cmd_defdelay_2[] = {"DEFAULTDELAY "};
static const char ducky_cmd_repeat[] = {"REPEAT "};
static const char ducky_cmd_sysrq[] = {"SYSRQ "};

static const char ducky_cmd_altchar[] = {"ALTCHAR "};
static const char ducky_cmd_altstr_1[] = {"ALTSTRING "};
static const char ducky_cmd_altstr_2[] = {"ALTCODE "};

static const char ducky_cmd_lang[] = {"DUCKY_LANG"};

static const uint8_t numpad_keys[10] = {
    HID_KEYPAD_0,
    HID_KEYPAD_1,
    HID_KEYPAD_2,
    HID_KEYPAD_3,
    HID_KEYPAD_4,
    HID_KEYPAD_5,
    HID_KEYPAD_6,
    HID_KEYPAD_7,
    HID_KEYPAD_8,
    HID_KEYPAD_9,
};

uint8_t bt_timeout = 0;

static LevelRssiRange bt_remote_rssi_range(Bt* bt) {
    BtRssi rssi_data = {0};

    if(!bt_remote_rssi(bt, &rssi_data)) return LevelRssiError;

    if(rssi_data.rssi <= 39)
        return LevelRssi39_0;
    else if(rssi_data.rssi <= 59)
        return LevelRssi59_40;
    else if(rssi_data.rssi <= 79)
        return LevelRssi79_60;
    else if(rssi_data.rssi <= 99)
        return LevelRssi99_80;
    else if(rssi_data.rssi <= 122)
        return LevelRssi122_100;

    return LevelRssiError;
}

static inline void update_bt_timeout(Bt* bt) {
    LevelRssiRange r = bt_remote_rssi_range(bt);
    if(r < LevelRssiNum) {
        bt_timeout = bt_hid_delays[r];
    }
}

/**
 *  @brief  Wait until there are enough free slots in the keyboard buffer
 * 
 * @param  n_free_chars    Number of free slots to wait for (and consider the buffer not full) 
*/
static void bt_hid_hold_while_keyboard_buffer_full(uint8_t n_free_chars, int32_t timeout) {
    uint32_t start = furi_get_tick();
    uint32_t timeout_ms = timeout <= -1 ? 0 : timeout;
    while(furi_hal_bt_hid_kb_free_slots(n_free_chars) == false) {
        furi_delay_ms(100);

        if(timeout != -1 && (furi_get_tick() - start) > timeout_ms) {
            break;
        }
    }
}

static bool ducky_get_number(const char* param, uint32_t* val) {
    uint32_t value = 0;
    if(sscanf(param, "%lu", &value) == 1) {
        *val = value;
        return true;
    }
    return false;
}

static uint32_t ducky_get_command_len(const char* line) {
    uint32_t len = strlen(line);
    for(uint32_t i = 0; i < len; i++) {
        if(line[i] == ' ') return i;
    }
    return 0;
}

static bool ducky_is_line_end(const char chr) {
    return ((chr == ' ') || (chr == '\0') || (chr == '\r') || (chr == '\n'));
}

static void ducky_numlock_on() {
    if((furi_hal_hid_get_led_state() & HID_KB_LED_NUM) == 0) {
        bt_hid_hold_while_keyboard_buffer_full(1, -1);
        furi_hal_bt_hid_kb_press(HID_KEYBOARD_LOCK_NUM_LOCK);

        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(HID_KEYBOARD_LOCK_NUM_LOCK);
    }
}

static bool ducky_numpad_press(const char num) {
    if((num < '0') || (num > '9')) return false;

    uint16_t key = numpad_keys[num - '0'];
    bt_hid_hold_while_keyboard_buffer_full(1, -1);
    FURI_LOG_I(WORKER_TAG, "Pressing %c\r\n", num);
    furi_hal_bt_hid_kb_press(key);
    furi_delay_ms(bt_timeout);
    furi_hal_bt_hid_kb_release(key);

    return true;
}

static bool ducky_altchar(const char* charcode) {
    uint8_t i = 0;
    bool state = false;

    FURI_LOG_I(WORKER_TAG, "char %s", charcode);

    bt_hid_hold_while_keyboard_buffer_full(1, -1);
    furi_hal_bt_hid_kb_press(KEY_MOD_LEFT_ALT);

    while(!ducky_is_line_end(charcode[i])) {
        state = ducky_numpad_press(charcode[i]);
        if(state == false) break;
        i++;
    }

    furi_hal_bt_hid_kb_release(KEY_MOD_LEFT_ALT);
    return state;
}

static bool ducky_altstring(const char* param) {
    uint32_t i = 0;
    bool state = false;

    while(param[i] != '\0') {
        if((param[i] < ' ') || (param[i] > '~')) {
            i++;
            continue; // Skip non-printable chars
        }

        char temp_str[4];
        snprintf(temp_str, 4, "%u", param[i]);

        state = ducky_altchar(temp_str);
        if(state == false) break;
        i++;
    }
    return state;
}

static bool ducky_string(BadBleScript* bad_ble, const char* param) {
    uint32_t i = 0;
    while(param[i] != '\0') {
        uint16_t keycode = BADBLE_ASCII_TO_KEY(bad_ble, param[i]);
        if(keycode != HID_KEYBOARD_NONE) {
            bt_hid_hold_while_keyboard_buffer_full(1, -1);
            furi_hal_bt_hid_kb_press(keycode);

            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(keycode);
        }
        i++;
    }
    return true;
}

static uint16_t ducky_get_keycode(BadBleScript* bad_ble, const char* param, bool accept_chars) {
    for(size_t i = 0; i < (sizeof(ducky_keys) / sizeof(ducky_keys[0])); i++) {
        size_t key_cmd_len = strlen(ducky_keys[i].name);
        if((strncmp(param, ducky_keys[i].name, key_cmd_len) == 0) &&
           (ducky_is_line_end(param[key_cmd_len]))) {
            return ducky_keys[i].keycode;
        }
    }
    if((accept_chars) && (strlen(param) > 0)) {
        return (BADBLE_ASCII_TO_KEY(bad_ble, param[0]) & 0xFF);
    }
    return 0;
}

static int32_t
    ducky_parse_line(BadBleScript* bad_ble, FuriString* line, char* error, size_t error_len) {
    uint32_t line_len = furi_string_size(line);
    const char* line_tmp = furi_string_get_cstr(line);
    bool state = false;

    if(line_len == 0) {
        return SCRIPT_STATE_NEXT_LINE; // Skip empty lines
    }

    // General commands
    if(strncmp(line_tmp, ducky_cmd_comment, strlen(ducky_cmd_comment)) == 0) {
        // REM - comment line
        return (0);
    } else if(strncmp(line_tmp, ducky_cmd_id, strlen(ducky_cmd_id)) == 0) {
        // ID - executed in ducky_script_preload
        return (0);
    } else if(strncmp(line_tmp, ducky_cmd_lang, strlen(ducky_cmd_lang)) == 0) {
        // DUCKY_LANG - ignore command to retain compatibility with existing scripts
        return (0);
    } else if(strncmp(line_tmp, ducky_cmd_delay, strlen(ducky_cmd_delay)) == 0) {
        // DELAY
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        uint32_t delay_val = 0;
        state = ducky_get_number(line_tmp, &delay_val);
        if((state) && (delay_val > 0)) {
            return (int32_t)delay_val;
        }
        if(error != NULL) {
            snprintf(error, error_len, "Invalid number %s", line_tmp);
        }
        return SCRIPT_STATE_ERROR;
    } else if(
        (strncmp(line_tmp, ducky_cmd_defdelay_1, strlen(ducky_cmd_defdelay_1)) == 0) ||
        (strncmp(line_tmp, ducky_cmd_defdelay_2, strlen(ducky_cmd_defdelay_2)) == 0)) {
        // DEFAULT_DELAY
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        state = ducky_get_number(line_tmp, &bad_ble->defdelay);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid number %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_string, strlen(ducky_cmd_string)) == 0) {
        // STRING
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        state = ducky_string(bad_ble, line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid string %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_altchar, strlen(ducky_cmd_altchar)) == 0) {
        // ALTCHAR
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        ducky_numlock_on();
        state = ducky_altchar(line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid altchar %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(
        (strncmp(line_tmp, ducky_cmd_altstr_1, strlen(ducky_cmd_altstr_1)) == 0) ||
        (strncmp(line_tmp, ducky_cmd_altstr_2, strlen(ducky_cmd_altstr_2)) == 0)) {
        // ALTSTRING
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        ducky_numlock_on();
        state = ducky_altstring(line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid altstring %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_repeat, strlen(ducky_cmd_repeat)) == 0) {
        // REPEAT
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        state = ducky_get_number(line_tmp, &bad_ble->repeat_cnt);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid number %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_sysrq, strlen(ducky_cmd_sysrq)) == 0) {
        // SYSRQ
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        uint16_t key = ducky_get_keycode(bad_ble, line_tmp, true);
        bt_hid_hold_while_keyboard_buffer_full(1, -1);
        furi_hal_bt_hid_kb_press(KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
        furi_hal_bt_hid_kb_press(key);

        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(key);
        furi_hal_bt_hid_kb_release(KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
        return (0);
    } else {
        // Special keys + modifiers
        uint16_t key = ducky_get_keycode(bad_ble, line_tmp, false);
        if(key == HID_KEYBOARD_NONE) {
            if(error != NULL) {
                snprintf(error, error_len, "No keycode defined for %s", line_tmp);
            }
            return SCRIPT_STATE_ERROR;
        }
        if((key & 0xFF00) != 0) {
            // It's a modifier key
            line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
            key |= ducky_get_keycode(bad_ble, line_tmp, true);
        }
        furi_hal_bt_hid_kb_press(key);

        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(key);
        return (0);
    }
}

static bool ducky_script_preload(BadBleScript* bad_ble, File* script_file) {
    uint8_t ret = 0;
    uint32_t line_len = 0;

    furi_string_reset(bad_ble->line);

    do {
        ret = storage_file_read(script_file, bad_ble->file_buf, FILE_BUFFER_LEN);
        for(uint16_t i = 0; i < ret; i++) {
            if(bad_ble->file_buf[i] == '\n' && line_len > 0) {
                bad_ble->st.line_nb++;
                line_len = 0;
            } else {
                if(bad_ble->st.line_nb == 0) { // Save first line
                    furi_string_push_back(bad_ble->line, bad_ble->file_buf[i]);
                }
                line_len++;
            }
        }
        if(storage_file_eof(script_file)) {
            if(line_len > 0) {
                bad_ble->st.line_nb++;
                break;
            }
        }
    } while(ret > 0);

    storage_file_seek(script_file, 0, true);
    furi_string_reset(bad_ble->line);

    return true;
}

static int32_t ducky_script_execute_next(BadBleScript* bad_ble, File* script_file) {
    int32_t delay_val = 0;

    if(bad_ble->repeat_cnt > 0) {
        bad_ble->repeat_cnt--;
        delay_val = ducky_parse_line(
            bad_ble, bad_ble->line_prev, bad_ble->st.error, sizeof(bad_ble->st.error));
        if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
            return 0;
        } else if(delay_val < 0) { // Script error
            bad_ble->st.error_line = bad_ble->st.line_cur - 1;
            FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_ble->st.line_cur - 1U);
            return SCRIPT_STATE_ERROR;
        } else {
            return (delay_val + bad_ble->defdelay);
        }
    }

    furi_string_set(bad_ble->line_prev, bad_ble->line);
    furi_string_reset(bad_ble->line);

    while(1) {
        if(bad_ble->buf_len == 0) {
            bad_ble->buf_len = storage_file_read(script_file, bad_ble->file_buf, FILE_BUFFER_LEN);
            if(storage_file_eof(script_file)) {
                if((bad_ble->buf_len < FILE_BUFFER_LEN) && (bad_ble->file_end == false)) {
                    bad_ble->file_buf[bad_ble->buf_len] = '\n';
                    bad_ble->buf_len++;
                    bad_ble->file_end = true;
                }
            }

            bad_ble->buf_start = 0;
            if(bad_ble->buf_len == 0) return SCRIPT_STATE_END;
        }
        for(uint8_t i = bad_ble->buf_start; i < (bad_ble->buf_start + bad_ble->buf_len); i++) {
            if(bad_ble->file_buf[i] == '\n' && furi_string_size(bad_ble->line) > 0) {
                bad_ble->st.line_cur++;
                bad_ble->buf_len = bad_ble->buf_len + bad_ble->buf_start - (i + 1);
                bad_ble->buf_start = i + 1;
                furi_string_trim(bad_ble->line);
                delay_val = ducky_parse_line(
                    bad_ble, bad_ble->line, bad_ble->st.error, sizeof(bad_ble->st.error));
                if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
                    return 0;
                } else if(delay_val < 0) {
                    bad_ble->st.error_line = bad_ble->st.line_cur;
                    if(delay_val == SCRIPT_STATE_NEXT_LINE) {
                        snprintf(
                            bad_ble->st.error, sizeof(bad_ble->st.error), "Forbidden empty line");
                        FURI_LOG_E(
                            WORKER_TAG, "Forbidden empty line at line %u", bad_ble->st.line_cur);
                    } else {
                        FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_ble->st.line_cur);
                    }
                    return SCRIPT_STATE_ERROR;
                } else {
                    return (delay_val + bad_ble->defdelay);
                }
            } else {
                furi_string_push_back(bad_ble->line, bad_ble->file_buf[i]);
            }
        }
        bad_ble->buf_len = 0;
        if(bad_ble->file_end) return SCRIPT_STATE_END;
    }

    return 0;
}

static void bad_ble_hid_state_callback(BtStatus status, void* context) {
    furi_assert(context);
    BadBleScript* bad_ble = (BadBleScript*)context;
    bool state = (status == BtStatusConnected);

    if(state == true) {
        LevelRssiRange r = bt_remote_rssi_range(bad_ble->bt);
        if(r != LevelRssiError) {
            bt_timeout = bt_hid_delays[r];
        }
        furi_thread_flags_set(furi_thread_get_id(bad_ble->thread), WorkerEvtConnect);
    } else
        furi_thread_flags_set(furi_thread_get_id(bad_ble->thread), WorkerEvtDisconnect);
}

static int32_t bad_ble_worker(void* context) {
    BadBleScript* bad_ble = context;

    BadBleWorkerState worker_state = BadBleStateInit;
    int32_t delay_val = 0;

    // BLE HID init
    bt_timeout = bt_hid_delays[LevelRssi39_0];

    bt_disconnect(bad_ble->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_storage_path(bad_ble->bt, HID_BT_KEYS_STORAGE_PATH);

    if(!bt_set_profile(bad_ble->bt, BtProfileHidKeyboard)) {
        FURI_LOG_E(TAG, "Failed to switch to HID profile");
        return -1;
    }

    furi_hal_bt_start_advertising();

    FURI_LOG_I(WORKER_TAG, "Init");
    File* script_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    bad_ble->line = furi_string_alloc();
    bad_ble->line_prev = furi_string_alloc();

    bt_set_status_changed_callback(bad_ble->bt, bad_ble_hid_state_callback, bad_ble);
    while(1) {
        if(worker_state == BadBleStateInit) { // State: initialization
            if(storage_file_open(
                   script_file,
                   furi_string_get_cstr(bad_ble->file_path),
                   FSAM_READ,
                   FSOM_OPEN_EXISTING)) {
                if((ducky_script_preload(bad_ble, script_file)) && (bad_ble->st.line_nb > 0)) {
                    worker_state = BadBleStateNotConnected; // Ready to run
                } else {
                    worker_state = BadBleStateScriptError; // Script preload error
                }
            } else {
                FURI_LOG_E(WORKER_TAG, "File open error");
                worker_state = BadBleStateFileError; // File open error
            }
            bad_ble->st.state = worker_state;

        } else if(worker_state == BadBleStateNotConnected) { // State: ble not connected
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) {
                worker_state = BadBleStateIdle; // Ready to run
            } else if(flags & WorkerEvtToggle) {
                worker_state = BadBleStateWillRun; // Will run when ble is connected
            }
            bad_ble->st.state = worker_state;

        } else if(worker_state == BadBleStateIdle) { // State: ready to start
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtToggle | WorkerEvtDisconnect,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtToggle) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadUsbPlayScript);
                delay_val = 0;
                bad_ble->buf_len = 0;
                bad_ble->st.line_cur = 0;
                bad_ble->defdelay = 0;
                bad_ble->repeat_cnt = 0;
                bad_ble->file_end = false;
                storage_file_seek(script_file, 0, true);
                worker_state = BadBleStateRunning;
            } else if(flags & WorkerEvtDisconnect) {
                worker_state = BadBleStateNotConnected; // ble disconnected
            }
            bad_ble->st.state = worker_state;

        } else if(worker_state == BadBleStateWillRun) { // State: start on connection
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadUsbPlayScript);
                delay_val = 0;
                bad_ble->buf_len = 0;
                bad_ble->st.line_cur = 0;
                bad_ble->defdelay = 0;
                bad_ble->repeat_cnt = 0;
                bad_ble->file_end = false;
                storage_file_seek(script_file, 0, true);
                // extra time for PC to recognize Flipper as keyboard
                furi_thread_flags_wait(0, FuriFlagWaitAny, 1500);

                update_bt_timeout(bad_ble->bt);
                FURI_LOG_I(WORKER_TAG, "BLE Key timeout : %u", bt_timeout);

                worker_state = BadBleStateRunning;
            } else if(flags & WorkerEvtToggle) { // Cancel scheduled execution
                worker_state = BadBleStateNotConnected;
            }
            bad_ble->st.state = worker_state;

        } else if(worker_state == BadBleStateRunning) { // State: running
            uint16_t delay_cur = (delay_val > 1000) ? (1000) : (delay_val);
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtToggle | WorkerEvtDisconnect, FuriFlagWaitAny, delay_cur);
            delay_val -= delay_cur;
            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtToggle) {
                    worker_state = BadBleStateIdle; // Stop executing script
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadBleStateNotConnected; // ble disconnected
                    furi_hal_bt_hid_kb_release_all();
                }
                bad_ble->st.state = worker_state;
                continue;
            } else if(
                (flags == (unsigned)FuriFlagErrorTimeout) ||
                (flags == (unsigned)FuriFlagErrorResource)) {
                if(delay_val > 0) {
                    bad_ble->st.delay_remain--;
                    continue;
                }
                bad_ble->st.state = BadBleStateRunning;
                delay_val = ducky_script_execute_next(bad_ble, script_file);
                if(delay_val == SCRIPT_STATE_ERROR) { // Script error
                    delay_val = 0;
                    worker_state = BadBleStateScriptError;
                    bad_ble->st.state = worker_state;
                } else if(delay_val == SCRIPT_STATE_END) { // End of script
                    delay_val = 0;
                    worker_state = BadBleStateIdle;
                    bad_ble->st.state = BadBleStateDone;
                    furi_hal_bt_hid_kb_release_all();
                    continue;
                } else if(delay_val > 1000) {
                    bad_ble->st.state = BadBleStateDelay; // Show long delays
                    bad_ble->st.delay_remain = delay_val / 1000;
                }
            } else {
                furi_check((flags & FuriFlagError) == 0);
            }

        } else if(
            (worker_state == BadBleStateFileError) ||
            (worker_state == BadBleStateScriptError)) { // State: error
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd, FuriFlagWaitAny, FuriWaitForever); // Waiting for exit command
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            }
        }

        update_bt_timeout(bad_ble->bt);
        FURI_LOG_D(WORKER_TAG, "BLE Key timeout : %u", bt_timeout);
    }

    // release all keys
    bt_hid_hold_while_keyboard_buffer_full(6, 3000);

    // stop ble
    bt_set_status_changed_callback(bad_ble->bt, NULL, NULL);

    bt_disconnect(bad_ble->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_default_path(bad_ble->bt);

    if(!bt_set_profile(bad_ble->bt, BtProfileSerial)) {
        FURI_LOG_E(TAG, "Failed to switch to Serial profile");
    }

    storage_file_close(script_file);
    storage_file_free(script_file);
    furi_string_free(bad_ble->line);
    furi_string_free(bad_ble->line_prev);

    FURI_LOG_I(WORKER_TAG, "End");

    return 0;
}

static void bad_ble_script_set_default_keyboard_layout(BadBleScript* bad_ble) {
    furi_assert(bad_ble);
    memset(bad_ble->layout, HID_KEYBOARD_NONE, sizeof(bad_ble->layout));
    memcpy(bad_ble->layout, hid_asciimap, MIN(sizeof(hid_asciimap), sizeof(bad_ble->layout)));
}

BadBleScript* bad_ble_script_open(FuriString* file_path, Bt* bt) {
    furi_assert(file_path);

    BadBleScript* bad_ble = malloc(sizeof(BadBleScript));
    bad_ble->file_path = furi_string_alloc();
    furi_string_set(bad_ble->file_path, file_path);
    bad_ble_script_set_default_keyboard_layout(bad_ble);

    bad_ble->st.state = BadBleStateInit;
    bad_ble->st.error[0] = '\0';

    bad_ble->bt = bt;

    bad_ble->thread = furi_thread_alloc_ex("BadBleWorker", 2048, bad_ble_worker, bad_ble);
    furi_thread_start(bad_ble->thread);
    return bad_ble;
} //-V773

void bad_ble_script_close(BadBleScript* bad_ble) {
    furi_assert(bad_ble);
    furi_record_close(RECORD_STORAGE);
    furi_thread_flags_set(furi_thread_get_id(bad_ble->thread), WorkerEvtEnd);
    furi_thread_join(bad_ble->thread);
    furi_thread_free(bad_ble->thread);
    furi_string_free(bad_ble->file_path);
    free(bad_ble);
}

void bad_ble_script_set_keyboard_layout(BadBleScript* bad_ble, FuriString* layout_path) {
    furi_assert(bad_ble);

    if((bad_ble->st.state == BadBleStateRunning) || (bad_ble->st.state == BadBleStateDelay)) {
        // do not update keyboard layout while a script is running
        return;
    }

    File* layout_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    if(!furi_string_empty(layout_path)) {
        if(storage_file_open(
               layout_file, furi_string_get_cstr(layout_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint16_t layout[128];
            if(storage_file_read(layout_file, layout, sizeof(layout)) == sizeof(layout)) {
                memcpy(bad_ble->layout, layout, sizeof(layout));
            }
        }
        storage_file_close(layout_file);
    } else {
        bad_ble_script_set_default_keyboard_layout(bad_ble);
    }
    storage_file_free(layout_file);
}

void bad_ble_script_toggle(BadBleScript* bad_ble) {
    furi_assert(bad_ble);
    furi_thread_flags_set(furi_thread_get_id(bad_ble->thread), WorkerEvtToggle);
}

BadBleState* bad_ble_script_get_state(BadBleScript* bad_ble) {
    furi_assert(bad_ble);
    return &(bad_ble->st);
}
