#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/toolbox/args.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>
#include <storage/storage.h>
#include "bad_kb_script.h"
#include <dolphin/dolphin.h>

#include <bt/bt_service/bt.h>

#define HID_BT_KEYS_STORAGE_PATH EXT_PATH("apps/Tools/.bt_hid.keys")

#define TAG "BadKB"
#define WORKER_TAG TAG "Worker"
#define FILE_BUFFER_LEN 16

#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)

#define BADKB_ASCII_TO_KEY(script, x) \
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

typedef enum {
    BadKbConnectionModeNone,
    BadKbConnectionModeUsb,
    BadKbConnectionModeBt,
} BadKbConnectionMode;

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

struct BadKbScript {
    FuriHalUsbHidConfig hid_cfg;
    BadKbState st;
    FuriString* file_path;
    FuriString* keyboard_layout;
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
    {"GUI-CTRL", KEY_MOD_LEFT_GUI | KEY_MOD_LEFT_CTRL},

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

BadKbConnectionMode connection_mode = BadKbConnectionModeNone;
FuriHalUsbInterface* usb_mode_prev = NULL;
GapPairing bt_mode_prev = GapPairingNone;
bool bt_connected = false;
bool usb_connected = false;
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
        FURI_LOG_D(WORKER_TAG, "BLE Key timeout : %u", bt_timeout);
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

static void ducky_numlock_on(BadKbScript* bad_kb) {
    if(bad_kb->bt) {
        if((furi_hal_bt_hid_get_led_state() & HID_KB_LED_NUM) == 0) { // FIXME
            furi_hal_bt_hid_kb_press(HID_KEYBOARD_LOCK_NUM_LOCK);
            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(HID_KEYBOARD_LOCK_NUM_LOCK);
        }
    } else {
        if((furi_hal_hid_get_led_state() & HID_KB_LED_NUM) == 0) {
            furi_hal_hid_kb_press(HID_KEYBOARD_LOCK_NUM_LOCK);
            furi_hal_hid_kb_release(HID_KEYBOARD_LOCK_NUM_LOCK);
        }
    }
}

static bool ducky_numpad_press(BadKbScript* bad_kb, const char num) {
    if((num < '0') || (num > '9')) return false;

    uint16_t key = numpad_keys[num - '0'];
    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_press(key);
        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(key);
    } else {
        furi_hal_hid_kb_press(key);
        furi_hal_hid_kb_release(key);
    }

    return true;
}

static bool ducky_altchar(BadKbScript* bad_kb, const char* charcode) {
    uint8_t i = 0;
    bool state = false;

    FURI_LOG_I(WORKER_TAG, "char %s", charcode);

    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_press(KEY_MOD_LEFT_ALT);
    } else {
        furi_hal_hid_kb_press(KEY_MOD_LEFT_ALT);
    }

    while(!ducky_is_line_end(charcode[i])) {
        state = ducky_numpad_press(bad_kb, charcode[i]);
        if(state == false) break;
        i++;
    }

    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_release(KEY_MOD_LEFT_ALT);
    } else {
        furi_hal_hid_kb_release(KEY_MOD_LEFT_ALT);
    }
    return state;
}

static bool ducky_altstring(BadKbScript* bad_kb, const char* param) {
    uint32_t i = 0;
    bool state = false;

    while(param[i] != '\0') {
        if((param[i] < ' ') || (param[i] > '~')) {
            i++;
            continue; // Skip non-printable chars
        }

        char temp_str[4];
        snprintf(temp_str, 4, "%u", param[i]);

        state = ducky_altchar(bad_kb, temp_str);
        if(state == false) break;
        i++;
    }
    return state;
}

static bool ducky_string(BadKbScript* bad_kb, const char* param) {
    uint32_t i = 0;
    while(param[i] != '\0') {
        uint16_t keycode = BADKB_ASCII_TO_KEY(bad_kb, param[i]);
        if(keycode != HID_KEYBOARD_NONE) {
            if(bad_kb->bt) {
                furi_hal_bt_hid_kb_press(keycode);
                furi_delay_ms(bt_timeout);
                furi_hal_bt_hid_kb_release(keycode);
            } else {
                furi_hal_hid_kb_press(keycode);
                furi_hal_hid_kb_release(keycode);
            }
        }
        i++;
    }
    return true;
}

