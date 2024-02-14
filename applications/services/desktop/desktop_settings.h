#pragma once

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>

#define DESKTOP_SETTINGS_OLD_PATH CFG_PATH("desktop.settings")
#define DESKTOP_SETTINGS_PATH INT_PATH(".desktop.settings")
#define DESKTOP_SETTINGS_MAGIC (0x17)
#define DESKTOP_SETTINGS_VER (11)

#define DESKTOP_KEYBINDS_OLD_PATH CFG_PATH(".desktop.keybinds")
#define DESKTOP_KEYBINDS_PATH CFG_PATH("desktop.keybinds")
#define DESKTOP_KEYBINDS_MAGIC (0x14)
#define DESKTOP_KEYBINDS_VER (1)

#define DESKTOP_SETTINGS_RUN_PIN_SETUP_ARG "run_pin_setup"

#define MAX_PIN_SIZE 10
#define MIN_PIN_SIZE 4
#define MAX_KEYBIND_LENGTH 64

typedef struct {
    InputKey data[MAX_PIN_SIZE];
    uint8_t length;
} PinCode;

typedef struct {
    char data[MAX_KEYBIND_LENGTH];
} Keybind;

typedef enum {
    KeybindTypePress,
    KeybindTypeHold,
    KeybindTypeCount,
} KeybindType;

typedef enum {
    KeybindKeyUp,
    KeybindKeyDown,
    KeybindKeyRight,
    KeybindKeyLeft,
    KeybindKeyCount,
} KeybindKey;

typedef struct {
    PinCode pin_code;
    uint32_t auto_lock_delay_ms;
    bool auto_lock_with_pin;
} DesktopSettings;

typedef Keybind Keybinds[KeybindTypeCount][KeybindKeyCount];

#ifdef __cplusplus
extern "C" {
#endif

bool DESKTOP_SETTINGS_SAVE(DesktopSettings* x);

bool DESKTOP_SETTINGS_LOAD(DesktopSettings* x);

bool DESKTOP_KEYBINDS_SAVE(Keybinds* x, size_t size);

bool DESKTOP_KEYBINDS_LOAD(Keybinds* x, size_t size);

#ifdef __cplusplus
}
#endif
