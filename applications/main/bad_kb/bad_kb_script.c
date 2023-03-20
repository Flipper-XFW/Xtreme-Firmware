#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/toolbox/args.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>
#include <storage/storage.h>
#include "bad_kb_script.h"
#include "mnemonic.h"
#include <dolphin/dolphin.h>

#include <bt/bt_service/bt.h>

#include "bad_kb_app_i.h"

#define HID_BT_KEYS_STORAGE_PATH EXT_PATH("apps/Tools/.bt_hid.keys")

#define TAG "BadKB"
#define WORKER_TAG TAG "Worker"

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
static const char ducky_cmd_stringln[] = {"STRINGLN "};
static const char ducky_cmd_defdelay_1[] = {"DEFAULT_DELAY "};
static const char ducky_cmd_defdelay_2[] = {"DEFAULTDELAY "};
static const char ducky_cmd_stringdelay_1[] = {"STRINGDELAY "};
static const char ducky_cmd_stringdelay_2[] = {"STRING_DELAY "};
static const char ducky_cmd_repeat[] = {"REPEAT "};
static const char ducky_cmd_sysrq[] = {"SYSRQ "};
static const char ducky_cmd_hold[] = {"HOLD "};
static const char ducky_cmd_release[] = {"RELEASE "};

static const char ducky_cmd_altchar[] = {"ALTCHAR "};
static const char ducky_cmd_altstr_1[] = {"ALTSTRING "};
static const char ducky_cmd_altstr_2[] = {"ALTCODE "};

uint8_t bt_timeout = 0;

static LevelRssiRange bt_remote_rssi_range(Bt* bt) {
    uint8_t rssi;

    if(!bt_remote_rssi(bt, &rssi)) return LevelRssiError;

    if(rssi <= 39)
        return LevelRssi39_0;
    else if(rssi <= 59)
        return LevelRssi59_40;
    else if(rssi <= 79)
        return LevelRssi79_60;
    else if(rssi <= 99)
        return LevelRssi99_80;
    else if(rssi <= 122)
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

uint32_t ducky_get_command_len(const char* line) {
    uint32_t len = strlen(line);
    for(uint32_t i = 0; i < len; i++) {
        if(line[i] == ' ') return i;
    }
    return 0;
}

bool ducky_is_line_end(const char chr) {
    return ((chr == ' ') || (chr == '\0') || (chr == '\r') || (chr == '\n'));
}

uint16_t ducky_get_keycode(BadKbScript* bad_kb, const char* param, bool accept_chars) {
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
    const char* ducky_cmd_table[] = {
        ducky_cmd_comment,
        ducky_cmd_id,
        ducky_cmd_delay,
        ducky_cmd_string,
        ducky_cmd_defdelay_1,
        ducky_cmd_defdelay_2,
        ducky_cmd_stringdelay_1,
        ducky_cmd_stringdelay_2,
        ducky_cmd_repeat,
        ducky_cmd_sysrq,
        ducky_cmd_altchar,
        ducky_cmd_altstr_1,
        ducky_cmd_altstr_2,
        ducky_cmd_stringln,
        ducky_cmd_hold,
        ducky_cmd_release,
        NULL};
    int32_t (*fnc_ptr[])(BadKbScript*, FuriString*, const char*, char*, size_t) = {
        &ducky_fnc_noop,
        &ducky_fnc_noop,
        &ducky_fnc_delay,
        &ducky_fnc_string,
        &ducky_fnc_defdelay,
        &ducky_fnc_defdelay,
        &ducky_fnc_strdelay,
        &ducky_fnc_strdelay,
        &ducky_fnc_repeat,
        &ducky_fnc_sysrq,
        &ducky_fnc_altchar,
        &ducky_fnc_altstring,
        &ducky_fnc_altstring,
        &ducky_fnc_stringln,
        &ducky_fnc_hold,
        &ducky_fnc_release,
        NULL};

    if(line_len == 0) {
        return SCRIPT_STATE_NEXT_LINE; // Skip empty lines
    }
    FURI_LOG_D(WORKER_TAG, "line:%s", line_tmp);
    // Ducky Lang Functions
    for(size_t i = 0; ducky_cmd_table[i]; i++) {
        if(strncmp(line_tmp, ducky_cmd_table[i], strlen(ducky_cmd_table[i])) == 0)
            return ((fnc_ptr[i])(bad_kb, line, line_tmp, error, error_len));
    }
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
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtConnect);
    } else {
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtDisconnect);
    }
}

