#pragma once
#include "hid_usage_desktop.h"
#include "hid_usage_button.h"
#include "hid_usage_keyboard.h"
#include "hid_usage_consumer.h"
#include "hid_usage_led.h"

#define HID_KEYBOARD_NONE 0x00
// Remapping the colon key which is shift + ; to comma
#define HID_KEYBOARD_COMMA HID_KEYBOARD_COLON

/** HID keyboard modifier keys */
enum HidKeyboardMods {
    KEY_MOD_LEFT_CTRL = (1 << 8),
    KEY_MOD_LEFT_SHIFT = (1 << 9),
    KEY_MOD_LEFT_ALT = (1 << 10),
    KEY_MOD_LEFT_GUI = (1 << 11),
    KEY_MOD_RIGHT_CTRL = (1 << 12),
    KEY_MOD_RIGHT_SHIFT = (1 << 13),
    KEY_MOD_RIGHT_ALT = (1 << 14),
    KEY_MOD_RIGHT_GUI = (1 << 15),
};

/** ASCII to keycode conversion table US */
static const uint16_t hid_asciimap[] = {
    HID_KEYBOARD_NONE, // NUL
    HID_KEYBOARD_NONE, // SOH
    HID_KEYBOARD_NONE, // STX
    HID_KEYBOARD_NONE, // ETX
    HID_KEYBOARD_NONE, // EOT
    HID_KEYBOARD_NONE, // ENQ
    HID_KEYBOARD_NONE, // ACK
    HID_KEYBOARD_NONE, // BEL
    HID_KEYBOARD_DELETE, // BS   Backspace
    HID_KEYBOARD_TAB, // TAB  Tab
    HID_KEYBOARD_RETURN, // LF   Enter
    HID_KEYBOARD_NONE, // VT
    HID_KEYBOARD_NONE, // FF
    HID_KEYBOARD_NONE, // CR
    HID_KEYBOARD_NONE, // SO
    HID_KEYBOARD_NONE, // SI
    HID_KEYBOARD_NONE, // DEL
    HID_KEYBOARD_NONE, // DC1
    HID_KEYBOARD_NONE, // DC2
    HID_KEYBOARD_NONE, // DC3
    HID_KEYBOARD_NONE, // DC4
    HID_KEYBOARD_NONE, // NAK
    HID_KEYBOARD_NONE, // SYN
    HID_KEYBOARD_NONE, // ETB
    HID_KEYBOARD_NONE, // CAN
    HID_KEYBOARD_NONE, // EM
    HID_KEYBOARD_NONE, // SUB
    HID_KEYBOARD_NONE, // ESC
    HID_KEYBOARD_NONE, // FS
    HID_KEYBOARD_NONE, // GS
    HID_KEYBOARD_NONE, // RS
    HID_KEYBOARD_NONE, // US
    HID_KEYBOARD_SPACEBAR, // ' ' Space
    HID_KEYBOARD_1 | KEY_MOD_LEFT_SHIFT, // !
    HID_KEYBOARD_APOSTROPHE | KEY_MOD_LEFT_SHIFT, // "
    HID_KEYBOARD_3 | KEY_MOD_LEFT_SHIFT, // #
    HID_KEYBOARD_4 | KEY_MOD_LEFT_SHIFT, // $
    HID_KEYBOARD_5 | KEY_MOD_LEFT_SHIFT, // %
    HID_KEYBOARD_7 | KEY_MOD_LEFT_SHIFT, // &
    HID_KEYBOARD_APOSTROPHE, // '
    HID_KEYBOARD_9 | KEY_MOD_LEFT_SHIFT, // (
    HID_KEYBOARD_0 | KEY_MOD_LEFT_SHIFT, // )
    HID_KEYBOARD_8 | KEY_MOD_LEFT_SHIFT, // *
    HID_KEYBOARD_EQUAL_SIGN | KEY_MOD_LEFT_SHIFT, // +
    HID_KEYBOARD_COMMA, // ,
    HID_KEYBOARD_MINUS, // -
    HID_KEYBOARD_DOT, // .
    HID_KEYBOARD_SLASH, // /
    HID_KEYBOARD_0, // 0
    HID_KEYBOARD_1, // 1
    HID_KEYBOARD_2, // 2
    HID_KEYBOARD_3, // 3
    HID_KEYBOARD_4, // 4
    HID_KEYBOARD_5, // 5
    HID_KEYBOARD_6, // 6
    HID_KEYBOARD_7, // 7
    HID_KEYBOARD_8, // 8
    HID_KEYBOARD_9, // 9
    HID_KEYBOARD_SEMICOLON | KEY_MOD_LEFT_SHIFT, // :
    HID_KEYBOARD_SEMICOLON, // ;
    HID_KEYBOARD_COMMA | KEY_MOD_LEFT_SHIFT, // <
    HID_KEYBOARD_EQUAL_SIGN, // =
    HID_KEYBOARD_DOT | KEY_MOD_LEFT_SHIFT, // >
    HID_KEYBOARD_SLASH | KEY_MOD_LEFT_SHIFT, // ?
    HID_KEYBOARD_2 | KEY_MOD_LEFT_SHIFT, // @
    HID_KEYBOARD_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_D | KEY_MOD_LEFT_SHIFT, // D
    HID_KEYBOARD_E | KEY_MOD_LEFT_SHIFT, // E
    HID_KEYBOARD_F | KEY_MOD_LEFT_SHIFT, // F
    HID_KEYBOARD_G | KEY_MOD_LEFT_SHIFT, // G
    HID_KEYBOARD_H | KEY_MOD_LEFT_SHIFT, // H
    HID_KEYBOARD_I | KEY_MOD_LEFT_SHIFT, // I
    HID_KEYBOARD_J | KEY_MOD_LEFT_SHIFT, // J
    HID_KEYBOARD_K | KEY_MOD_LEFT_SHIFT, // K
    HID_KEYBOARD_L | KEY_MOD_LEFT_SHIFT, // L
    HID_KEYBOARD_M | KEY_MOD_LEFT_SHIFT, // M
    HID_KEYBOARD_N | KEY_MOD_LEFT_SHIFT, // N
    HID_KEYBOARD_O | KEY_MOD_LEFT_SHIFT, // O
    HID_KEYBOARD_P | KEY_MOD_LEFT_SHIFT, // P
    HID_KEYBOARD_Q | KEY_MOD_LEFT_SHIFT, // Q
    HID_KEYBOARD_R | KEY_MOD_LEFT_SHIFT, // R
    HID_KEYBOARD_S | KEY_MOD_LEFT_SHIFT, // S
    HID_KEYBOARD_T | KEY_MOD_LEFT_SHIFT, // T
    HID_KEYBOARD_U | KEY_MOD_LEFT_SHIFT, // U
    HID_KEYBOARD_V | KEY_MOD_LEFT_SHIFT, // V
    HID_KEYBOARD_W | KEY_MOD_LEFT_SHIFT, // W
    HID_KEYBOARD_X | KEY_MOD_LEFT_SHIFT, // X
    HID_KEYBOARD_Y | KEY_MOD_LEFT_SHIFT, // Y
    HID_KEYBOARD_Z | KEY_MOD_LEFT_SHIFT, // Z
    HID_KEYBOARD_OPEN_BRACKET, // [
    HID_KEYBOARD_BACKSLASH, // bslash
    HID_KEYBOARD_CLOSE_BRACKET, // ]
    HID_KEYBOARD_6 | KEY_MOD_LEFT_SHIFT, // ^
    HID_KEYBOARD_MINUS | KEY_MOD_LEFT_SHIFT, // _
    HID_KEYBOARD_GRAVE_ACCENT, // `
    HID_KEYBOARD_A, // a
    HID_KEYBOARD_B, // b
    HID_KEYBOARD_C, // c
    HID_KEYBOARD_D, // d
    HID_KEYBOARD_E, // e
    HID_KEYBOARD_F, // f
    HID_KEYBOARD_G, // g
    HID_KEYBOARD_H, // h
    HID_KEYBOARD_I, // i
    HID_KEYBOARD_J, // j
    HID_KEYBOARD_K, // k
    HID_KEYBOARD_L, // l
    HID_KEYBOARD_M, // m
    HID_KEYBOARD_N, // n
    HID_KEYBOARD_O, // o
    HID_KEYBOARD_P, // p
    HID_KEYBOARD_Q, // q
    HID_KEYBOARD_R, // r
    HID_KEYBOARD_S, // s
    HID_KEYBOARD_T, // t
    HID_KEYBOARD_U, // u
    HID_KEYBOARD_V, // v
    HID_KEYBOARD_W, // w
    HID_KEYBOARD_X, // x
    HID_KEYBOARD_Y, // y
    HID_KEYBOARD_Z, // z
    HID_KEYBOARD_OPEN_BRACKET | KEY_MOD_LEFT_SHIFT, // {
    HID_KEYBOARD_BACKSLASH | KEY_MOD_LEFT_SHIFT, // |
    HID_KEYBOARD_CLOSE_BRACKET | KEY_MOD_LEFT_SHIFT, // }
    HID_KEYBOARD_GRAVE_ACCENT | KEY_MOD_LEFT_SHIFT, // ~
    HID_KEYBOARD_NONE, // DEL
};

