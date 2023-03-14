#include <furi_hal.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>
#include "mnemonic.h"

#define TAG "BadKB"
#define WORKER_TAG TAG "Worker"

#define FILE_BUFFER_LEN 16
#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)

#define BADKB_ASCII_TO_KEY(script, x) \
    (((uint8_t)x < 128) ? (script->layout[(uint8_t)x]) : HID_KEYBOARD_NONE)

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

static bool ducky_get_number(const char* param, uint32_t* val) {
    uint32_t value = 0;
    if(sscanf(param, "%lu", &value) == 1) {
        *val = value;
        return true;
    }
    return false;
}

static void ducky_numlock_on(BadKbScript* bad_kb) {
    if(bad_kb->bt) {
        if((furi_hal_bt_hid_get_led_state() & HID_KB_LED_NUM) == 0) {
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
            if(bad_kb->stringdelay > 0) {
                furi_delay_ms(bad_kb->stringdelay);
            }
        }
        i++;
    }
    bad_kb->stringdelay = 0;
    return true;
}

int32_t ducky_fnc_noop(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    (void)bad_kb;
    (void)line;
    (void)line_tmp;
    (void)error;
    (void)error_len;
    return (0);
}

int32_t ducky_fnc_delay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)bad_kb;
    (void)line;

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
}

int32_t ducky_fnc_defdelay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    state = ducky_get_number(line_tmp, &bad_kb->defdelay);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid number %s", line_tmp);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_strdelay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    state = ducky_get_number(line_tmp, &bad_kb->stringdelay);
    if((state) && (bad_kb->stringdelay > 0)) {
        return state;
    }
    if(error != NULL) {
        snprintf(error, error_len, "Invalid number %s", line_tmp);
    }
    return SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_string(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    state = ducky_string(bad_kb, line_tmp);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid string %s", line_tmp);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_repeat(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    state = ducky_get_number(line_tmp, &bad_kb->repeat_cnt);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid number %s", line_tmp);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_sysrq(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    (void)error;
    (void)error_len;
    (void)line;
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
}

int32_t ducky_fnc_altchar(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)bad_kb;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    ducky_numlock_on(bad_kb);
    state = ducky_altchar(bad_kb, line_tmp);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid altchar %s", line_tmp);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_altstring(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)bad_kb;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    ducky_numlock_on(bad_kb);
    state = ducky_altstring(bad_kb, line_tmp);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid altstring %s", line_tmp);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_stringln(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    bool state = false;
    (void)line;

    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    state = ducky_string(bad_kb, line_tmp);
    if(!state && error != NULL) {
        snprintf(error, error_len, "Invalid string %s", line_tmp);
    }
    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_press(HID_KEYBOARD_RETURN);
        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(HID_KEYBOARD_RETURN);
    } else {
        furi_hal_hid_kb_press(HID_KEYBOARD_RETURN);
        furi_hal_hid_kb_release(HID_KEYBOARD_RETURN);
    }
    return (state) ? (0) : SCRIPT_STATE_ERROR;
}

int32_t ducky_fnc_hold(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    (void)line;
    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    uint16_t key = ducky_get_keycode(bad_kb, line_tmp, true);
    if(key == HID_KEYBOARD_NONE) {
        if(error != NULL) {
            snprintf(error, error_len, "No keycode defined for %s", line_tmp);
        }
        return SCRIPT_STATE_ERROR;
    }
    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_press(key);
    } else {
        furi_hal_hid_kb_press(key);
    }
    return (0);
}

int32_t ducky_fnc_release(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len) {
    (void)line;
    line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
    uint16_t key = ducky_get_keycode(bad_kb, line_tmp, true);
    if(key == HID_KEYBOARD_NONE) {
        if(error != NULL) {
            snprintf(error, error_len, "No keycode defined for %s", line_tmp);
        }
        return SCRIPT_STATE_ERROR;
    }
    if(bad_kb->bt) {
        furi_hal_bt_hid_kb_release(key);
    } else {
        furi_hal_hid_kb_release(key);
    }
    return (0);
}
