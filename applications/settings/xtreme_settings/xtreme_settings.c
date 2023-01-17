#include "xtreme_settings.h"

bool XTREME_SETTINGS_LOAD(XtremeSettings* xtreme_settings) {
    furi_assert(xtreme_settings);

    bool loaded = saved_struct_load(
        XTREME_SETTINGS_PATH, xtreme_settings, sizeof(XtremeSettings), XTREME_SETTINGS_MAGIC, XTREME_SETTINGS_VERSION);
    if(!loaded) {
        memset(xtreme_settings, 0, sizeof(XtremeSettings));
        loaded = XTREME_SETTINGS_SAVE(xtreme_settings);
    }
    return loaded;
}

bool XTREME_SETTINGS_SAVE(XtremeSettings* xtreme_settings) {
    furi_assert(xtreme_settings);

    return saved_struct_save(
        XTREME_SETTINGS_PATH, xtreme_settings, sizeof(XtremeSettings), XTREME_SETTINGS_MAGIC, XTREME_SETTINGS_VERSION);
}
