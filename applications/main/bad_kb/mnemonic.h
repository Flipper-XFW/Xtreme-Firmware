#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "bad_kb_script.h"

// A no opperation function
int32_t ducky_fnc_noop(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// DELAY
int32_t ducky_fnc_delay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// DEFAULTDELAY
int32_t ducky_fnc_defdelay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// STRINGDELAY
int32_t ducky_fnc_strdelay(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// STRING
int32_t ducky_fnc_string(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// STRINGLN
int32_t ducky_fnc_stringln(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// REPEAT
int32_t ducky_fnc_repeat(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// SYSRQ
int32_t ducky_fnc_sysrq(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// ALTCHAR
int32_t ducky_fnc_altchar(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// ALTSTRING
int32_t ducky_fnc_altstring(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// HOLD
int32_t ducky_fnc_hold(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);
// RELEASE
int32_t ducky_fnc_release(
    BadKbScript* bad_kb,
    FuriString* line,
    const char* line_tmp,
    char* error,
    size_t error_len);

#ifdef __cplusplus
}
#endif
