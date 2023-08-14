/*
    RGB backlight FlipperZero driver
    Copyright (C) 2022-2023 Victor Nikitchuk (https://github.com/quen0n)
    Heavily modified by Willy-JL and Z3bro

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "rgb_backlight.h"
#include <furi_hal.h>
#include <storage/storage.h>
#include <toolbox/saved_struct.h>

#define RGB_BACKLIGHT_SETTINGS_MAGIC 0x15
#define RGB_BACKLIGHT_SETTINGS_VERSION 6
#define RGB_BACKLIGHT_SETTINGS_PATH CFG_PATH("rgb_backlight.settings")

static struct {
    uint8_t display_color_index;
    RGBBacklightRainbowMode rainbow_mode;
    uint8_t rainbow_speed;
    uint32_t rainbow_interval;
} rgb_settings = {
    .display_color_index = 0,
    .rainbow_mode = RGBBacklightRainbowModeOff,
    .rainbow_speed = 5,
    .rainbow_interval = 1000,
};

static const struct {
    char* name;
    RgbColor color;
} colors[] = {
    {"Orange", {255, 69, 0}},
    {"Red", {255, 0, 0}},
    {"Maroon", {128, 0, 0}},
    {"Yellow", {255, 255, 0}},
    {"Olive", {128, 128, 0}},
    {"Lime", {0, 255, 0}},
    {"Green", {0, 128, 0}},
    {"Aqua", {0, 255, 127}},
    {"Cyan", {0, 210, 210}},
    {"Azure", {0, 127, 255}},
    {"Teal", {0, 128, 128}},
    {"Blue", {0, 0, 255}},
    {"Navy", {0, 0, 128}},
    {"Purple", {128, 0, 128}},
    {"Fuchsia", {255, 0, 255}},
    {"Pink", {173, 31, 173}},
    {"Brown", {165, 42, 42}},
    {"White", {255, 192, 203}},
};

static struct {
    bool settings_loaded;
    bool last_rainbow;
    uint8_t last_brightness;
    uint8_t last_color_index;
    FuriTimer* rainbow_timer;
    HsvColor rainbow_hsv;
} rgb_state = {
    .settings_loaded = false,
    .last_rainbow = true,
    .last_brightness = 0,
    .last_color_index = 255,
    .rainbow_timer = NULL,
    .rainbow_hsv =
        {
            .h = 0,
            .s = 255,
            .v = 255,
        },
};

uint8_t rgb_backlight_get_color_count(void) {
    return COUNT_OF(colors);
}

const char* rgb_backlight_get_color_text(uint8_t index) {
    return colors[index].name;
}

static void rainbow_timer(void* ctx) {
    UNUSED(ctx);
    rgb_backlight_update(rgb_state.last_brightness, true);
}

static void rainbow_configure() {
    if(rgb_settings.rainbow_mode != RGBBacklightRainbowModeOff) {
        if(rgb_state.rainbow_timer == NULL) {
            rgb_state.rainbow_timer = furi_timer_alloc(rainbow_timer, FuriTimerTypePeriodic, NULL);
        } else {
            furi_timer_stop(rgb_state.rainbow_timer);
        }
        furi_timer_start(rgb_state.rainbow_timer, rgb_settings.rainbow_interval);
    } else if(rgb_state.rainbow_timer != NULL) {
        furi_timer_stop(rgb_state.rainbow_timer);
        furi_timer_free(rgb_state.rainbow_timer);
        rgb_state.rainbow_timer = NULL;
    }
}

void rgb_backlight_load_settings(void) {
    //Не загружать данные из внутренней памяти при загрузке в режиме DFU
    if(!furi_hal_is_normal_boot() || rgb_state.settings_loaded) {
        rgb_state.settings_loaded = true;
        return;
    }

    saved_struct_load(
        RGB_BACKLIGHT_SETTINGS_PATH,
        &rgb_settings,
        sizeof(rgb_settings),
        RGB_BACKLIGHT_SETTINGS_MAGIC,
        RGB_BACKLIGHT_SETTINGS_VERSION);

    rgb_state.settings_loaded = true;
    rainbow_configure();
}

void rgb_backlight_save_settings(void) {
    saved_struct_save(
        RGB_BACKLIGHT_SETTINGS_PATH,
        &rgb_settings,
        sizeof(rgb_settings),
        RGB_BACKLIGHT_SETTINGS_MAGIC,
        RGB_BACKLIGHT_SETTINGS_VERSION);
}

void rgb_backlight_set_color(uint8_t color_index) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    if(color_index > (rgb_backlight_get_color_count() - 1)) color_index = 0;
    rgb_settings.display_color_index = color_index;
}

uint8_t rgb_backlight_get_color() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.display_color_index;
}

void rgb_backlight_set_rainbow_mode(RGBBacklightRainbowMode rainbow_mode) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    if(rainbow_mode > (RGBBacklightRainbowModeCount - 1)) rainbow_mode = 0;
    rgb_settings.rainbow_mode = rainbow_mode;
    rainbow_configure();
}

RGBBacklightRainbowMode rgb_backlight_get_rainbow_mode() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_mode;
}

void rgb_backlight_set_rainbow_speed(uint8_t rainbow_speed) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_speed = rainbow_speed;
}

uint8_t rgb_backlight_get_rainbow_speed() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_speed;
}

void rgb_backlight_set_rainbow_interval(uint32_t rainbow_interval) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_interval = rainbow_interval;
    rainbow_configure();
}

uint32_t rgb_backlight_get_rainbow_interval() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_interval;
}

void rgb_backlight_update(uint8_t brightness, bool tick) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }

    switch(rgb_settings.rainbow_mode) {
    case RGBBacklightRainbowModeOff: {
        if(rgb_state.last_brightness == brightness &&
           rgb_state.last_color_index == rgb_settings.display_color_index &&
           !rgb_state.last_rainbow) {
            return;
        }
        rgb_state.last_rainbow = false;
        rgb_state.last_color_index = rgb_settings.display_color_index;

        RgbColor rgb = colors[rgb_settings.display_color_index].color;
        rgb.r *= (brightness / 255.0f);
        rgb.g *= (brightness / 255.0f);
        rgb.b *= (brightness / 255.0f);
        FURI_LOG_I("RgbBacklight", "rgb %d %d %d", rgb.r, rgb.g, rgb.b);

        for(uint8_t i = 0; i < SK6805_get_led_count(); i++) {
            SK6805_set_led_color(i, rgb.r, rgb.g, rgb.b);
        }
        break;
    }

    case RGBBacklightRainbowModeWave:
    case RGBBacklightRainbowModeSolid: {
        rgb_state.last_rainbow = true;

        if(tick && brightness) {
            rgb_state.rainbow_hsv.h += rgb_settings.rainbow_speed;
        } else {
            if(rgb_state.last_brightness == brightness && rgb_state.last_rainbow) {
                return;
            }
            rgb_state.rainbow_hsv.v = brightness;
        }

        HsvColor hsv = rgb_state.rainbow_hsv;
        FURI_LOG_I("RgbBacklight", "hsv %d %d %d", hsv.h, hsv.s, hsv.v);
        RgbColor rgb = hsv2rgb(hsv);
        FURI_LOG_I("RgbBacklight", "rgb %d %d %d", rgb.r, rgb.g, rgb.b);

        for(uint8_t i = 0; i < SK6805_get_led_count(); i++) {
            if(i && rgb_settings.rainbow_mode == RGBBacklightRainbowModeWave) {
                hsv.h += (50 * i);
                rgb = hsv2rgb(hsv);
                FURI_LOG_I("RgbBacklight", "rgb %d %d %d", rgb.r, rgb.g, rgb.b);
            }
            SK6805_set_led_color(i, rgb.r, rgb.g, rgb.b);
        }
        break;
    }

    default:
        return;
    }

    rgb_state.last_brightness = brightness;
    SK6805_update();
}
