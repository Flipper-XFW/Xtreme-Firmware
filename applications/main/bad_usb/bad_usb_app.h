#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BadUsbApp BadUsbApp;

void bad_usb_set_name(BadUsbApp* app, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
