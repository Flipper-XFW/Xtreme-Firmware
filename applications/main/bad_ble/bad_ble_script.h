#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <bt/bt_service/bt_i.h>

typedef struct BadBleScript BadBleScript;

typedef enum {
    BadBleStateInit,
    BadBleStateNotConnected,
    BadBleStateIdle,
    BadBleStateWillRun,
    BadBleStateRunning,
    BadBleStateDelay,
    BadBleStateDone,
    BadBleStateScriptError,
    BadBleStateFileError,
} BadBleWorkerState;

typedef struct {
    BadBleWorkerState state;
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
} BadBleState;

BadBleScript* bad_ble_script_open(FuriString* file_path, Bt *bt);

void bad_ble_script_close(BadBleScript* bad_ble);

void bad_ble_script_set_keyboard_layout(BadBleScript* bad_ble, FuriString* layout_path);

void bad_ble_script_start(BadBleScript* bad_ble);

void bad_ble_script_stop(BadBleScript* bad_ble);

void bad_ble_script_toggle(BadBleScript* bad_ble);

BadBleState* bad_ble_script_get_state(BadBleScript* bad_ble);

#ifdef __cplusplus
}
#endif
