#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XTREME_SETTINGS_PATH CFG_PATH("xtreme_settings.txt")
#define XTREME_ASSETS_OLD_PATH EXT_PATH("dolphin_custom")
#define XTREME_ASSETS_PATH EXT_PATH("asset_packs")
#define XTREME_MENU_OLD_PATH CFG_PATH("xtreme_apps.txt")
#define XTREME_MENU_PATH CFG_PATH("xtreme_menu.txt")
#define XTREME_ASSETS_PACK_NAME_LEN 32

typedef enum {
    BatteryIconOff,
    BatteryIconBar,
    BatteryIconPercent,
    BatteryIconInvertedPercent,
    BatteryIconRetro3,
    BatteryIconRetro5,
    BatteryIconBarPercent,
    BatteryIconCount,
} BatteryIcon;

typedef enum {
    MenuStyleList,
    MenuStyleWii,
    MenuStyleDsi,
    MenuStylePs4,
    MenuStyleVertical,
    MenuStyleC64,
    MenuStyleEurocorp,
    MenuStyleCompact,
    MenuStyleTerminal,
    MenuStyleCount,
} MenuStyle;

typedef enum {
    SpiDefault, // cs on pa4
    SpiExtra, // cs on pc3
    SpiCount,
} SpiHandle;

typedef enum {
    UARTDefault, // pin 13,14
    UARTExtra, // pin 15,16
    UARTCount,
} UARTChannel;

typedef struct {
    bool is_nsfw; // TODO: replace with packs text support

    char asset_pack[XTREME_ASSETS_PACK_NAME_LEN];
    uint32_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    bool fallback_anim;
    MenuStyle menu_style;
    bool lock_on_boot;
    bool bad_pins_format;
    bool allow_locked_rpc_commands;
    bool lockscreen_time;
    bool lockscreen_seconds;
    bool lockscreen_date;
    bool lockscreen_statusbar;
    bool lockscreen_prompt;
    bool lockscreen_transparent;
    BatteryIcon battery_icon;
    bool statusbar_clock;
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
    SpiHandle spi_cc1101_handle;
    SpiHandle spi_nrf24_handle;
    UARTChannel uart_esp_channel;
    UARTChannel uart_nmea_channel;
    UARTChannel uart_general_channel;
} XtremeSettings;

void XTREME_SETTINGS_LOAD();
void XTREME_SETTINGS_SAVE();
XtremeSettings* XTREME_SETTINGS();

void XTREME_ASSETS_LOAD();
void XTREME_ASSETS_FREE();

#ifdef __cplusplus
}
#endif
