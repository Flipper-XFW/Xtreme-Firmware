#include "desktop_settings.h"

bool DESKTOP_SETTINGS_SAVE(DesktopSettings* x) {
    return saved_struct_save(
        DESKTOP_SETTINGS_PATH,
        x,
        sizeof(DesktopSettings),
        DESKTOP_SETTINGS_MAGIC,
        DESKTOP_SETTINGS_VER);
}

bool DESKTOP_SETTINGS_LOAD(DesktopSettings* x) {
    return saved_struct_load(
        DESKTOP_SETTINGS_PATH,
        x,
        sizeof(DesktopSettings),
        DESKTOP_SETTINGS_MAGIC,
        DESKTOP_SETTINGS_VER);
}

bool DESKTOP_KEYBINDS_SAVE(Keybind (*x)[KeybindTypeCount][KeybindKeyCount], size_t size) {
    return saved_struct_save(
        DESKTOP_KEYBINDS_PATH, x, size, DESKTOP_KEYBINDS_MAGIC, DESKTOP_KEYBINDS_VER);
}

bool DESKTOP_KEYBINDS_LOAD(Keybind (*x)[KeybindTypeCount][KeybindKeyCount], size_t size) {
    return saved_struct_load(
        DESKTOP_KEYBINDS_PATH, x, size, DESKTOP_KEYBINDS_MAGIC, DESKTOP_KEYBINDS_VER);
}