static void bad_kb_usb_hid_state_callback(bool state, void* context) {
    furi_assert(context);
    BadKbScript* bad_kb = context;

    if(state == true) {
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtConnect);
    } else {
        furi_thread_flags_set(furi_thread_get_id(bad_kb->thread), WorkerEvtDisconnect);
    }
}

void bad_kb_reload_worker(BadKbApp* app) {
    bad_kb_script_close(app->bad_kb_script);
    app->bad_kb_script = bad_kb_script_open(app->file_path, app->is_bt ? app->bt : NULL);
    bad_kb_script_set_keyboard_layout(app->bad_kb_script, app->keyboard_layout);
}

void bad_kb_config_switch_mode(BadKbApp* app) {
    scene_manager_previous_scene(app->scene_manager);
    if(app->is_bt) {
        furi_hal_bt_start_advertising();
    } else {
        furi_hal_bt_stop_advertising();
    }
    scene_manager_next_scene(app->scene_manager, BadKbSceneConfig);
    bad_kb_reload_worker(app);
}

void bad_kb_config_switch_remember_mode(BadKbApp* app) {
    if(app->bt_remember) {
        // set bouding mac
        uint8_t mac[6] = BAD_KB_BOUND_MAC_ADDRESS;
        furi_hal_bt_set_profile_pairing_method(
            FuriHalBtProfileHidKeyboard, GapPairingPinCodeVerifyYesNo);
        bt_set_profile_mac_address(app->bt, mac); // this also restart bt
        // enable keys storage
        bt_enable_peer_key_update(app->bt);
    } else {
        // set back user defined mac address
        furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, GapPairingNone);
        bt_set_profile_mac_address(app->bt, app->mac);
        // disable key storage
        bt_disable_peer_key_update(app->bt);
    }
    bad_kb_reload_worker(app);
}

int32_t bad_kb_connection_init(BadKbApp* app) {
    app->usb_prev_mode = furi_hal_usb_get_config();
    furi_hal_usb_set_config(NULL, NULL);

    bt_timeout = bt_hid_delays[LevelRssi39_0];
    bt_disconnect(app->bt);
    // furi_delay_ms(200);
    bt_keys_storage_set_storage_path(app->bt, BAD_KB_APP_PATH_BOUND_KEYS_FILE);
    app->bt_prev_mode = furi_hal_bt_get_profile_pairing_method(FuriHalBtProfileHidKeyboard);
    if(app->bt_remember) {
        uint8_t mac[6] = BAD_KB_BOUND_MAC_ADDRESS;
        furi_hal_bt_set_profile_mac_addr(FuriHalBtProfileHidKeyboard, mac);
        // using GapPairingNone breaks bounding between devices
        furi_hal_bt_set_profile_pairing_method(
            FuriHalBtProfileHidKeyboard, GapPairingPinCodeVerifyYesNo);
    } else {
        furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, GapPairingNone);
    }

    bt_set_profile(app->bt, BtProfileHidKeyboard);
    if(app->is_bt) {
        furi_hal_bt_start_advertising();
        if(app->bt_remember) {
            bt_enable_peer_key_update(app->bt);
        } else {
            bt_disable_peer_key_update(app->bt); // disable peer key adding to bt SRAM storage
        }
    } else {
        furi_hal_bt_stop_advertising();
    }

    return 0;
}

