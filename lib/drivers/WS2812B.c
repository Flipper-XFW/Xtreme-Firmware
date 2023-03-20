/*
    WS2812B FlipperZero driver
    Copyright (C) 2022  Victor Nikitchuk (https://github.com/quen0n)

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

#include "WS2812B.h"
#include <string.h>
#include <stm32wbxx.h>
#include "furi_hal_light.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <input/input.h>

#define TAG "RGB Backlight"
#define RGB_BACKLIGHT_SETTINGS_VERSION 5
#define RGB_BACKLIGHT_SETTINGS_FILE_NAME ".rgb_backlight.settings"
#define RGB_BACKLIGHT_SETTINGS_PATH EXT_PATH(RGB_BACKLIGHT_SETTINGS_FILE_NAME)

static uint8_t WS2812B_ledbuffer[WS2812B_LEDS][3];

static RGBBacklightSettings rgb_settings = {
    .version = RGB_BACKLIGHT_SETTINGS_VERSION,
    .display_color_index = 0,
    .settings_is_loaded = false};

#define COLOR_COUNT (sizeof(colors) / sizeof(WS2812B_Color))

const WS2812B_Color colors[] = {
    {"Orange", 255, 79, 0},
    {"Yellow", 255, 170, 0},
    {"Spring", 167, 255, 0},
    {"Lime", 0, 255, 0},
    {"Aqua", 0, 255, 127},
    {"Cyan", 0, 210, 210},
    {"Azure", 0, 127, 255},
    {"Blue", 0, 0, 255},
    {"Purple", 127, 0, 255},
    {"Magenta", 210, 0, 210},
    {"Pink", 255, 0, 127},
    {"Red", 255, 0, 0},
    {"White", 140, 140, 140},
};

static void _port_init(void) {
    furi_hal_gpio_write(LED_PIN, true);
    furi_hal_gpio_init(LED_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
}

void WS2812B_send(void) {
    _port_init();
    furi_kernel_lock();
    /* Последовательная отправка цветов светодиодов */
    for(uint8_t lednumber = 0; lednumber < WS2812B_LEDS; lednumber++) {
        //Последовательная отправка цветов светодиода
        for(uint8_t color = 0; color < 3; color++) {
            //Последовательная отправка битов цвета
            for(uint8_t i = 7; i != 255; i--) {
                if(WS2812B_ledbuffer[lednumber][color] & (1 << i)) {
                    furi_hal_gpio_write(LED_PIN, true);
                    uint32_t start = DWT->CYCCNT;
                    while((DWT->CYCCNT - start) < 31) {
                    }
                    furi_hal_gpio_write(LED_PIN, false);
                    start = DWT->CYCCNT;
                    while((DWT->CYCCNT - start) < 15) {
                    }
                } else {
                    furi_hal_gpio_write(LED_PIN, true);
                    uint32_t start = DWT->CYCCNT;
                    while((DWT->CYCCNT - start) < 15) {
                    }
                    furi_hal_gpio_write(LED_PIN, false);
                    start = DWT->CYCCNT;
                    while((DWT->CYCCNT - start) < 31) {
                    }
                }
            }
        }
    }
    furi_kernel_unlock();
    //Необходимая задержка - признак окончания передачи
    furi_delay_us(100);
}

uint8_t rgb_backlight_get_color_count(void) {
    return COLOR_COUNT;
}

const char* rgb_backlight_get_color_text(uint8_t index) {
    return colors[index].name;
}

static void rgb_backlight_load_settings(void) {
    _port_init();

    FuriHalRtcBootMode bm = furi_hal_rtc_get_boot_mode();
    if(bm == FuriHalRtcBootModeDfu) {
        rgb_settings.settings_is_loaded = true;
        return;
    }

    RGBBacklightSettings settings;
    File* file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    const size_t settings_size = sizeof(RGBBacklightSettings);

    FURI_LOG_I(TAG, "loading settings from \"%s\"", RGB_BACKLIGHT_SETTINGS_PATH);
    bool fs_result =
        storage_file_open(file, RGB_BACKLIGHT_SETTINGS_PATH, FSAM_READ, FSOM_OPEN_EXISTING);

    if(fs_result) {
        uint16_t bytes_count = storage_file_read(file, &settings, settings_size);

        if(bytes_count != settings_size) {
            fs_result = false;
        }
    }

    if(fs_result) {
        FURI_LOG_I(TAG, "load success");
        if(settings.version != RGB_BACKLIGHT_SETTINGS_VERSION) {
            FURI_LOG_E(
                TAG,
                "version(%d != %d) mismatch",
                settings.version,
                RGB_BACKLIGHT_SETTINGS_VERSION);
        } else {
            memcpy(&rgb_settings, &settings, settings_size);
        }
    } else {
        FURI_LOG_E(TAG, "load failed, %s", storage_file_get_error_desc(file));
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    rgb_settings.settings_is_loaded = true;
};

void rgb_backlight_save_settings(void) {
    RGBBacklightSettings settings;
    File* file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    const size_t settings_size = sizeof(RGBBacklightSettings);

    FURI_LOG_I(TAG, "saving settings to \"%s\"", RGB_BACKLIGHT_SETTINGS_PATH);

    memcpy(&settings, &rgb_settings, settings_size);

    bool fs_result =
        storage_file_open(file, RGB_BACKLIGHT_SETTINGS_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS);

    if(fs_result) {
        uint16_t bytes_count = storage_file_write(file, &settings, settings_size);

        if(bytes_count != settings_size) {
            fs_result = false;
        }
    }

    if(fs_result) {
        FURI_LOG_I(TAG, "save success");
    } else {
        FURI_LOG_E(TAG, "save failed, %s", storage_file_get_error_desc(file));
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
};

RGBBacklightSettings* rgb_backlight_get_settings(void) {
    if(!rgb_settings.settings_is_loaded) {
        rgb_backlight_load_settings();
    }
    return &rgb_settings;
}

void rgb_backlight_set_color(uint8_t color_index) {
    rgb_settings.display_color_index = color_index;
}

void rgb_backlight_update(uint8_t backlight) {
    if(!rgb_settings.settings_is_loaded) {
        rgb_backlight_load_settings();
    }
    for(uint8_t i = 0; i < WS2812B_LEDS; i++) {
        //Green
        WS2812B_ledbuffer[i][0] =
            colors[rgb_settings.display_color_index].green * (backlight / 255.0f);
        //Red
        WS2812B_ledbuffer[i][1] =
            colors[rgb_settings.display_color_index].red * (backlight / 255.0f);
        //Blue
        WS2812B_ledbuffer[i][2] =
            colors[rgb_settings.display_color_index].blue * (backlight / 255.0f);
    }

    WS2812B_send();
}
