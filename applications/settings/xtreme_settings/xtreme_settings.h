#pragma once

#include "xtreme_settings_filename.h"

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>
#include <power/power_service/power.h>

#define MAX_PACK_NAME_LEN 32

#define XTREME_SETTINGS_VERSION (1)
#define XTREME_SETTINGS_PATH INT_PATH(XTREME_SETTINGS_FILE_NAME)
#define XTREME_SETTINGS_MAGIC (0x69)

typedef struct {
    int32_t cycle_anims;
    bool unlock_anims;
    bool nsfw_mode;
    char asset_pack[MAX_PACK_NAME_LEN];
    BatteryStyle battery_style;
    uint16_t anim_speed;
} XtremeSettings;

XtremeSettings* XTREME_SETTINGS();

bool XTREME_SETTINGS_LOAD();

bool XTREME_SETTINGS_SAVE();
