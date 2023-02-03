#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BadKbApp BadKbApp;

void bad_kb_set_name(BadKbApp* app, const char* fmt, ...);

#ifdef __cplusplus
}
#endif
