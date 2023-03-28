#include "bt_settings.h"

#include <furi.h>
#include <lib/toolbox/saved_struct.h>
#include <storage/storage.h>

#define BT_SETTINGS_OLD_PATH INT_PATH(".bt.settings")
#define BT_SETTINGS_PATH CFG_PATH("bt.settings")
#define BT_SETTINGS_VERSION (0)
#define BT_SETTINGS_MAGIC (0x19)

bool bt_settings_load(BtSettings* bt_settings) {
    furi_assert(bt_settings);

    bool ret = saved_struct_load(
        BT_SETTINGS_PATH, bt_settings, sizeof(BtSettings), BT_SETTINGS_MAGIC, BT_SETTINGS_VERSION);

    if(!ret) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        storage_common_copy(storage, BT_SETTINGS_OLD_PATH, BT_SETTINGS_PATH);
        storage_common_remove(storage, BT_SETTINGS_OLD_PATH);
        furi_record_close(RECORD_STORAGE);
        ret = saved_struct_load(
            BT_SETTINGS_PATH,
            bt_settings,
            sizeof(BtSettings),
            BT_SETTINGS_MAGIC,
            BT_SETTINGS_VERSION);
    }

    return ret;
}

bool bt_settings_save(BtSettings* bt_settings) {
    furi_assert(bt_settings);

    return saved_struct_save(
        BT_SETTINGS_PATH, bt_settings, sizeof(BtSettings), BT_SETTINGS_MAGIC, BT_SETTINGS_VERSION);
}