/** HID keyboard key codes DE */
enum HidKeyboardKeysDE {
    HID_KEYBOARD_DE_ERROR_ROLLOVER = 0x01,
    HID_KEYBOARD_DE_POST_FAIL = 0x02,
    HID_KEYBOARD_DE_ERROR_UNDEFINED = 0x03,

    HID_KEYBOARD_DE_ENTER = 0x28,
    HID_KEYBOARD_DE_ESC = 0x29,
    HID_KEYBOARD_DE_BACKSPACE = 0x2A,
    HID_KEYBOARD_DE_TAB = 0x2B,
    HID_KEYBOARD_DE_SPACE = 0x2C,

    HID_KEYBOARD_DE_A = 0x04,
    HID_KEYBOARD_DE_B = 0x05,
    HID_KEYBOARD_DE_C = 0x06,
    HID_KEYBOARD_DE_D = 0x07,
    HID_KEYBOARD_DE_E = 0x08,
    HID_KEYBOARD_DE_F = 0x09,
    HID_KEYBOARD_DE_G = 0x0A,
    HID_KEYBOARD_DE_H = 0x0B,
    HID_KEYBOARD_DE_I = 0x0C,
    HID_KEYBOARD_DE_J = 0x0D,
    HID_KEYBOARD_DE_K = 0x0E,
    HID_KEYBOARD_DE_L = 0x0F,
    HID_KEYBOARD_DE_M = 0x10,
    HID_KEYBOARD_DE_N = 0x11,
    HID_KEYBOARD_DE_O = 0x12,
    HID_KEYBOARD_DE_P = 0x13,
    HID_KEYBOARD_DE_Q = 0x14,
    HID_KEYBOARD_DE_R = 0x15,
    HID_KEYBOARD_DE_S = 0x16,
    HID_KEYBOARD_DE_T = 0x17,
    HID_KEYBOARD_DE_U = 0x18,
    HID_KEYBOARD_DE_V = 0x19,
    HID_KEYBOARD_DE_W = 0x1A,
    HID_KEYBOARD_DE_X = 0x1B,
    HID_KEYBOARD_DE_Y = 0x1D,
    HID_KEYBOARD_DE_Z = 0x1C,