void bad_kb_connection_deinit(BadKbApp* app) {
    furi_hal_usb_set_config(app->usb_prev_mode, NULL);

    // bt_hid_hold_while_keyboard_buffer_full(6, 3000); // release all keys
    bt_disconnect(app->bt); // stop ble
    // furi_delay_ms(200); // Wait 2nd core to update nvm storage
    bt_keys_storage_set_default_path(app->bt);
    if(app->bt_remember) {
        // hal primitives doesn't restarts ble, that's what we want cuz we are shutting down
        furi_hal_bt_set_profile_mac_addr(FuriHalBtProfileHidKeyboard, app->mac);
    }
    bt_enable_peer_key_update(app->bt); // starts saving peer keys (bounded devices)
    // fails if ble radio stack isn't ready when switching profile
    // if it happens, maybe we should increase the delay after bt_disconnect
    bt_set_profile(app->bt, BtProfileSerial);
    furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, app->bt_prev_mode);
}

static uint32_t bad_kb_flags_get(uint32_t flags_mask, uint32_t timeout) {
    uint32_t flags = furi_thread_flags_get();
    furi_check((flags & FuriFlagError) == 0);
    if(flags == 0) {
        flags = furi_thread_flags_wait(flags_mask, FuriFlagWaitAny, timeout);
        furi_check(((flags & FuriFlagError) == 0) || (flags == (unsigned)FuriFlagErrorTimeout));
    } else {
        uint32_t state = furi_thread_flags_clear(flags);
        furi_check((state & FuriFlagError) == 0);
    }
    return flags;
}

static int32_t bad_kb_worker(void* context) {
    BadKbScript* bad_kb = context;

    BadKbWorkerState worker_state = BadKbStateInit;
    int32_t delay_val = 0;

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
                        if(furi_hal_bt_is_connected()) {
                            worker_state = BadKbStateIdle; // Ready to run
                        } else {
                            worker_state = BadKbStateNotConnected; // Not connected
                        }
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
            uint32_t flags = bad_kb_flags_get(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle, FuriWaitForever);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) {
                worker_state = BadKbStateIdle; // Ready to run
            } else if(flags & WorkerEvtToggle) {
                worker_state = BadKbStateWillRun; // Will run when connected
            }
            bad_kb->st.state = worker_state;

        } else if(worker_state == BadKbStateIdle) { // State: ready to start
            uint32_t flags = bad_kb_flags_get(
                WorkerEvtEnd | WorkerEvtToggle | WorkerEvtDisconnect, FuriWaitForever);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtToggle) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadKbPlayScript);
                delay_val = 0;
                bad_kb->buf_len = 0;
                bad_kb->st.line_cur = 0;
                bad_kb->defdelay = 0;
                bad_kb->stringdelay = 0;
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
            uint32_t flags = bad_kb_flags_get(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle, FuriWaitForever);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) { // Start executing script
                DOLPHIN_DEED(DolphinDeedBadKbPlayScript);
                delay_val = 0;
                bad_kb->buf_len = 0;
                bad_kb->st.line_cur = 0;
                bad_kb->defdelay = 0;
                bad_kb->stringdelay = 0;
                bad_kb->repeat_cnt = 0;
                bad_kb->file_end = false;
                storage_file_seek(script_file, 0, true);
                // extra time for PC to recognize Flipper as keyboard
                flags = furi_thread_flags_wait(
                    WorkerEvtEnd | WorkerEvtDisconnect | WorkerEvtToggle,
                    FuriFlagWaitAny | FuriFlagNoClear,
                    1500);
                if(flags == (unsigned)FuriFlagErrorTimeout) {
                    // If nothing happened - start script execution
                    worker_state = BadKbStateRunning;
                } else if(flags & WorkerEvtToggle) {
                    worker_state = BadKbStateIdle;
                    furi_thread_flags_clear(WorkerEvtToggle);
                }
                if(bad_kb->bt) {
                    update_bt_timeout(bad_kb->bt);
                }
                bad_kb_script_set_keyboard_layout(bad_kb, bad_kb->keyboard_layout);
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
            uint32_t flags =
                bad_kb_flags_get(WorkerEvtEnd, FuriWaitForever); // Waiting for exit command
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
    bad_kb->st.is_bt = !!bt;

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
    if(!furi_string_empty(layout_path)) { //-V1051
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