static uint16_t ducky_get_keycode(BadKbScript* bad_kb, const char* param, bool accept_chars) {
    for(size_t i = 0; i < (sizeof(ducky_keys) / sizeof(ducky_keys[0])); i++) {
        size_t key_cmd_len = strlen(ducky_keys[i].name);
        if((strncmp(param, ducky_keys[i].name, key_cmd_len) == 0) &&
           (ducky_is_line_end(param[key_cmd_len]))) {
            return ducky_keys[i].keycode;
        }
    }
    if((accept_chars) && (strlen(param) > 0)) {
        return (BADKB_ASCII_TO_KEY(bad_kb, param[0]) & 0xFF);
    }
    return 0;
}

static int32_t
    ducky_parse_line(BadKbScript* bad_kb, FuriString* line, char* error, size_t error_len) {
    uint32_t line_len = furi_string_size(line);
    const char* line_tmp = furi_string_get_cstr(line);
    bool state = false;

    if(line_len == 0) {
        return SCRIPT_STATE_NEXT_LINE; // Skip empty lines
    }

    FURI_LOG_D(WORKER_TAG, "line:%s", line_tmp);

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
        state = ducky_get_number(line_tmp, &bad_kb->defdelay);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid number %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_string, strlen(ducky_cmd_string)) == 0) {
        // STRING
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        state = ducky_string(bad_kb, line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid string %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_altchar, strlen(ducky_cmd_altchar)) == 0) {
        // ALTCHAR
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        ducky_numlock_on(bad_kb);
        state = ducky_altchar(bad_kb, line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid altchar %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(
        (strncmp(line_tmp, ducky_cmd_altstr_1, strlen(ducky_cmd_altstr_1)) == 0) ||
        (strncmp(line_tmp, ducky_cmd_altstr_2, strlen(ducky_cmd_altstr_2)) == 0)) {
        // ALTSTRING
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        ducky_numlock_on(bad_kb);
        state = ducky_altstring(bad_kb, line_tmp);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid altstring %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_repeat, strlen(ducky_cmd_repeat)) == 0) {
        // REPEAT
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        state = ducky_get_number(line_tmp, &bad_kb->repeat_cnt);
        if(!state && error != NULL) {
            snprintf(error, error_len, "Invalid number %s", line_tmp);
        }
        return (state) ? (0) : SCRIPT_STATE_ERROR;
    } else if(strncmp(line_tmp, ducky_cmd_sysrq, strlen(ducky_cmd_sysrq)) == 0) {
        // SYSRQ
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        uint16_t key = ducky_get_keycode(bad_kb, line_tmp, true);
        if(bad_kb->bt) {
            furi_hal_bt_hid_kb_press(KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
            furi_hal_bt_hid_kb_press(key);
            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(key);
            furi_hal_bt_hid_kb_release(KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
        } else {
            furi_hal_hid_kb_press(KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
            furi_hal_hid_kb_press(key);
            furi_hal_hid_kb_release_all();
        }
        return (0);
    } else {
        // Special keys + modifiers
        uint16_t key = ducky_get_keycode(bad_kb, line_tmp, false);
        if(key == HID_KEYBOARD_NONE) {
            if(error != NULL) {
                snprintf(error, error_len, "No keycode defined for %s", line_tmp);
            }
            return SCRIPT_STATE_ERROR;
        }
        if((key & 0xFF00) != 0) {
            // It's a modifier key
            line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
            key |= ducky_get_keycode(bad_kb, line_tmp, true);
        }
        if(bad_kb->bt) {
            furi_hal_bt_hid_kb_press(key);
            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(key);
        } else {
            furi_hal_hid_kb_press(key);
            furi_hal_hid_kb_release(key);
        }
        return (0);
    }
}

static bool ducky_set_usb_id(BadKbScript* bad_kb, const char* line) {
    if(sscanf(line, "%lX:%lX", &bad_kb->hid_cfg.vid, &bad_kb->hid_cfg.pid) == 2) {
        bad_kb->hid_cfg.manuf[0] = '\0';
        bad_kb->hid_cfg.product[0] = '\0';

        uint8_t id_len = ducky_get_command_len(line);
        if(!ducky_is_line_end(line[id_len + 1])) {
            sscanf(
                &line[id_len + 1],
                "%31[^\r\n:]:%31[^\r\n]",
                bad_kb->hid_cfg.manuf,
                bad_kb->hid_cfg.product);
        }
        FURI_LOG_D(
            WORKER_TAG,
            "set id: %04lX:%04lX mfr:%s product:%s",
            bad_kb->hid_cfg.vid,
            bad_kb->hid_cfg.pid,
            bad_kb->hid_cfg.manuf,
            bad_kb->hid_cfg.product);
        return true;
    }
    return false;
}

static bool ducky_script_preload(BadKbScript* bad_kb, File* script_file) {
    uint8_t ret = 0;
    uint32_t line_len = 0;

    furi_string_reset(bad_kb->line);

    do {
        ret = storage_file_read(script_file, bad_kb->file_buf, FILE_BUFFER_LEN);
        for(uint16_t i = 0; i < ret; i++) {
            if(bad_kb->file_buf[i] == '\n' && line_len > 0) {
                bad_kb->st.line_nb++;
                line_len = 0;
            } else {
                if(bad_kb->st.line_nb == 0) { // Save first line
                    furi_string_push_back(bad_kb->line, bad_kb->file_buf[i]);
                }
                line_len++;
            }
        }
        if(storage_file_eof(script_file)) {
            if(line_len > 0) {
                bad_kb->st.line_nb++;
                break;
            }
        }
    } while(ret > 0);

    if(!bad_kb->bt) {
        const char* line_tmp = furi_string_get_cstr(bad_kb->line);
        bool id_set = false; // Looking for ID command at first line
        if(strncmp(line_tmp, ducky_cmd_id, strlen(ducky_cmd_id)) == 0) {
            id_set = ducky_set_usb_id(bad_kb, &line_tmp[strlen(ducky_cmd_id) + 1]);
        }

        if(id_set) {
            furi_check(furi_hal_usb_set_config(&usb_hid, &bad_kb->hid_cfg));
        } else {
            furi_check(furi_hal_usb_set_config(&usb_hid, NULL));
        }
    }

    storage_file_seek(script_file, 0, true);
    furi_string_reset(bad_kb->line);

    return true;
}

static int32_t ducky_script_execute_next(BadKbScript* bad_kb, File* script_file) {
    int32_t delay_val = 0;

    if(bad_kb->repeat_cnt > 0) {
        bad_kb->repeat_cnt--;
        delay_val = ducky_parse_line(
            bad_kb, bad_kb->line_prev, bad_kb->st.error, sizeof(bad_kb->st.error));
        if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
            return 0;
        } else if(delay_val < 0) { // Script error
            bad_kb->st.error_line = bad_kb->st.line_cur - 1;
            FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_kb->st.line_cur - 1U);
            return SCRIPT_STATE_ERROR;
        } else {
            return (delay_val + bad_kb->defdelay);
        }
    }

    furi_string_set(bad_kb->line_prev, bad_kb->line);
    furi_string_reset(bad_kb->line);

    while(1) {
        if(bad_kb->buf_len == 0) {
            bad_kb->buf_len = storage_file_read(script_file, bad_kb->file_buf, FILE_BUFFER_LEN);
            if(storage_file_eof(script_file)) {
                if((bad_kb->buf_len < FILE_BUFFER_LEN) && (bad_kb->file_end == false)) {
                    bad_kb->file_buf[bad_kb->buf_len] = '\n';
                    bad_kb->buf_len++;
                    bad_kb->file_end = true;
                }
            }

            bad_kb->buf_start = 0;
            if(bad_kb->buf_len == 0) return SCRIPT_STATE_END;
        }
        for(uint8_t i = bad_kb->buf_start; i < (bad_kb->buf_start + bad_kb->buf_len); i++) {
            if(bad_kb->file_buf[i] == '\n' && furi_string_size(bad_kb->line) > 0) {
                bad_kb->st.line_cur++;
                bad_kb->buf_len = bad_kb->buf_len + bad_kb->buf_start - (i + 1);
                bad_kb->buf_start = i + 1;
                furi_string_trim(bad_kb->line);
                delay_val = ducky_parse_line(
                    bad_kb, bad_kb->line, bad_kb->st.error, sizeof(bad_kb->st.error));
                if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
                    return 0;
                } else if(delay_val < 0) {
                    bad_kb->st.error_line = bad_kb->st.line_cur;
                    if(delay_val == SCRIPT_STATE_NEXT_LINE) {
                        snprintf(
                            bad_kb->st.error, sizeof(bad_kb->st.error), "Forbidden empty line");
                        FURI_LOG_E(
                            WORKER_TAG, "Forbidden empty line at line %u", bad_kb->st.line_cur);
                    } else {
                        FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_kb->st.line_cur);
                    }
                    return SCRIPT_STATE_ERROR;
                } else {
                    return (delay_val + bad_kb->defdelay);
                }
            } else {
                furi_string_push_back(bad_kb->line, bad_kb->file_buf[i]);
            }
        }
        bad_kb->buf_len = 0;
        if(bad_kb->file_end) return SCRIPT_STATE_END;
    }
}

