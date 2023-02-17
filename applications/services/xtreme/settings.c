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
            xtreme_settings->asset_pack = ""; // SFW
            xtreme_settings->anim_speed = 100; // 100%
            xtreme_settings->cycle_anims = 0; // Meta.txt
            xtreme_settings->unlock_anims = false; // OFF
            xtreme_settings->battery_icon = BatteryIconBarPercent; // Bar %
            xtreme_settings->status_icons = true; // ON
            xtreme_settings->bar_borders = true; // ON
            xtreme_settings->bar_background = false; // OFF
            xtreme_settings->bad_bt = false; // USB
            xtreme_settings->sort_dirs_first = true; // ON
            xtreme_settings->butthurt_timer = 43200; // 12 H
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