    HID_KEYBOARD_DE_1 = 0x1E,
    HID_KEYBOARD_DE_2 = 0x1F,
    HID_KEYBOARD_DE_3 = 0x20,
    HID_KEYBOARD_DE_4 = 0x21,
    HID_KEYBOARD_DE_5 = 0x22,
    HID_KEYBOARD_DE_6 = 0x23,
    HID_KEYBOARD_DE_7 = 0x24,
    HID_KEYBOARD_DE_8 = 0x25,
    HID_KEYBOARD_DE_9 = 0x26,
    HID_KEYBOARD_DE_0 = 0x27,

    HID_KEYBOARD_DE_EXCLAMATION = 0x1E,
    HID_KEYBOARD_DE_DOUBLE_QUOTE = 0x1F,
    HID_KEYBOARD_DE_DOLLAR = 0x21,
    HID_KEYBOARD_DE_PERCENT = 0x22,
    HID_KEYBOARD_DE_AND = 0x23,
    HID_KEYBOARD_DE_SINGLE_QUOTE = 0x31, //tocheck
    HID_KEYBOARD_DE_LEFT_PARENTHESIS = 0x25,
    HID_KEYBOARD_DE_RIGHT_PARENTHESIS = 0x26,
    HID_KEYBOARD_DE_STAR = 0x30,
    HID_KEYBOARD_DE_EQUAL = 0x27,
    HID_KEYBOARD_DE_COMMA = 0x36,
    HID_KEYBOARD_DE_DASH = 0x38,
    HID_KEYBOARD_DE_SEMI_COLON = 0x36,
    HID_KEYBOARD_DE_DOUBLE_POINTS = 0x37,
    HID_KEYBOARD_DE_SMALLER = 0x64, //todo
    HID_KEYBOARD_DE_UNDERSCORE = 0x38,
    HID_KEYBOARD_DE_CIRCUMFLEX = 0x35, //tocheck
    HID_KEYBOARD_DE_BACKTICK = 0x2E,

    HID_KEYBOARD_DE_CAPS_LOCK = 0xC1,
    HID_KEYBOARD_DE_F1 = 0xC2,
    HID_KEYBOARD_DE_F2 = 0xC3,
    HID_KEYBOARD_DE_F3 = 0xC4,
    HID_KEYBOARD_DE_F4 = 0xC5,
    HID_KEYBOARD_DE_F5 = 0xC6,
    HID_KEYBOARD_DE_F6 = 0xC7,
    HID_KEYBOARD_DE_F7 = 0xC8,
    HID_KEYBOARD_DE_F8 = 0xC9,
    HID_KEYBOARD_DE_F9 = 0xCA,
    HID_KEYBOARD_DE_F10 = 0xCB,
    HID_KEYBOARD_DE_F11 = 0xCC,
    HID_KEYBOARD_DE_F12 = 0xCD,
    HID_KEYBOARD_DE_PRINT = 0x63,
    HID_KEYBOARD_DE_SCROLL_LOCK = 0x47,
    HID_KEYBOARD_DE_PAUSE = 0x48,
    HID_KEYBOARD_DE_INSERT = 0xD1,
    HID_KEYBOARD_DE_HOME = 0xD2,
    HID_KEYBOARD_DE_PAGE_UP = 0xD3,
    HID_KEYBOARD_DE_DELETE = 0xD4,
    HID_KEYBOARD_DE_END = 0xD5,
    HID_KEYBOARD_DE_PAGE_DOWN = 0xD6,
    HID_KEYBOARD_DE_RIGHT_ARROW = 0xD7,
    HID_KEYBOARD_DE_LEFT_ARROW = 0xD8,
    HID_KEYBOARD_DE_DOWN_ARROW = 0xD9,
    HID_KEYBOARD_DE_UP_ARROW = 0xDA,
    HID_KEYBOARD_DE_NUM_LOCK = 0x53,
    HID_KEYBOARD_DE_NON_US = 0x64,
    HID_KEYBOARD_DE_APPLICATION = 0x65,

    HID_KEYBOARD_DE_SHARP_SS = 0x2D,
};