static void bad_kb_bt_hid_state_callback(BtStatus status, void* context) {
    furi_assert(context);
    BadKbScript* bad_kb = (BadKbScript*)context;
    bool state = (status == BtStatusConnected);

    if(state == true) {
        LevelRssiRange r = bt_remote_rssi_range(bad_kb->bt);
        if(r != LevelRssiError) {
            bt_timeout = bt_hid_delays[r];
        }
        bt_connected = true;
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtConnect);
    } else {
        bt_connected = false;
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtDisconnect);
    }
}

static void bad_kb_usb_hid_state_callback(bool state, void* context) {
    furi_assert(context);
    BadKbScript* bad_kb = context;

    if(state == true) {
        usb_connected = true;
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtConnect);
    } else {
        usb_connected = false;
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtDisconnect);
    }
}

void bad_kb_bt_init(Bt* bt) {
    bt_timeout = bt_hid_delays[LevelRssi39_0];
    bt_disconnect(bt);
    furi_delay_ms(200);
    bt_keys_storage_set_storage_path(bt, HID_BT_KEYS_STORAGE_PATH);
    furi_assert(bt_set_profile(bt, BtProfileHidKeyboard));
    bt_mode_prev = bt_get_profile_pairing_method(bt);
    bt_set_profile_pairing_method(bt, GapPairingNone);
    furi_hal_bt_start_advertising();
    // disable peer key adding to bt SRAM storage
    bt_disable_peer_key_update(bt);

    connection_mode = BadKbConnectionModeBt;
}

