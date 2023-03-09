#pragma once

#include "settings_filename.h"

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>
#include <power/power_service/power.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PACK_NAME_LEN 32

#define XTREME_SETTINGS_VERSION (5)
#define XTREME_SETTINGS_PATH_OLD INT_PATH(XTREME_SETTINGS_FILE_NAME)
#define XTREME_SETTINGS_PATH EXT_PATH(XTREME_SETTINGS_FILE_NAME)
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
    bool bad_bt_remember;
    int32_t butthurt_timer;
    bool sort_dirs_first;
    bool dark_mode;
    bool left_handed;
} XtremeSettings;

XtremeSettings* XTREME_SETTINGS();

void XTREME_SETTINGS_LOAD();

bool XTREME_SETTINGS_SAVE();

#ifdef __cplusplus
}
#endif