/** ASCII to keycode conversion table DE */
static const uint16_t hid_asciimap_de[] = {
    HID_KEYBOARD_NONE, // NUL
    HID_KEYBOARD_NONE, // SOH
    HID_KEYBOARD_NONE, // STX
    HID_KEYBOARD_NONE, // ETX
    HID_KEYBOARD_NONE, // EOT
    HID_KEYBOARD_NONE, // ENQ
    HID_KEYBOARD_NONE, // ACK
    HID_KEYBOARD_NONE, // BEL
    HID_KEYBOARD_DE_BACKSPACE, // BS   Backspace
    HID_KEYBOARD_DE_TAB, // TAB  Tab
    HID_KEYBOARD_DE_ENTER, // LF   Enter
    HID_KEYBOARD_NONE, // VT
    HID_KEYBOARD_NONE, // FF
    HID_KEYBOARD_NONE, // CR
    HID_KEYBOARD_NONE, // SO
    HID_KEYBOARD_NONE, // SI
    HID_KEYBOARD_NONE, // DEL
    HID_KEYBOARD_NONE, // DC1
    HID_KEYBOARD_NONE, // DC2
    HID_KEYBOARD_NONE, // DC3
    HID_KEYBOARD_NONE, // DC4
    HID_KEYBOARD_NONE, // NAK
    HID_KEYBOARD_NONE, // SYN
    HID_KEYBOARD_NONE, // ETB
    HID_KEYBOARD_NONE, // CAN
    HID_KEYBOARD_NONE, // EM
    HID_KEYBOARD_NONE, // SUB
    HID_KEYBOARD_NONE, // ESC
    HID_KEYBOARD_NONE, // FS
    HID_KEYBOARD_NONE, // GS
    HID_KEYBOARD_NONE, // RS
    HID_KEYBOARD_NONE, // US
    HID_KEYBOARD_DE_SPACE, // ' ' Space
    HID_KEYBOARD_DE_EXCLAMATION | KEY_MOD_LEFT_SHIFT, // !
    HID_KEYBOARD_DE_DOUBLE_QUOTE | KEY_MOD_LEFT_SHIFT, // "
    HID_KEYBOARD_DE_SINGLE_QUOTE, // #
    HID_KEYBOARD_DE_DOLLAR | KEY_MOD_LEFT_SHIFT, // $
    HID_KEYBOARD_DE_PERCENT | KEY_MOD_LEFT_SHIFT, // %
    HID_KEYBOARD_DE_AND | KEY_MOD_LEFT_SHIFT, // &
    HID_KEYBOARD_DE_SINGLE_QUOTE | KEY_MOD_LEFT_SHIFT, // '
    HID_KEYBOARD_DE_LEFT_PARENTHESIS | KEY_MOD_LEFT_SHIFT, // (
    HID_KEYBOARD_DE_RIGHT_PARENTHESIS | KEY_MOD_LEFT_SHIFT, // )
    HID_KEYBOARD_DE_STAR | KEY_MOD_LEFT_SHIFT, // *
    HID_KEYBOARD_DE_STAR, // +
    HID_KEYBOARD_DE_COMMA, // ,
    HID_KEYBOARD_DE_DASH, // -
    HID_KEYBOARD_DE_DOUBLE_POINTS, // .
    HID_KEYBOARD_DE_7 | KEY_MOD_LEFT_SHIFT, // /
    HID_KEYBOARD_DE_0, // 0
    HID_KEYBOARD_DE_1, // 1
    HID_KEYBOARD_DE_2, // 2
    HID_KEYBOARD_DE_3, // 3
    HID_KEYBOARD_DE_4, // 4
    HID_KEYBOARD_DE_5, // 5
    HID_KEYBOARD_DE_6, // 6
    HID_KEYBOARD_DE_7, // 7
    HID_KEYBOARD_DE_8, // 8
    HID_KEYBOARD_DE_9, // 9
    HID_KEYBOARD_DE_DOUBLE_POINTS | KEY_MOD_LEFT_SHIFT, // :
    HID_KEYBOARD_DE_SEMI_COLON | KEY_MOD_LEFT_SHIFT, // ;
    HID_KEYBOARD_DE_SMALLER, // <
    HID_KEYBOARD_DE_EQUAL | KEY_MOD_LEFT_SHIFT, // =
    HID_KEYBOARD_DE_SMALLER | KEY_MOD_LEFT_SHIFT, // >
    HID_KEYBOARD_DE_SHARP_SS | KEY_MOD_LEFT_SHIFT, // ?
    HID_KEYBOARD_DE_Q | KEY_MOD_RIGHT_ALT, // @
    HID_KEYBOARD_DE_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_DE_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_DE_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_DE_D | KEY_MOD_LEFT_SHIFT, // D
    HID_KEYBOARD_DE_E | KEY_MOD_LEFT_SHIFT, // E
    HID_KEYBOARD_DE_F | KEY_MOD_LEFT_SHIFT, // F
    HID_KEYBOARD_DE_G | KEY_MOD_LEFT_SHIFT, // G
    HID_KEYBOARD_DE_H | KEY_MOD_LEFT_SHIFT, // H
    HID_KEYBOARD_DE_I | KEY_MOD_LEFT_SHIFT, // I
    HID_KEYBOARD_DE_J | KEY_MOD_LEFT_SHIFT, // J
    HID_KEYBOARD_DE_K | KEY_MOD_LEFT_SHIFT, // K
    HID_KEYBOARD_DE_L | KEY_MOD_LEFT_SHIFT, // L
    HID_KEYBOARD_DE_M | KEY_MOD_LEFT_SHIFT, // M
    HID_KEYBOARD_DE_N | KEY_MOD_LEFT_SHIFT, // N
    HID_KEYBOARD_DE_O | KEY_MOD_LEFT_SHIFT, // O
    HID_KEYBOARD_DE_P | KEY_MOD_LEFT_SHIFT, // P
    HID_KEYBOARD_DE_Q | KEY_MOD_LEFT_SHIFT, // Q
    HID_KEYBOARD_DE_R | KEY_MOD_LEFT_SHIFT, // R
    HID_KEYBOARD_DE_S | KEY_MOD_LEFT_SHIFT, // S
    HID_KEYBOARD_DE_T | KEY_MOD_LEFT_SHIFT, // T
    HID_KEYBOARD_DE_U | KEY_MOD_LEFT_SHIFT, // U
    HID_KEYBOARD_DE_V | KEY_MOD_LEFT_SHIFT, // V
    HID_KEYBOARD_DE_W | KEY_MOD_LEFT_SHIFT, // W
    HID_KEYBOARD_DE_X | KEY_MOD_LEFT_SHIFT, // X
    HID_KEYBOARD_DE_Y | KEY_MOD_LEFT_SHIFT, // Y
    HID_KEYBOARD_DE_Z | KEY_MOD_LEFT_SHIFT, // Z
    HID_KEYBOARD_DE_LEFT_PARENTHESIS | KEY_MOD_RIGHT_ALT, // [
    HID_KEYBOARD_DE_SHARP_SS | KEY_MOD_RIGHT_ALT, // bslash
    HID_KEYBOARD_DE_RIGHT_PARENTHESIS | KEY_MOD_RIGHT_ALT, // ]
    HID_KEYBOARD_DE_CIRCUMFLEX, // ^
    HID_KEYBOARD_DE_UNDERSCORE | KEY_MOD_LEFT_SHIFT, // _
    HID_KEYBOARD_DE_BACKTICK | KEY_MOD_LEFT_SHIFT, // `
    HID_KEYBOARD_DE_A, // a
    HID_KEYBOARD_DE_B, // b
    HID_KEYBOARD_DE_C, // c
    HID_KEYBOARD_DE_D, // d
    HID_KEYBOARD_DE_E, // e
    HID_KEYBOARD_DE_F, // f
    HID_KEYBOARD_DE_G, // g
    HID_KEYBOARD_DE_H, // h
    HID_KEYBOARD_DE_I, // i
    HID_KEYBOARD_DE_J, // j
    HID_KEYBOARD_DE_K, // k
    HID_KEYBOARD_DE_L, // l
    HID_KEYBOARD_DE_M, // m
    HID_KEYBOARD_DE_N, // n
    HID_KEYBOARD_DE_O, // o
    HID_KEYBOARD_DE_P, // p
    HID_KEYBOARD_DE_Q, // q
    HID_KEYBOARD_DE_R, // r
    HID_KEYBOARD_DE_S, // s
    HID_KEYBOARD_DE_T, // t
    HID_KEYBOARD_DE_U, // u
    HID_KEYBOARD_DE_V, // v
    HID_KEYBOARD_DE_W, // w
    HID_KEYBOARD_DE_X, // x
    HID_KEYBOARD_DE_Y, // y
    HID_KEYBOARD_DE_Z, // z
    HID_KEYBOARD_DE_7 | KEY_MOD_RIGHT_ALT, // {
    HID_KEYBOARD_DE_SMALLER | KEY_MOD_RIGHT_ALT, // |
    HID_KEYBOARD_DE_0 | KEY_MOD_RIGHT_ALT, // }
    HID_KEYBOARD_DE_STAR | KEY_MOD_RIGHT_ALT, // ~
    HID_KEYBOARD_NONE, // DEL
};

