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
        bool loaded = false;
        bool skip = furi_hal_rtc_get_boot_mode() != FuriHalRtcBootModeNormal;

        if(skip) {
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
            if(!skip) {
                storage_simply_remove(furi_record_open(RECORD_STORAGE), XTREME_SETTINGS_PATH_OLD);
                furi_record_close(RECORD_STORAGE);
            }
            memset(xtreme_settings, 0, sizeof(XtremeSettings));
            strlcpy(xtreme_settings->asset_pack, "", MAX_PACK_NAME_LEN); // SFW
            xtreme_settings->anim_speed = 100; // 100%
            xtreme_settings->cycle_anims = 0; // Meta.txt
            xtreme_settings->unlock_anims = false; // OFF
            xtreme_settings->battery_icon = BatteryIconBarPercent; // Bar %
            xtreme_settings->status_icons = true; // ON
            xtreme_settings->bar_borders = true; // ON
            xtreme_settings->bar_background = false; // OFF
            xtreme_settings->bad_bt = false; // USB
            xtreme_settings->bad_bt_remember = false; // OFF
            xtreme_settings->butthurt_timer = 43200; // 12 H
            xtreme_settings->sort_dirs_first = true; // ON
            xtreme_settings->dark_mode = false; // OFF
            xtreme_settings->left_handed = false; // OFF
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
