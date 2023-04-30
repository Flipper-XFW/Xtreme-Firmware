#include "settings.h"

#include <lib/flipper_format/flipper_format.h>

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

        // Set default values
        memset(xtreme_settings, 0, sizeof(XtremeSettings));
        strlcpy(xtreme_settings->asset_pack, "", MAX_PACK_NAME_LEN); // SFW
        xtreme_settings->anim_speed = 100; // 100%
        xtreme_settings->cycle_anims = 0; // Meta.txt
        xtreme_settings->unlock_anims = false; // OFF
        xtreme_settings->fallback_anim = true; // ON
        xtreme_settings->wii_menu = true; // ON
        xtreme_settings->bad_pins_format = false; // OFF
        xtreme_settings->lockscreen_time = true; // ON
        xtreme_settings->lockscreen_seconds = false; // OFF
        xtreme_settings->lockscreen_date = true; // ON
        xtreme_settings->lockscreen_statusbar = true; // ON
        xtreme_settings->lockscreen_prompt = true; // ON
        xtreme_settings->battery_icon = BatteryIconBarPercent; // Bar %
        xtreme_settings->status_icons = true; // ON
        xtreme_settings->bar_borders = true; // ON
        xtreme_settings->bar_background = false; // OFF
        xtreme_settings->sort_dirs_first = true; // ON
        xtreme_settings->dark_mode = false; // OFF
        xtreme_settings->bad_bt = false; // USB
        xtreme_settings->bad_bt_remember = false; // OFF
        xtreme_settings->butthurt_timer = 43200; // 12 H
        xtreme_settings->rgb_backlight = false; // OFF

        if(!furi_hal_is_normal_boot()) {
            FURI_LOG_W(TAG, "Load skipped. Device is in special startup mode.");
            return;
        }

        XtremeSettings* x = xtreme_settings;
        Storage* storage = furi_record_open(RECORD_STORAGE);
        FlipperFormat* file = flipper_format_file_alloc(storage);
        if(flipper_format_file_open_existing(file, XTREME_SETTINGS_PATH)) {
            FuriString* string = furi_string_alloc();
            if(flipper_format_read_string(file, "asset_pack", string)) {
                strlcpy(x->asset_pack, furi_string_get_cstr(string), MAX_PACK_NAME_LEN);
            }
            furi_string_free(string);
            flipper_format_rewind(file);
            flipper_format_read_uint32(file, "anim_speed", &x->anim_speed, 1);
            flipper_format_rewind(file);
            flipper_format_read_int32(file, "cycle_anims", &x->cycle_anims, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "unlock_anims", &x->unlock_anims, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "fallback_anim", &x->fallback_anim, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "wii_menu", &x->wii_menu, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "bad_pins_format", &x->bad_pins_format, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "lockscreen_time", &x->lockscreen_time, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "lockscreen_seconds", &x->lockscreen_seconds, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "lockscreen_date", &x->lockscreen_date, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "lockscreen_statusbar", &x->lockscreen_statusbar, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "lockscreen_prompt", &x->lockscreen_prompt, 1);
            flipper_format_rewind(file);
            flipper_format_read_uint32(file, "battery_icon", (uint32_t*)&x->battery_icon, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "status_icons", &x->status_icons, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "bar_borders", &x->bar_borders, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "bar_background", &x->bar_background, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "dark_mode", &x->dark_mode, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "bad_bt", &x->bad_bt, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "bad_bt_remember", &x->bad_bt_remember, 1);
            flipper_format_rewind(file);
            flipper_format_read_int32(file, "butthurt_timer", &x->butthurt_timer, 1);
            flipper_format_rewind(file);
            flipper_format_read_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
        }
        flipper_format_free(file);
        furi_record_close(RECORD_STORAGE);
    }
}

void XTREME_SETTINGS_SAVE() {
    if(xtreme_settings == NULL) {
        XTREME_SETTINGS_LOAD();
    }

    if(!furi_hal_is_normal_boot()) {
        return;
    }

    XtremeSettings* x = xtreme_settings;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_always(file, XTREME_SETTINGS_PATH)) {
        flipper_format_write_string_cstr(file, "asset_pack", x->asset_pack);
        flipper_format_write_uint32(file, "anim_speed", &x->anim_speed, 1);
        flipper_format_write_int32(file, "cycle_anims", &x->cycle_anims, 1);
        flipper_format_write_bool(file, "unlock_anims", &x->unlock_anims, 1);
        flipper_format_write_bool(file, "fallback_anim", &x->fallback_anim, 1);
        flipper_format_write_bool(file, "wii_menu", &x->wii_menu, 1);
        flipper_format_write_bool(file, "bad_pins_format", &x->bad_pins_format, 1);
        flipper_format_write_bool(file, "lockscreen_time", &x->lockscreen_time, 1);
        flipper_format_write_bool(file, "lockscreen_seconds", &x->lockscreen_seconds, 1);
        flipper_format_write_bool(file, "lockscreen_date", &x->lockscreen_date, 1);
        flipper_format_write_bool(file, "lockscreen_statusbar", &x->lockscreen_statusbar, 1);
        flipper_format_write_bool(file, "lockscreen_prompt", &x->lockscreen_prompt, 1);
        flipper_format_write_uint32(file, "battery_icon", (uint32_t*)&x->battery_icon, 1);
        flipper_format_write_bool(file, "status_icons", &x->status_icons, 1);
        flipper_format_write_bool(file, "bar_borders", &x->bar_borders, 1);
        flipper_format_write_bool(file, "bar_background", &x->bar_background, 1);
        flipper_format_write_bool(file, "sort_dirs_first", &x->sort_dirs_first, 1);
        flipper_format_write_bool(file, "dark_mode", &x->dark_mode, 1);
        flipper_format_write_bool(file, "bad_bt", &x->bad_bt, 1);
        flipper_format_write_bool(file, "bad_bt_remember", &x->bad_bt_remember, 1);
        flipper_format_write_int32(file, "butthurt_timer", &x->butthurt_timer, 1);
        flipper_format_write_bool(file, "rgb_backlight", &x->rgb_backlight, 1);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}
