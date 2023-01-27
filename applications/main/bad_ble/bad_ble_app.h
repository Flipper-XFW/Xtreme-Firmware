#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BadBleApp BadBleApp;

void bad_ble_set_name(BadBleApp* app, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