/** HID keyboard key codes FR */
enum HidKeyboardKeysFR {
    HID_KEYBOARD_FR_ERROR_ROLLOVER = 0x01,
    HID_KEYBOARD_FR_POST_FAIL = 0x02,
    HID_KEYBOARD_FR_ERROR_UNDEFINED = 0x03,

    HID_KEYBOARD_FR_ENTER = 0x28,
    HID_KEYBOARD_FR_ESC = 0x29,
    HID_KEYBOARD_FR_BACKSPACE = 0x2A,
    HID_KEYBOARD_FR_TAB = 0x2B,
    HID_KEYBOARD_FR_SPACE = 0x2C,

    HID_KEYBOARD_FR_A = 0x14,
    HID_KEYBOARD_FR_B = 0x05,
    HID_KEYBOARD_FR_C = 0x06,
    HID_KEYBOARD_FR_D = 0x07,
    HID_KEYBOARD_FR_E = 0x08,
    HID_KEYBOARD_FR_F = 0x09,
    HID_KEYBOARD_FR_G = 0x0A,
    HID_KEYBOARD_FR_H = 0x0B,
    HID_KEYBOARD_FR_I = 0x0C,
    HID_KEYBOARD_FR_J = 0x0D,
    HID_KEYBOARD_FR_K = 0x0E,
    HID_KEYBOARD_FR_L = 0x0F,
    HID_KEYBOARD_FR_M = 0x33,
    HID_KEYBOARD_FR_N = 0x11,
    HID_KEYBOARD_FR_O = 0x12,
    HID_KEYBOARD_FR_P = 0x13,
    HID_KEYBOARD_FR_Q = 0x04,
    HID_KEYBOARD_FR_R = 0x15,
    HID_KEYBOARD_FR_S = 0x16,
    HID_KEYBOARD_FR_T = 0x17,
    HID_KEYBOARD_FR_U = 0x18,
    HID_KEYBOARD_FR_V = 0x19,
    HID_KEYBOARD_FR_W = 0x1D,
    HID_KEYBOARD_FR_X = 0x1B,
    HID_KEYBOARD_FR_Y = 0x1C,
    HID_KEYBOARD_FR_Z = 0x1A,