void bad_kb_bt_deinit(Bt* bt) {
    // release all keys
    // bt_hid_hold_while_keyboard_buffer_full(6, 3000);

    // stop ble
    bt_disconnect(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_default_path(bt);

    bt_set_profile_pairing_method(bt, bt_mode_prev);

    // fails if ble radio stack isn't ready when switching profile
    // if it happens, maybe we should increase the delay after bt_disconnect
    bt_set_profile(bt, BtProfileSerial);

    // starts saving peer keys (bounded devices)
    bt_enable_peer_key_update(bt);

    connection_mode = BadKbConnectionModeNone;
}

void bad_kb_usb_init() {
    usb_mode_prev = furi_hal_usb_get_config();

    connection_mode = BadKbConnectionModeUsb;
}

void bad_kb_usb_deinit() {
    furi_hal_usb_set_config(usb_mode_prev, NULL);

    connection_mode = BadKbConnectionModeNone;
}

void bad_kb_connection_init(Bt* bt) {
    if(connection_mode != BadKbConnectionModeNone) return;

    if(bt) {
        bad_kb_bt_init(bt);
    } else {
        bad_kb_usb_init();
    }
}

void bad_kb_connection_deinit(Bt* bt) {
    if(connection_mode == BadKbConnectionModeNone) return;

    if(connection_mode == BadKbConnectionModeBt) {
        bad_kb_bt_deinit(bt);
    } else {
        bad_kb_usb_deinit();
    }
}

static int32_t bad_kb_worker(void* context) {
    BadKbScript* bad_kb = context;

    BadKbWorkerState worker_state = BadKbStateInit;
    int32_t delay_val = 0;

    bad_kb_connection_init(bad_kb->bt);

    if(bad_kb->bt) {
        bt_set_status_changed_callback(bad_kb->bt, bad_kb_bt_hid_state_callback, bad_kb);
    } else {
        furi_hal_hid_set_state_callback(bad_kb_usb_hid_state_callback, bad_kb);
    }

    FURI_LOG_I(WORKER_TAG, "Init");
    File* script_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    bad_kb->line = furi_string_alloc();
    bad_kb->line_prev = furi_string_alloc();

    while(1) {
        if(worker_state == BadKbStateInit) { // State: initialization
            if(storage_file_open(
                   script_file,
                   furi_string_get_cstr(bad_kb->file_path),
                   FSAM_READ,
                   FSOM_OPEN_EXISTING)) {
                if((ducky_script_preload(bad_kb, script_file)) && (bad_kb->st.line_nb > 0)) {
                    if(bad_kb->bt) {
                        worker_state = BadKbStateNotConnected; // Ready to run
                    } else {
                        if(furi_hal_hid_is_connected()) {
                            worker_state = BadKbStateIdle; // Ready to run
                        } else {
                            worker_state = BadKbStateNotConnected; // Not connected
                        }
                    }
                } else {
                    worker_state = BadKbStateScriptError; // Script preload error
                }
            } else {
                FURI_LOG_E(WORKER_TAG, "File open error");
                worker_state = BadKbStateFileError; // File open error
            }
            bad_kb->st.state = worker_state;

        } else if(worker_state == BadKbStateNotConnected) { // State: Not connected
            if((bad_kb->bt && bt_connected) || (!bad_kb->bt && usb_connected)) {
                worker_state = BadKbStateIdle; // Ready to run
            } else {
                uint32_t flags = furi_thread_flags_wait(
                    WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                    FuriFlagWaitAny,
                    FuriWaitForever);
                furi_check((flags & FuriFlagError) == 0);
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtConnect) {
                    worker_state = BadKbStateIdle; // Ready to run
                } else if(flags & WorkerEvtToggle) {
                    worker_state = BadKbStateWillRun; // Will run when connected
                }
            }
            bad_kb->st.state = worker_state;

        } else if(worker_state == BadKbStateIdle) { // State: ready to start
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtToggle | WorkerEvtDisconnect,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtToggle) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadKbPlayScript);
                delay_val = 0;
                bad_kb->buf_len = 0;
                bad_kb->st.line_cur = 0;
                bad_kb->defdelay = 0;
                bad_kb->repeat_cnt = 0;
                bad_kb->file_end = false;
                storage_file_seek(script_file, 0, true);
                bad_kb_script_set_keyboard_layout(bad_kb, bad_kb->keyboard_layout);
                worker_state = BadKbStateRunning;
            } else if(flags & WorkerEvtDisconnect) {
                worker_state = BadKbStateNotConnected; // Disconnected
            }
            bad_kb->st.state = worker_state;

        } else if(worker_state == BadKbStateWillRun) { // State: start on connection
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadKbPlayScript);
                delay_val = 0;
                bad_kb->buf_len = 0;
                bad_kb->st.line_cur = 0;
                bad_kb->defdelay = 0;
                bad_kb->repeat_cnt = 0;
                bad_kb->file_end = false;
                storage_file_seek(script_file, 0, true);
                // extra time for PC to recognize Flipper as keyboard
                furi_thread_flags_wait(0, FuriFlagWaitAny, 1500);
                if(bad_kb->bt) {
                    update_bt_timeout(bad_kb->bt);
                }
                bad_kb_script_set_keyboard_layout(bad_kb, bad_kb->keyboard_layout);
                worker_state = BadKbStateRunning;
            } else if(flags & WorkerEvtToggle) { // Cancel scheduled execution
                worker_state = BadKbStateNotConnected;
            }
            bad_kb->st.state = worker_state;

        } else if(worker_state == BadKbStateRunning) { // State: running
            uint16_t delay_cur = (delay_val > 1000) ? (1000) : (delay_val);
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtToggle | WorkerEvtDisconnect, FuriFlagWaitAny, delay_cur);
            delay_val -= delay_cur;
            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtToggle) {
                    worker_state = BadKbStateIdle; // Stop executing script
                    if(bad_kb->bt) {
                        furi_hal_bt_hid_kb_release_all();
                    } else {
                        furi_hal_hid_kb_release_all();
                    }
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadKbStateNotConnected; // Disconnected
                    if(bad_kb->bt) {
                        furi_hal_bt_hid_kb_release_all();
                    } else {
                        furi_hal_hid_kb_release_all();
                    }
                }
                bad_kb->st.state = worker_state;
                continue;
            } else if(
                (flags == (unsigned)FuriFlagErrorTimeout) ||
                (flags == (unsigned)FuriFlagErrorResource)) {
                if(delay_val > 0) {
                    bad_kb->st.delay_remain--;
                    continue;
                }
                bad_kb->st.state = BadKbStateRunning;
                delay_val = ducky_script_execute_next(bad_kb, script_file);
                if(delay_val == SCRIPT_STATE_ERROR) { // Script error
                    delay_val = 0;
                    worker_state = BadKbStateScriptError;
                    bad_kb->st.state = worker_state;
                } else if(delay_val == SCRIPT_STATE_END) { // End of script
                    delay_val = 0;
                    worker_state = BadKbStateIdle;
                    bad_kb->st.state = BadKbStateDone;
                    if(bad_kb->bt) {
                        furi_hal_bt_hid_kb_release_all();
                    } else {
                        furi_hal_hid_kb_release_all();
                    }
                    continue;
                } else if(delay_val > 1000) {
                    bad_kb->st.state = BadKbStateDelay; // Show long delays
                    bad_kb->st.delay_remain = delay_val / 1000;
                }
            } else {
                furi_check((flags & FuriFlagError) == 0);
            }

        } else if(
            (worker_state == BadKbStateFileError) ||
            (worker_state == BadKbStateScriptError)) { // State: error
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd, FuriFlagWaitAny, FuriWaitForever); // Waiting for exit command
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            }
        }
        if(bad_kb->bt) {
            update_bt_timeout(bad_kb->bt);
        }
    }

    if(bad_kb->bt) {
        bt_set_status_changed_callback(bad_kb->bt, NULL, NULL);
    } else {
        furi_hal_hid_set_state_callback(NULL, NULL);
    }

    storage_file_close(script_file);
    storage_file_free(script_file);
    furi_string_free(bad_kb->line);
    furi_string_free(bad_kb->line_prev);

    FURI_LOG_I(WORKER_TAG, "End");

    return 0;
}

