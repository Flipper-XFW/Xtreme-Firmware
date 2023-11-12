#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <furi_hal_version.h>
#include <furi_hal_memory.h>
#include <furi_hal_rtc.h>
#include <storage/storage.h>
#include <gui/canvas_i.h>
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

#include <FreeRTOS.h>

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
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Revert cringe
    FURI_LOG_I(TAG, "Migrate: Remove unused files");
    storage_common_remove(storage, INT_PATH(".passport.settings"));
    storage_common_remove(storage, INT_PATH(".region_data"));

    // Migrate files
    FURI_LOG_I(TAG, "Migrate: Renames on external");
    storage_common_copy(storage, ARCHIVE_FAV_OLD_PATH, ARCHIVE_FAV_PATH);
    storage_common_remove(storage, ARCHIVE_FAV_OLD_PATH);
    storage_common_copy(storage, DESKTOP_KEYBINDS_OLD_PATH, DESKTOP_KEYBINDS_PATH);
    storage_common_remove(storage, DESKTOP_KEYBINDS_OLD_PATH);
    // Int -> Ext
    FURI_LOG_I(TAG, "Migrate: Internal to External");
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
    FURI_LOG_I(TAG, "Migrate: External to Internal");
    storage_common_copy(storage, DESKTOP_SETTINGS_OLD_PATH, DESKTOP_SETTINGS_PATH);
    storage_common_remove(storage, DESKTOP_SETTINGS_OLD_PATH);

    // Special care for U2F
    FURI_LOG_I(TAG, "Migrate: U2F");
    FileInfo file_info;
    if(storage_common_stat(storage, U2F_CNT_OLD_FILE, &file_info) == FSE_OK &&
       file_info.size > 200) { // Is on Int and has content
        storage_common_move(storage, U2F_CNT_OLD_FILE, U2F_CNT_FILE); // Int -> Ext
    }
    storage_common_copy(storage, U2F_KEY_OLD_FILE, U2F_KEY_FILE); // Ext -> Int

    FURI_LOG_I(TAG, "Migrate: Asset Packs");
    storage_common_migrate(storage, XTREME_ASSETS_OLD_PATH, XTREME_ASSETS_PATH);

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

static void flipper_boot_status(Canvas* canvas, const char* text) {
    FURI_LOG_I(TAG, text);
    canvas_reset(canvas);
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, text);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 24, AlignCenter, AlignCenter, "Xtreme is Booting");
    canvas_commit(canvas);
}

void flipper_init() {
    furi_hal_light_sequence("rgb WB");
    flipper_print_version("Firmware", furi_hal_version_get_firmware_version());
    FURI_LOG_I(TAG, "Boot mode %d", furi_hal_rtc_get_boot_mode());
    Canvas* canvas = canvas_init();

    // Start storage service first, thanks OFW :/
    flipper_boot_status(canvas, "Initializing Storage");
    flipper_start_service(&FLIPPER_SERVICES[0]);

    if(furi_hal_is_normal_boot()) {
        furi_record_open(RECORD_STORAGE);
        furi_record_close(RECORD_STORAGE);
        flipper_boot_status(canvas, "Migrating Files");
        flipper_migrate_files();
        flipper_boot_status(canvas, "Starting Namespoof");
        NAMESPOOF_INIT();
        flipper_boot_status(canvas, "Loading Xtreme Settings");
        XTREME_SETTINGS_LOAD();
        furi_hal_light_sequence("rgb RB");
        flipper_boot_status(canvas, "Loading Xtreme Assets");
        XTREME_ASSETS_LOAD();
    } else {
        FURI_LOG_I(TAG, "Special boot, skipping optional components");
    }

    // Everything else
    flipper_boot_status(canvas, "Initializing Services");
    for(size_t i = 1; i < FLIPPER_SERVICES_COUNT; i++) {
        flipper_start_service(&FLIPPER_SERVICES[i]);
    }
    canvas_free(canvas);

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
