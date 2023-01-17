#include "xtreme_settings.h"

XtremeSettings* xtreme_settings = NULL;

XtremeSettings* XTREME_SETTINGS() {
    if (xtreme_settings == NULL) {
        xtreme_settings = malloc(sizeof(XtremeSettings));
        _XTREME_SETTINGS_LOAD(xtreme_settings);
    }
    return xtreme_settings;
}

bool XTREME_SETTINGS_SAVE() {
    if (xtreme_settings == NULL) {
        XTREME_SETTINGS();
    }
    return _XTREME_SETTINGS_SAVE(xtreme_settings);
}

bool _XTREME_SETTINGS_LOAD(XtremeSettings* xtreme_settings) {
    furi_assert(xtreme_settings);

    bool loaded = saved_struct_load(
        XTREME_SETTINGS_PATH, xtreme_settings, sizeof(XtremeSettings), XTREME_SETTINGS_MAGIC, XTREME_SETTINGS_VERSION);
    if(!loaded) {
        memset(xtreme_settings, 0, sizeof(XtremeSettings));
        loaded = _XTREME_SETTINGS_SAVE(xtreme_settings);
    }
    return loaded;
}

bool _XTREME_SETTINGS_SAVE(XtremeSettings* xtreme_settings) {
    furi_assert(xtreme_settings);

    return saved_struct_save(
        XTREME_SETTINGS_PATH, xtreme_settings, sizeof(XtremeSettings), XTREME_SETTINGS_MAGIC, XTREME_SETTINGS_VERSION);
}
