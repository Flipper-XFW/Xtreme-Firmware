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

bool DESKTOP_KEYBINDS_SAVE(Keybinds* x, size_t size) {
    return saved_struct_save(
        DESKTOP_KEYBINDS_PATH, x, size, DESKTOP_KEYBINDS_MAGIC, DESKTOP_KEYBINDS_VER);
}

bool DESKTOP_KEYBINDS_LOAD(Keybinds* x, size_t size) {
    bool ok = saved_struct_load(
        DESKTOP_KEYBINDS_PATH, x, size, DESKTOP_KEYBINDS_MAGIC, DESKTOP_KEYBINDS_VER);
    if(ok) {
        for(KeybindType i = 0; i < KeybindTypeCount; i++) {
            for(KeybindKey j = 0; j < KeybindKeyCount; j++) {
                char* keybind = (*x)[i][j].data;
                if(!strncmp(keybind, EXT_PATH("apps/Misc/nightstand.fap"), MAX_KEYBIND_LENGTH)) {
                    strcpy(keybind, "Clock");
                }
            }
        }
    } else {
        memset(x, 0, size);
        strcpy((*x)[KeybindTypePress][KeybindKeyUp].data, "Lock Menu");
        strcpy((*x)[KeybindTypePress][KeybindKeyDown].data, "Archive");
        strcpy((*x)[KeybindTypePress][KeybindKeyRight].data, "Passport");
        strcpy((*x)[KeybindTypePress][KeybindKeyLeft].data, "Clock");
        strcpy((*x)[KeybindTypeHold][KeybindKeyUp].data, "");
        strcpy((*x)[KeybindTypeHold][KeybindKeyDown].data, "");
        strcpy((*x)[KeybindTypeHold][KeybindKeyRight].data, "Device Info");
        strcpy((*x)[KeybindTypeHold][KeybindKeyLeft].data, "Lock with PIN");
    }
    return ok;
}
