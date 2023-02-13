#pragma once

#include "settings_filename.h"

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>
#include <power/power_service/power.h>

#define MAX_PACK_NAME_LEN 32

#define XTREME_SETTINGS_VERSION (2)
#define XTREME_SETTINGS_PATH INT_PATH(XTREME_SETTINGS_FILE_NAME)
#define XTREME_SETTINGS_MAGIC (0x69)

// Some settings function backwards (logically) in
// order to fit the default value we want
// (values will default to 0 / false)
typedef struct {
    char asset_pack[MAX_PACK_NAME_LEN];
    uint16_t anim_speed;
    int32_t cycle_anims;
    bool unlock_anims;
    BatteryIcon battery_icon;
    bool status_icons;
    bool bar_borders;
    bool bar_background;
    bool bad_bt;
    bool sort_ignore_dirs;
} XtremeSettings;

XtremeSettings* XTREME_SETTINGS();

bool XTREME_SETTINGS_LOAD();

bool XTREME_SETTINGS_SAVE();
