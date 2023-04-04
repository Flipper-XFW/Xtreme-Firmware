#include "power_settings.h"

bool SAVE_POWER_SETTINGS(uint32_t* x) {
    return saved_struct_save(
        POWER_SETTINGS_PATH, x, sizeof(uint32_t), POWER_SETTINGS_MAGIC, POWER_SETTINGS_VER);
}

bool LOAD_POWER_SETTINGS(uint32_t* x) {
    bool ret = saved_struct_load(
        POWER_SETTINGS_PATH, x, sizeof(uint32_t), POWER_SETTINGS_MAGIC, POWER_SETTINGS_VER);

    if(!ret) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        storage_common_copy(storage, POWER_SETTINGS_OLD_PATH, POWER_SETTINGS_PATH);
        storage_common_remove(storage, POWER_SETTINGS_OLD_PATH);
        furi_record_close(RECORD_STORAGE);
        ret = saved_struct_load(
            POWER_SETTINGS_PATH, x, sizeof(uint32_t), POWER_SETTINGS_MAGIC, POWER_SETTINGS_VER);
    }

    return ret;
}