static void bad_kb_script_set_default_keyboard_layout(BadKbScript* bad_kb) {
    furi_assert(bad_kb);
    furi_string_set_str(bad_kb->keyboard_layout, "");
    memset(bad_kb->layout, HID_KEYBOARD_NONE, sizeof(bad_kb->layout));
    memcpy(bad_kb->layout, hid_asciimap, MIN(sizeof(hid_asciimap), sizeof(bad_kb->layout)));
}

BadKbScript* bad_kb_script_open(FuriString* file_path, Bt* bt) {
    furi_assert(file_path);

    BadKbScript* bad_kb = malloc(sizeof(BadKbScript));
    bad_kb->file_path = furi_string_alloc();
    furi_string_set(bad_kb->file_path, file_path);
    bad_kb->keyboard_layout = furi_string_alloc();
    bad_kb_script_set_default_keyboard_layout(bad_kb);

    bad_kb->st.state = BadKbStateInit;
    bad_kb->st.error[0] = '\0';

    bad_kb->bt = bt;

    bad_kb->thread = furi_thread_alloc_ex("BadKbWorker", 2048, bad_kb_worker, bad_kb);
    furi_thread_start(bad_kb->thread);
    return bad_kb;
} //-V773

void bad_kb_script_close(BadKbScript* bad_kb) {
    furi_assert(bad_kb);
    furi_record_close(RECORD_STORAGE);
    furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtEnd);
    furi_thread_join(bad_kb->thread);
    furi_thread_free(bad_kb->thread);
    furi_string_free(bad_kb->file_path);
    furi_string_free(bad_kb->keyboard_layout);
    free(bad_kb);
}

void bad_kb_script_set_keyboard_layout(BadKbScript* bad_kb, FuriString* layout_path) {
    furi_assert(bad_kb);

    if((bad_kb->st.state == BadKbStateRunning) || (bad_kb->st.state == BadKbStateDelay)) {
        // do not update keyboard layout while a script is running
        return;
    }

    File* layout_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    if(!furi_string_empty(layout_path)) {
        furi_string_set(bad_kb->keyboard_layout, layout_path);
        if(storage_file_open(
               layout_file, furi_string_get_cstr(layout_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint16_t layout[128];
            if(storage_file_read(layout_file, layout, sizeof(layout)) == sizeof(layout)) {
                memcpy(bad_kb->layout, layout, sizeof(layout));
            }
        }
        storage_file_close(layout_file);
    } else {
        bad_kb_script_set_default_keyboard_layout(bad_kb);
    }
    storage_file_free(layout_file);
}

void bad_kb_script_toggle(BadKbScript* bad_kb) {
    furi_assert(bad_kb);
    furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtToggle);
}

BadKbState* bad_kb_script_get_state(BadKbScript* bad_kb) {
    furi_assert(bad_kb);
    return &(bad_kb->st);
}
