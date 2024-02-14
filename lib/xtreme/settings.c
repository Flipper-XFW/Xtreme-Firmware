#include "xtreme.h"
#include <furi_hal.h>
#include <rgb_backlight.h>
#include <flipper_format/flipper_format.h>

#define TAG "XtremeSettings"

XtremeSettings xtreme_settings = {
    .asset_pack = "", // Default
    .anim_speed = 100, // 100%
    .cycle_anims = 0, // Meta.txt
    .unlock_anims = false, // OFF
    .credits_anim = true, // ON
    .menu_style = MenuStyleWii, // Wii
    .lock_on_boot = false, // OFF
    .bad_pins_format = false, // OFF
    .allow_locked_rpc_commands = false, // OFF
    .lockscreen_poweroff = true, // ON
    .lockscreen_time = true, // ON
    .lockscreen_seconds = false, // OFF
    .lockscreen_date = true, // ON
    .lockscreen_statusbar = true, // ON
    .lockscreen_prompt = true, // ON
    .lockscreen_transparent = false, // OFF
    .battery_icon = BatteryIconBarPercent, // Bar %
    .statusbar_clock = false, // OFF
    .status_icons = true, // ON
    .bar_borders = true, // ON
    .bar_background = false, // OFF
    .sort_dirs_first = true, // ON
    .show_hidden_files = false, // OFF
    .show_internal_tab = false, // OFF
    .favorite_timeout = 0, // OFF
    .bad_bt = false, // USB
    .bad_bt_remember = false, // OFF
    .dark_mode = false, // OFF
    .rgb_backlight = false, // OFF
    .butthurt_timer = 21600, // 6 H
    .charge_cap = 100, // 100%
    .spi_cc1101_handle = SpiDefault, // &furi_hal_spi_bus_handle_external
    .spi_nrf24_handle = SpiDefault, // &furi_hal_spi_bus_handle_external
    .uart_esp_channel = FuriHalSerialIdUsart, // pin 13,14
    .uart_nmea_channel = FuriHalSerialIdUsart, // pin 13,14
    .uart_general_channel = FuriHalSerialIdUsart, // pin 13,14
    .file_naming_prefix_after = false, // Before
};

typedef enum {
    xtreme_settings_type_str,
    xtreme_settings_type_int,
    xtreme_settings_type_uint,
    xtreme_settings_type_enum,
    xtreme_settings_type_bool,
} xtreme_settings_type;

static const struct {
    xtreme_settings_type type;
    const char* key;
    void* val;
    union {
        size_t str_len;
        struct {
            int32_t i_min;
            int32_t i_max;
        };
        struct {
            uint32_t u_min;
            uint32_t u_max;
        };
        uint8_t e_cnt;
    };
#define clamp(t, min, max) .t##_min = min, .t##_max = max
#define setting(t, n) .type = xtreme_settings_type##t, .key = #n, .val = &xtreme_settings.n
#define setting_str(n) setting(_str, n), .str_len = sizeof(xtreme_settings.n)
#define setting_int(n, min, max) setting(_int, n), clamp(i, min, max)
#define setting_uint(n, min, max) setting(_uint, n), clamp(u, min, max)
#define setting_enum(n, cnt) setting(_enum, n), .e_cnt = cnt
#define setting_bool(n) setting(_bool, n)
} xtreme_settings_entries[] = {
    {setting_str(asset_pack)},
    {setting_uint(anim_speed, 25, 300)},
    {setting_int(cycle_anims, -1, 86400)},
    {setting_bool(unlock_anims)},
    {setting_bool(credits_anim)},
    {setting_enum(menu_style, MenuStyleCount)},
    {setting_bool(bad_pins_format)},
    {setting_bool(allow_locked_rpc_commands)},
    {setting_bool(lock_on_boot)},
    {setting_bool(lockscreen_poweroff)},
    {setting_bool(lockscreen_time)},
    {setting_bool(lockscreen_seconds)},
    {setting_bool(lockscreen_date)},
    {setting_bool(lockscreen_statusbar)},
    {setting_bool(lockscreen_prompt)},
    {setting_bool(lockscreen_transparent)},
    {setting_enum(battery_icon, BatteryIconCount)},
    {setting_bool(statusbar_clock)},
    {setting_bool(status_icons)},
    {setting_bool(bar_borders)},
    {setting_bool(bar_background)},
    {setting_bool(sort_dirs_first)},
    {setting_bool(show_hidden_files)},
    {setting_bool(show_internal_tab)},
    {setting_uint(favorite_timeout, 0, 60)},
    {setting_bool(bad_bt)},
    {setting_bool(bad_bt_remember)},
    {setting_bool(dark_mode)},
    {setting_bool(rgb_backlight)},
    {setting_uint(butthurt_timer, 0, 172800)},
    {setting_uint(charge_cap, 5, 100)},
    {setting_enum(spi_cc1101_handle, SpiCount)},
    {setting_enum(spi_nrf24_handle, SpiCount)},
    {setting_enum(uart_esp_channel, FuriHalSerialIdMax)},
    {setting_enum(uart_nmea_channel, FuriHalSerialIdMax)},
    {setting_enum(uart_general_channel, FuriHalSerialIdMax)},
    {setting_bool(file_naming_prefix_after)},
};

