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

#ifndef WS2812B_H_
#define WS2812B_H_

#include "furi.h"
#include <stdint.h>
#include <stdbool.h>
#include <input/input.h>

typedef struct {
    uint8_t version;
    uint8_t display_color_index;
    bool settings_is_loaded;
} RGBBacklightSettings;

typedef struct {
    char* name;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} WS2812B_Color;

#define LED_PIN &gpio_ext_pa7
#define WS2812B_LEDS 3

void rgb_backlight_save_settings(void);

void rgb_backlight_update(uint8_t backlight);

void rgb_backlight_set_color(uint8_t color_index);
void rgb_backlight_set_color(uint8_t color_index);

RGBBacklightSettings* rgb_backlight_get_settings(void);
uint8_t rgb_backlight_get_color_count(void);
const char* rgb_backlight_get_color_text(uint8_t index);

#endif /* WS2812B_H_ */