    HID_KEYBOARD_FR_1 = 0x1E,
    HID_KEYBOARD_FR_2 = 0x1F,
    HID_KEYBOARD_FR_3 = 0x20,
    HID_KEYBOARD_FR_4 = 0x21,
    HID_KEYBOARD_FR_5 = 0x22,
    HID_KEYBOARD_FR_6 = 0x23,
    HID_KEYBOARD_FR_7 = 0x24,
    HID_KEYBOARD_FR_8 = 0x25,
    HID_KEYBOARD_FR_9 = 0x26,
    HID_KEYBOARD_FR_0 = 0x27,

    HID_KEYBOARD_FR_EXCLAMATION = 0x38,
    HID_KEYBOARD_FR_DOUBLE_QUOTE = 0x20,
    HID_KEYBOARD_FR_DOLLAR = 0x30,
    HID_KEYBOARD_FR_U_BACKTICK = 0x34,
    HID_KEYBOARD_FR_AND = 0x1E,
    HID_KEYBOARD_FR_SINGLE_QUOTE = 0x21,
    HID_KEYBOARD_FR_LEFT_PARENTHESIS = 0x22,
    HID_KEYBOARD_FR_RIGHT_PARENTHESIS = 0x2D,
    HID_KEYBOARD_FR_STAR = 0x31,
    HID_KEYBOARD_FR_EQUAL = 0x2E,
    HID_KEYBOARD_FR_COMMA = 0x10,
    HID_KEYBOARD_FR_DASH = 0x23,
    HID_KEYBOARD_FR_SEMI_COLON = 0x36,
    HID_KEYBOARD_FR_DOUBLE_POINTS = 0x37,
    HID_KEYBOARD_FR_SMALLER = 0x64,
    HID_KEYBOARD_FR_UNDERSCORE = 0x25,
    HID_KEYBOARD_FR_CIRCUMFLEX = 0x2F,
    HID_KEYBOARD_FR_A_BACKTICK = 0x27,
    HID_KEYBOARD_FR_E_ACCENT = 0x1F,
    HID_KEYBOARD_FR_E_BACKTICK = 0x24,
    HID_KEYBOARD_FR_C_CEDILLE = 0x26,

    HID_KEYBOARD_FR_CAPS_LOCK = 0xC1,
    HID_KEYBOARD_FR_F1 = 0xC2,
    HID_KEYBOARD_FR_F2 = 0xC3,
    HID_KEYBOARD_FR_F3 = 0xC4,
    HID_KEYBOARD_FR_F4 = 0xC5,
    HID_KEYBOARD_FR_F5 = 0xC6,
    HID_KEYBOARD_FR_F6 = 0xC7,
    HID_KEYBOARD_FR_F7 = 0xC8,
    HID_KEYBOARD_FR_F8 = 0xC9,
    HID_KEYBOARD_FR_F9 = 0xCA,
    HID_KEYBOARD_FR_F10 = 0xCB,
    HID_KEYBOARD_FR_F11 = 0xCC,
    HID_KEYBOARD_FR_F12 = 0xCD,
    HID_KEYBOARD_FR_PRINT = 0x63,
    HID_KEYBOARD_FR_SCROLL_LOCK = 0x47,
    HID_KEYBOARD_FR_PAUSE = 0x48,
    HID_KEYBOARD_FR_INSERT = 0xD1,
    HID_KEYBOARD_FR_HOME = 0xD2,
    HID_KEYBOARD_FR_PAGE_UP = 0xD3,
    HID_KEYBOARD_FR_DELETE = 0xD4,
    HID_KEYBOARD_FR_END = 0xD5,
    HID_KEYBOARD_FR_PAGE_DOWN = 0xD6,
    HID_KEYBOARD_FR_RIGHT_ARROW = 0xD7,
    HID_KEYBOARD_FR_LEFT_ARROW = 0xD8,
    HID_KEYBOARD_FR_DOWN_ARROW = 0xD9,
    HID_KEYBOARD_FR_UP_ARROW = 0xDA,
    HID_KEYBOARD_FR_NUM_LOCK = 0x53,
    HID_KEYBOARD_FR_NON_US = 0x64,
    HID_KEYBOARD_FR_APPLICATION = 0x65,
};