void xtreme_settings_load() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, XTREME_SETTINGS_PATH)) {
        FuriString* val_str = furi_string_alloc();
        int32_t val_int;
        uint32_t val_uint;
        bool val_bool;

        bool ok;
        for(size_t entry_i = 0; entry_i < COUNT_OF(xtreme_settings_entries); entry_i++) {
#define entry xtreme_settings_entries[entry_i]
            switch(entry.type) {
            case xtreme_settings_type_str:
                ok = flipper_format_read_string(file, entry.key, val_str);
                if(ok) strlcpy((char*)entry.val, furi_string_get_cstr(val_str), entry.str_len);
                break;
            case xtreme_settings_type_int:
                ok = flipper_format_read_int32(file, entry.key, &val_int, 1);
                if(ok) *(int32_t*)entry.val = CLAMP(val_int, entry.i_max, entry.i_min);
                break;
            case xtreme_settings_type_uint:
                ok = flipper_format_read_uint32(file, entry.key, &val_uint, 1);
                if(ok) *(uint32_t*)entry.val = CLAMP(val_uint, entry.u_max, entry.u_min);
                break;
            case xtreme_settings_type_enum:
                ok = flipper_format_read_uint32(file, entry.key, &val_uint, 1);
                if(ok) *(uint8_t*)entry.val = CLAMP(val_uint, entry.e_cnt - 1U, 0U);
                break;
            case xtreme_settings_type_bool:
                ok = flipper_format_read_bool(file, entry.key, &val_bool, 1);
                if(ok) *(bool*)entry.val = val_bool;
                break;
            default:
                continue;
            }
            if(!ok) flipper_format_rewind(file);
        }

        furi_string_free(val_str);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    rgb_backlight_load_settings(xtreme_settings.rgb_backlight);
}

void xtreme_settings_save() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_always(file, XTREME_SETTINGS_PATH)) {
        uint32_t tmp_enum;
        for(size_t entry_i = 0; entry_i < COUNT_OF(xtreme_settings_entries); entry_i++) {
#define entry xtreme_settings_entries[entry_i]
            switch(entry.type) {
            case xtreme_settings_type_str:
                flipper_format_write_string_cstr(file, entry.key, (char*)entry.val);
                break;
            case xtreme_settings_type_int:
                flipper_format_write_int32(file, entry.key, (int32_t*)entry.val, 1);
                break;
            case xtreme_settings_type_uint:
                flipper_format_write_uint32(file, entry.key, (uint32_t*)entry.val, 1);
                break;
            case xtreme_settings_type_enum:
                tmp_enum = *(uint8_t*)entry.val;
                flipper_format_write_uint32(file, entry.key, &tmp_enum, 1);
                break;
            case xtreme_settings_type_bool:
                flipper_format_write_bool(file, entry.key, (bool*)entry.val, 1);
                break;
            default:
                continue;
            }
        }
    }

    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}
