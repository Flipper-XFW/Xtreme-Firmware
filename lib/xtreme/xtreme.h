#pragma once

#include <gui/icon_i.h>
#include <power/power_service/power.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XTREME_SETTINGS_PATH CFG_PATH("xtreme_settings.txt")
#define XTREME_ASSETS_PATH EXT_PATH("dolphin_custom")
#define XTREME_APPS_PATH CFG_PATH("xtreme_apps.txt")
#define XTREME_ASSETS_PACK_NAME_LEN 32

typedef struct {
    bool is_nsfw; // TODO: replace with packs text support

    char asset_pack[XTREME_ASSETS_PACK_NAME_LEN];
    uint32_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    bool fallback_anim;
    bool wii_menu;
    bool lock_on_boot;
    bool bad_pins_format;
    bool lockscreen_time;
    bool lockscreen_seconds;
    bool lockscreen_date;
    bool lockscreen_statusbar;
    bool lockscreen_prompt;
    uint32_t battery_icon;
    bool status_icons;
    bool bar_borders;
    bool bar_background;
    bool sort_dirs_first;
    bool show_hidden_files;
    bool show_internal_tab;
    uint32_t favorite_timeout;
    bool bad_bt;
    bool bad_bt_remember;
    bool dark_mode;
    bool rgb_backlight;
    uint32_t butthurt_timer;
    uint32_t charge_cap;
} XtremeSettings;

void XTREME_SETTINGS_SAVE();
XtremeSettings* XTREME_SETTINGS();

#ifdef __cplusplus
}
#endif
