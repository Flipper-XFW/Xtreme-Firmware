#include "settings.h"

XtremeSettings* xtreme_settings = NULL;

XtremeSettings* XTREME_SETTINGS() {
    if(xtreme_settings == NULL) {
        XTREME_SETTINGS_LOAD();
    }
    return xtreme_settings;
}

bool XTREME_SETTINGS_LOAD() {
    if(xtreme_settings == NULL) {
        xtreme_settings = malloc(sizeof(XtremeSettings));
        bool loaded = saved_struct_load(
            XTREME_SETTINGS_PATH,
            xtreme_settings,
            sizeof(XtremeSettings),
            XTREME_SETTINGS_MAGIC,
            XTREME_SETTINGS_VERSION);
        if(!loaded) {
            memset(xtreme_settings, 0, sizeof(XtremeSettings));
            loaded = XTREME_SETTINGS_SAVE();
        }
        return loaded;
    }
    return true;
}

bool XTREME_SETTINGS_SAVE() {
    if(xtreme_settings == NULL) {
        XTREME_SETTINGS_LOAD();
    }
    return saved_struct_save(
        XTREME_SETTINGS_PATH,
        xtreme_settings,
        sizeof(XtremeSettings),
        XTREME_SETTINGS_MAGIC,
        XTREME_SETTINGS_VERSION);
}
