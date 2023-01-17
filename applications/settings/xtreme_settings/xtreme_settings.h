#pragma once

#include "xtreme_settings_filename.h"

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>

#define XTREME_SETTINGS_VERSION (1)
#define XTREME_SETTINGS_PATH INT_PATH(XTREME_SETTINGS_FILE_NAME)
#define XTREME_SETTINGS_MAGIC (0x69)

typedef struct {
    int32_t cycle_animation;
    // uint8_t sfw_mode;
} XtremeSettings;

bool XTREME_SETTINGS_LOAD(XtremeSettings* xtreme_settings);

bool XTREME_SETTINGS_SAVE(XtremeSettings* xtreme_settings);
