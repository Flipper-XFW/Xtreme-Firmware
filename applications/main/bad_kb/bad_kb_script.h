#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <bt/bt_service/bt_i.h>

typedef struct BadKbScript BadKbScript;

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
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
} BadKbState;

void bad_kb_connection_init(Bt* bt);

void bad_kb_connection_deinit(Bt* bt);

BadKbScript* bad_kb_script_open(FuriString* file_path, Bt* bt);

void bad_kb_script_close(BadKbScript* bad_kb);

void bad_kb_script_set_keyboard_layout(BadKbScript* bad_kb, FuriString* layout_path);

void bad_kb_script_start(BadKbScript* bad_kb);

void bad_kb_script_stop(BadKbScript* bad_kb);

void bad_kb_script_toggle(BadKbScript* bad_kb);

BadKbState* bad_kb_script_get_state(BadKbScript* bad_kb);

#ifdef __cplusplus
}
#endif
