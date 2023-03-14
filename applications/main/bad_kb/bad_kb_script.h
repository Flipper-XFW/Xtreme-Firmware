#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <furi_hal.h>
#include <bt/bt_service/bt_i.h>

typedef struct BadKbApp BadKbApp;

#define FILE_BUFFER_LEN 16

extern uint8_t bt_timeout;

typedef enum {
    BadKbStateInit,
    BadKbStateNotConnected,
    BadKbStateIdle,
    BadKbStateWillRun,
    BadKbStateRunning,
    BadKbStateDelay,
    BadKbStateDone,
    BadKbStateScriptError,
    BadKbStateFileError,
} BadKbWorkerState;

typedef struct {
    BadKbWorkerState state;
    bool is_bt;
    uint32_t pin;
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
} BadKbState;

typedef struct BadKbScript {
    FuriHalUsbHidConfig hid_cfg;
    BadKbState st;
    FuriString* file_path;
    FuriString* keyboard_layout;
    uint32_t defdelay;
    uint16_t layout[128];
    uint32_t stringdelay;
    FuriThread* thread;
    uint8_t file_buf[FILE_BUFFER_LEN + 1];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;
    FuriString* line;

    FuriString* line_prev;
    uint32_t repeat_cnt;

    Bt* bt;
} BadKbScript;

void bad_kb_config_switch_mode(BadKbApp* app);

void bad_kb_config_switch_remember_mode(BadKbApp* app);

int32_t bad_kb_connection_init(BadKbApp* app);

void bad_kb_connection_deinit(BadKbApp* app);

BadKbScript* bad_kb_script_open(FuriString* file_path, Bt* bt);

void bad_kb_script_close(BadKbScript* bad_kb);

void bad_kb_script_set_keyboard_layout(BadKbScript* bad_kb, FuriString* layout_path);

void bad_kb_script_start(BadKbScript* bad_kb);

void bad_kb_script_stop(BadKbScript* bad_kb);

void bad_kb_script_toggle(BadKbScript* bad_kb);

BadKbState* bad_kb_script_get_state(BadKbScript* bad_kb);

uint16_t ducky_get_keycode(BadKbScript* bad_kb, const char* param, bool accept_chars);

uint32_t ducky_get_command_len(const char* line);

bool ducky_is_line_end(const char chr);

#ifdef __cplusplus
}
#endif
