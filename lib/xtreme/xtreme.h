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
    char asset_pack[XTREME_ASSETS_PACK_NAME_LEN];
    uint32_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    bool fallback_anim;
    bool wii_menu;
    bool bad_pins_format;
    bool lockscreen_time;
    bool lockscreen_seconds;
    bool lockscreen_date;
    bool lockscreen_statusbar;
    bool lockscreen_prompt;
    BatteryIcon battery_icon;
    bool status_icons;
    bool bar_borders;
    bool bar_background;
    bool sort_dirs_first;
    bool dark_mode;
    uint32_t favorite_timeout;
    bool bad_bt;
    bool bad_bt_remember;
    int32_t butthurt_timer;
    bool rgb_backlight;
} XtremeSettings;

void XTREME_SETTINGS_SAVE();
XtremeSettings* XTREME_SETTINGS();

typedef struct {
    bool is_nsfw;
    const Icon* A_Levelup_128x64;
    const Icon* I_BLE_Pairing_128x64;
    const Icon* I_DolphinCommon_56x48;
    const Icon* I_DolphinMafia_115x62;
    const Icon* I_DolphinNice_96x59;
    const Icon* I_DolphinWait_61x59;
    const Icon* I_iButtonDolphinVerySuccess_108x52;
    const Icon* I_DolphinReadingSuccess_59x63;
    const Icon* I_Lockscreen;
    const Icon* I_WarningDolphin_45x42;
    const Icon* I_NFC_dolphin_emulation_47x61;
    const Icon* I_passport_bad_46x49;
    const Icon* I_passport_DB;
    const Icon* I_passport_happy_46x49;
    const Icon* I_passport_okay_46x49;
    const Icon* I_RFIDDolphinReceive_97x61;
    const Icon* I_RFIDDolphinSend_97x61;
    const Icon* I_RFIDDolphinSuccess_108x57;
    const Icon* I_Cry_dolph_55x52;
    const Icon* I_Background_128x11;
    const Icon* I_Fishing_123x52;
    const Icon* I_Scanning_123x52;
    const Icon* I_Auth_62x31;
    const Icon* I_Connect_me_62x31;
    const Icon* I_Connected_62x31;
    const Icon* I_Error_62x31;
} XtremeAssets;

XtremeAssets* XTREME_ASSETS();

#ifdef __cplusplus
}
#endif
