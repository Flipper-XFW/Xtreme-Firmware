#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <furi_hal_version.h>
#include <furi_hal_memory.h>
#include <furi_hal_rtc.h>
#include <storage/storage.h>
#include <bt/bt_settings.h>
#include <bt/bt_service/bt_i.h>
#include <power/power_settings.h>
#include <desktop/desktop_settings.h>
#include <notification/notification_app.h>
#include <dolphin/helpers/dolphin_state.h>
#include <applications/main/u2f/u2f_data.h>
#include <applications/main/archive/helpers/archive_favorites.h>
#include <namespoof.h>
#include <xtreme.h>

#define TAG "Flipper"

static void flipper_print_version(const char* target, const Version* version) {
    if(version) {
        FURI_LOG_I(
            TAG,
            "\r\n\t%s version:\t%s\r\n"
            "\tBuild date:\t\t%s\r\n"
            "\tGit Commit:\t\t%s (%s)%s\r\n"
            "\tGit Branch:\t\t%s",
            target,
            version_get_version(version),
            version_get_builddate(version),
            version_get_githash(version),
            version_get_gitbranchnum(version),
            version_get_dirty_flag(version) ? " (dirty)" : "",
            version_get_gitbranch(version));
    } else {
        FURI_LOG_I(TAG, "No build info for %s", target);
    }
}

void flipper_migrate_files() {
    if(!furi_hal_is_normal_boot()) return;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Revert cringe
    storage_common_remove(storage, INT_PATH(".passport.settings"));
    storage_common_remove(storage, INT_PATH(".region_data"));

    // Migrate files
    storage_common_copy(storage, ARCHIVE_FAV_OLD_PATH, ARCHIVE_FAV_PATH);
    storage_common_remove(storage, ARCHIVE_FAV_OLD_PATH);
    storage_common_copy(storage, DESKTOP_KEYBINDS_OLD_PATH, DESKTOP_KEYBINDS_PATH);
    storage_common_remove(storage, DESKTOP_KEYBINDS_OLD_PATH);
    // Int -> Ext
    storage_common_copy(storage, BT_SETTINGS_OLD_PATH, BT_SETTINGS_PATH);
    storage_common_remove(storage, BT_SETTINGS_OLD_PATH);
    storage_common_copy(storage, DOLPHIN_STATE_OLD_PATH, DOLPHIN_STATE_PATH);
    storage_common_remove(storage, DOLPHIN_STATE_OLD_PATH);
    storage_common_copy(storage, POWER_SETTINGS_OLD_PATH, POWER_SETTINGS_PATH);
    storage_common_remove(storage, POWER_SETTINGS_OLD_PATH);
    storage_common_copy(storage, BT_KEYS_STORAGE_OLD_PATH, BT_KEYS_STORAGE_PATH);
    storage_common_remove(storage, BT_KEYS_STORAGE_OLD_PATH);
    storage_common_copy(storage, NOTIFICATION_SETTINGS_OLD_PATH, NOTIFICATION_SETTINGS_PATH);
    storage_common_remove(storage, NOTIFICATION_SETTINGS_OLD_PATH);
    // Ext -> Int
    storage_common_copy(storage, DESKTOP_SETTINGS_OLD_PATH, DESKTOP_SETTINGS_PATH);
    storage_common_remove(storage, DESKTOP_SETTINGS_OLD_PATH);

    // Special care for U2F
    FileInfo file_info;
    if(storage_common_stat(storage, U2F_CNT_OLD_FILE, &file_info) == FSE_OK &&
       file_info.size > 200) { // Is on Int and has content
        storage_common_move(storage, U2F_CNT_OLD_FILE, U2F_CNT_FILE); // Int -> Ext
    }
    storage_common_copy(storage, U2F_KEY_OLD_FILE, U2F_KEY_FILE); // Ext -> Int

    storage_common_migrate(storage, EXT_PATH("dolphin_custom"), EXT_PATH("asset_packs"));

    furi_record_close(RECORD_STORAGE);
}

void flipper_start_service(const FlipperInternalApplication* service) {
    FURI_LOG_D(TAG, "Starting service %s", service->name);

    FuriThread* thread =
        furi_thread_alloc_ex(service->name, service->stack_size, service->app, NULL);
    furi_thread_mark_as_service(thread);
    furi_thread_set_appid(thread, service->appid);

    furi_thread_start(thread);
}

void flipper_init() {
    flipper_print_version("Firmware", furi_hal_version_get_firmware_version());

    FURI_LOG_I(TAG, "Boot mode %d, starting services", furi_hal_rtc_get_boot_mode());

    // Start storage service first, thanks OFW :/
    flipper_start_service(&FLIPPER_SERVICES[0]);

    flipper_migrate_files();

    NAMESPOOF_INIT();
    XTREME_SETTINGS_LOAD();
    XTREME_ASSETS_LOAD();

    // Everything else
    for(size_t i = 1; i < FLIPPER_SERVICES_COUNT; i++) {
        flipper_start_service(&FLIPPER_SERVICES[i]);
    }

    FURI_LOG_I(TAG, "Startup complete");
}

void vApplicationGetIdleTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = memmgr_alloc_from_pool(sizeof(StaticTask_t));
    *stack_ptr = memmgr_alloc_from_pool(sizeof(StackType_t) * configIDLE_TASK_STACK_DEPTH);
    *stack_size = configIDLE_TASK_STACK_DEPTH;
}

void vApplicationGetTimerTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = memmgr_alloc_from_pool(sizeof(StaticTask_t));
    *stack_ptr = memmgr_alloc_from_pool(sizeof(StackType_t) * configTIMER_TASK_STACK_DEPTH);
    *stack_size = configTIMER_TASK_STACK_DEPTH;
}
