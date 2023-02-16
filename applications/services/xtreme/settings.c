#include "settings.h"

#define TAG "XtremeSettings"

XtremeSettings* xtreme_settings = NULL;

XtremeSettings* XTREME_SETTINGS() {
    if(xtreme_settings == NULL) {
        XTREME_SETTINGS_LOAD();
    }
    return xtreme_settings;
}

void XTREME_SETTINGS_LOAD() {
    if(xtreme_settings == NULL) {

        xtreme_settings = malloc(sizeof(XtremeSettings));
        bool loaded;

        if(furi_hal_rtc_get_boot_mode() != FuriHalRtcBootModeNormal) {
            FURI_LOG_W(TAG, "Load skipped. Device is in special startup mode.");
            loaded = false;
        } else {
            loaded = saved_struct_load(
                XTREME_SETTINGS_PATH,
                xtreme_settings,
                sizeof(XtremeSettings),
                XTREME_SETTINGS_MAGIC,
                XTREME_SETTINGS_VERSION);
        }

        if(!loaded) {
            memset(xtreme_settings, 0, sizeof(XtremeSettings));
        }
    }
}

bool XTREME_SETTINGS_SAVE() {
    if(xtreme_settings == NULL) {
        XTREME_SETTINGS_LOAD();
    }

    if(furi_hal_rtc_get_boot_mode() != FuriHalRtcBootModeNormal) {
        return true;
    }

    return saved_struct_save(
        XTREME_SETTINGS_PATH,
        xtreme_settings,
        sizeof(XtremeSettings),
        XTREME_SETTINGS_MAGIC,
        XTREME_SETTINGS_VERSION);
}
