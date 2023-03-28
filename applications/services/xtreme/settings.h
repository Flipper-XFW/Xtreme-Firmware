#pragma once

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>
#include <power/power_service/power.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PACK_NAME_LEN 32

#define XTREME_SETTINGS_VERSION (10)
#define XTREME_SETTINGS_MAGIC (0x69)
#define XTREME_SETTINGS_OLD_INT_PATH INT_PATH(".xtreme.settings")
#define XTREME_SETTINGS_OLD_PATH EXT_PATH(".xtreme.settings")
#define XTREME_SETTINGS_PATH CFG_PATH("xtreme.settings")

#define XTREME_APPS_PATH CFG_PATH("xtreme_apps.txt")

typedef struct {
    char asset_pack[MAX_PACK_NAME_LEN];
    uint16_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    bool fallback_anim;
    bool wii_menu;
    bool lockscreen_time;
    bool lockscreen_date;
    bool lockscreen_statusbar;
    bool lockscreen_prompt;
    BatteryIcon battery_icon;
    bool status_icons;
    bool bar_borders;
    bool bar_background;
    bool sort_dirs_first;
    bool dark_mode;
    bool bad_bt;
    bool bad_bt_remember;
    int32_t butthurt_timer;
    bool rgb_backlight;
} XtremeSettings;

XtremeSettings* XTREME_SETTINGS();

void XTREME_SETTINGS_LOAD();

bool XTREME_SETTINGS_SAVE();

#ifdef __cplusplus
}
#endif
