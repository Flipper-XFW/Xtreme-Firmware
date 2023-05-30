#include <flipper_application/flipper_application.h>
#include <loader/firmware_api/firmware_api.h>
#include <furi_hal.h>
#include <toolbox/path.h>

#define TAG "Preload"

void* loader_preload(Storage* storage, const char* path) {
    FlipperApplication* app = flipper_application_alloc(storage, firmware_api_interface);
    size_t start = furi_get_tick();

    FURI_LOG_I(TAG, "Loading %s", path);

    FlipperApplicationPreloadStatus preload_res = flipper_application_preload(app, path);
    if(preload_res != FlipperApplicationPreloadStatusSuccess) {
        return NULL;
    }

    FURI_LOG_I(TAG, "Mapping");
    FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(app);
    if(load_status != FlipperApplicationLoadStatusSuccess) {
        const char* err_msg = flipper_application_load_status_to_string(load_status);
        FURI_LOG_E(TAG, "Failed to map to memory %s: %s", path, err_msg);
        return NULL;
    }

    FURI_LOG_I(TAG, "Loaded in %ums", (size_t)(furi_get_tick() - start));
    return app;
}

void loader_preload_start(void* _app, const char* path) {
    FlipperApplication* app = _app;
    FURI_LOG_I(TAG, "Starting app");

    FuriThread* thread = flipper_application_spawn(app, NULL);

    /* This flag is set by the debugger - to break on app start */
    if(furi_hal_debug_is_gdb_session_active()) {
        FURI_LOG_W(TAG, "Triggering BP for debugger");
        /* After hitting this, you can set breakpoints in your .fap's code
             * Note that you have to toggle breakpoints that were set before */
        __asm volatile("bkpt 0");
    }

    FuriString* app_name = furi_string_alloc();
    path_extract_filename_no_ext(path, app_name);
    furi_thread_set_appid(thread, furi_string_get_cstr(app_name));
    furi_string_free(app_name);

    furi_thread_start(thread);
    furi_thread_join(thread);

    flipper_application_despawn(app);
}