/** ASCII to keycode conversion table FR */
static const uint16_t hid_asciimap_fr[] = {
    HID_KEYBOARD_NONE, // NUL
    HID_KEYBOARD_NONE, // SOH
    HID_KEYBOARD_NONE, // STX
    HID_KEYBOARD_NONE, // ETX
    HID_KEYBOARD_NONE, // EOT
    HID_KEYBOARD_NONE, // ENQ
    HID_KEYBOARD_NONE, // ACK
    HID_KEYBOARD_NONE, // BEL
    HID_KEYBOARD_FR_BACKSPACE, // BS   Backspace
    HID_KEYBOARD_FR_TAB, // TAB  Tab
    HID_KEYBOARD_FR_ENTER, // LF   Enter
    HID_KEYBOARD_NONE, // VT
    HID_KEYBOARD_NONE, // FF
    HID_KEYBOARD_NONE, // CR
    HID_KEYBOARD_NONE, // SO
    HID_KEYBOARD_NONE, // SI
    HID_KEYBOARD_NONE, // DEL
    HID_KEYBOARD_NONE, // DC1
    HID_KEYBOARD_NONE, // DC2
    HID_KEYBOARD_NONE, // DC3
    HID_KEYBOARD_NONE, // DC4
    HID_KEYBOARD_NONE, // NAK
    HID_KEYBOARD_NONE, // SYN
    HID_KEYBOARD_NONE, // ETB
    HID_KEYBOARD_NONE, // CAN
    HID_KEYBOARD_NONE, // EM
    HID_KEYBOARD_NONE, // SUB
    HID_KEYBOARD_NONE, // ESC
    HID_KEYBOARD_NONE, // FS
    HID_KEYBOARD_NONE, // GS
    HID_KEYBOARD_NONE, // RS
    HID_KEYBOARD_NONE, // US
    HID_KEYBOARD_FR_SPACE, // ' ' Space
    HID_KEYBOARD_FR_EXCLAMATION, // !
    HID_KEYBOARD_FR_DOUBLE_QUOTE, // "
    HID_KEYBOARD_FR_DOUBLE_QUOTE | KEY_MOD_RIGHT_ALT, // #
    HID_KEYBOARD_FR_DOLLAR, // $
    HID_KEYBOARD_FR_U_BACKTICK | KEY_MOD_LEFT_SHIFT, // %
    HID_KEYBOARD_FR_AND, // &
    HID_KEYBOARD_FR_SINGLE_QUOTE, // '
    HID_KEYBOARD_FR_LEFT_PARENTHESIS, // (
    HID_KEYBOARD_FR_RIGHT_PARENTHESIS, // )
    HID_KEYBOARD_FR_STAR, // *
    HID_KEYBOARD_FR_EQUAL | KEY_MOD_LEFT_SHIFT, // +
    HID_KEYBOARD_FR_COMMA, // ,
    HID_KEYBOARD_FR_DASH, // -
    HID_KEYBOARD_FR_SEMI_COLON | KEY_MOD_LEFT_SHIFT, // .
    HID_KEYBOARD_FR_DOUBLE_POINTS | KEY_MOD_LEFT_SHIFT, // /
    HID_KEYBOARD_FR_A_BACKTICK | KEY_MOD_LEFT_SHIFT, // 0
    HID_KEYBOARD_FR_AND | KEY_MOD_LEFT_SHIFT, // 1
    HID_KEYBOARD_FR_E_ACCENT | KEY_MOD_LEFT_SHIFT, // 2
    HID_KEYBOARD_FR_DOUBLE_QUOTE | KEY_MOD_LEFT_SHIFT, // 3
    HID_KEYBOARD_FR_SINGLE_QUOTE | KEY_MOD_LEFT_SHIFT, // 4
    HID_KEYBOARD_FR_LEFT_PARENTHESIS | KEY_MOD_LEFT_SHIFT, // 5
    HID_KEYBOARD_FR_DASH | KEY_MOD_LEFT_SHIFT, // 6
    HID_KEYBOARD_FR_E_BACKTICK | KEY_MOD_LEFT_SHIFT, // 7
    HID_KEYBOARD_FR_UNDERSCORE | KEY_MOD_LEFT_SHIFT, // 8
    HID_KEYBOARD_FR_C_CEDILLE | KEY_MOD_LEFT_SHIFT, // 9
    HID_KEYBOARD_FR_DOUBLE_POINTS, // :
    HID_KEYBOARD_FR_SEMI_COLON, // ;
    HID_KEYBOARD_FR_SMALLER, // <
    HID_KEYBOARD_FR_EQUAL, // =
    HID_KEYBOARD_FR_SMALLER | KEY_MOD_LEFT_SHIFT, // >
    HID_KEYBOARD_FR_COMMA | KEY_MOD_LEFT_SHIFT, // ?
    HID_KEYBOARD_FR_A_BACKTICK | KEY_MOD_RIGHT_ALT, // @
    HID_KEYBOARD_FR_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_FR_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_FR_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_FR_D | KEY_MOD_LEFT_SHIFT, // D
    HID_KEYBOARD_FR_E | KEY_MOD_LEFT_SHIFT, // E
    HID_KEYBOARD_FR_F | KEY_MOD_LEFT_SHIFT, // F
    HID_KEYBOARD_FR_G | KEY_MOD_LEFT_SHIFT, // G
    HID_KEYBOARD_FR_H | KEY_MOD_LEFT_SHIFT, // H
    HID_KEYBOARD_FR_I | KEY_MOD_LEFT_SHIFT, // I
    HID_KEYBOARD_FR_J | KEY_MOD_LEFT_SHIFT, // J
    HID_KEYBOARD_FR_K | KEY_MOD_LEFT_SHIFT, // K
    HID_KEYBOARD_FR_L | KEY_MOD_LEFT_SHIFT, // L
    HID_KEYBOARD_FR_M | KEY_MOD_LEFT_SHIFT, // M
    HID_KEYBOARD_FR_N | KEY_MOD_LEFT_SHIFT, // N
    HID_KEYBOARD_FR_O | KEY_MOD_LEFT_SHIFT, // O
    HID_KEYBOARD_FR_P | KEY_MOD_LEFT_SHIFT, // P
    HID_KEYBOARD_FR_Q | KEY_MOD_LEFT_SHIFT, // Q
    HID_KEYBOARD_FR_R | KEY_MOD_LEFT_SHIFT, // R
    HID_KEYBOARD_FR_S | KEY_MOD_LEFT_SHIFT, // S
    HID_KEYBOARD_FR_T | KEY_MOD_LEFT_SHIFT, // T
    HID_KEYBOARD_FR_U | KEY_MOD_LEFT_SHIFT, // U
    HID_KEYBOARD_FR_V | KEY_MOD_LEFT_SHIFT, // V
    HID_KEYBOARD_FR_W | KEY_MOD_LEFT_SHIFT, // W
    HID_KEYBOARD_FR_X | KEY_MOD_LEFT_SHIFT, // X
    HID_KEYBOARD_FR_Y | KEY_MOD_LEFT_SHIFT, // Y
    HID_KEYBOARD_FR_Z | KEY_MOD_LEFT_SHIFT, // Z
    HID_KEYBOARD_FR_LEFT_PARENTHESIS | KEY_MOD_RIGHT_ALT, // [
    HID_KEYBOARD_FR_UNDERSCORE | KEY_MOD_RIGHT_ALT, // bslash
    HID_KEYBOARD_FR_RIGHT_PARENTHESIS | KEY_MOD_RIGHT_ALT, // ]
    HID_KEYBOARD_FR_CIRCUMFLEX, // ^
    HID_KEYBOARD_FR_UNDERSCORE, // _
    HID_KEYBOARD_FR_E_BACKTICK | KEY_MOD_RIGHT_ALT, // `
    HID_KEYBOARD_FR_A, // a
    HID_KEYBOARD_FR_B, // b
    HID_KEYBOARD_FR_C, // c
    HID_KEYBOARD_FR_D, // d
    HID_KEYBOARD_FR_E, // e
    HID_KEYBOARD_FR_F, // f
    HID_KEYBOARD_FR_G, // g
    HID_KEYBOARD_FR_H, // h
    HID_KEYBOARD_FR_I, // i
    HID_KEYBOARD_FR_J, // j
    HID_KEYBOARD_FR_K, // k
    HID_KEYBOARD_FR_L, // l
    HID_KEYBOARD_FR_M, // m
    HID_KEYBOARD_FR_N, // n
    HID_KEYBOARD_FR_O, // o
    HID_KEYBOARD_FR_P, // p
    HID_KEYBOARD_FR_Q, // q
    HID_KEYBOARD_FR_R, // r
    HID_KEYBOARD_FR_S, // s
    HID_KEYBOARD_FR_T, // t
    HID_KEYBOARD_FR_U, // u
    HID_KEYBOARD_FR_V, // v
    HID_KEYBOARD_FR_W, // w
    HID_KEYBOARD_FR_X, // x
    HID_KEYBOARD_FR_Y, // y
    HID_KEYBOARD_FR_Z, // z
    HID_KEYBOARD_FR_SINGLE_QUOTE | KEY_MOD_RIGHT_ALT, // {
    HID_KEYBOARD_FR_DASH | KEY_MOD_RIGHT_ALT, // |
    HID_KEYBOARD_FR_EQUAL | KEY_MOD_RIGHT_ALT, // }
    HID_KEYBOARD_FR_E_ACCENT | KEY_MOD_RIGHT_ALT, // ~
    HID_KEYBOARD_NONE, // DEL
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static const uint16_t* hid_asciimaps[] = {hid_asciimap, hid_asciimap_de, hid_asciimap_fr};
#pragma GCC diagnostic pop

typedef struct {
    uint32_t vid;
    uint32_t pid;
    char manuf[32];
    char product[32];
} FuriHalUsbHidConfig;

typedef void (*HidStateCallback)(bool state, void* context);

/** ASCII to keycode conversion macro */
#define HID_ASCII_TO_KEY(x, y) \
    (((uint8_t)y < 128) ? (hid_asciimaps[(uint8_t)x][(uint8_t)y]) : HID_KEYBOARD_NONE)

/** HID keyboard leds */
enum HidKeyboardLeds {
    HID_KB_LED_NUM = (1 << 0),
    HID_KB_LED_CAPS = (1 << 1),
    HID_KB_LED_SCROLL = (1 << 2),
};

/** HID mouse buttons */
enum HidMouseButtons {
    HID_MOUSE_BTN_LEFT = (1 << 0),
    HID_MOUSE_BTN_RIGHT = (1 << 1),
    HID_MOUSE_BTN_WHEEL = (1 << 2),
};

/** Get USB HID connection state
 *
 * @return      true / false
 */
bool furi_hal_hid_is_connected();

/** Get USB HID keyboard leds state
 *
 * @return      leds state
 */
uint8_t furi_hal_hid_get_led_state();

/** Set USB HID connect/disconnect callback
 *
 * @param      cb  callback
 * @param      ctx  callback context
 */
void furi_hal_hid_set_state_callback(HidStateCallback cb, void* ctx);

/** Set the following key to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_kb_press(uint16_t button);

/** Set the following key to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_kb_release(uint16_t button);

/** Clear all pressed keys and send HID report
 *
 */
bool furi_hal_hid_kb_release_all();

/** Set mouse movement and send HID report
 *
 * @param      dx  x coordinate delta
 * @param      dy  y coordinate delta
 */
bool furi_hal_hid_mouse_move(int8_t dx, int8_t dy);

/** Set mouse button to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_mouse_press(uint8_t button);

/** Set mouse button to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_mouse_release(uint8_t button);

/** Set mouse wheel position and send HID report
 *
 * @param      delta  number of scroll steps
 */
bool furi_hal_hid_mouse_scroll(int8_t delta);

/** Set the following consumer key to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_consumer_key_press(uint16_t button);

/** Set the following consumer key to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_consumer_key_release(uint16_t button);