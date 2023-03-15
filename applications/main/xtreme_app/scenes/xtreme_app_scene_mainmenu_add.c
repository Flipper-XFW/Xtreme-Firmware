#include "../xtreme_app.h"

enum FileBrowserResult {
    FileBrowserResultOk,
};

static bool xtreme_app_scene_mainmenu_add_file_browser_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
// #ifndef APP_FAP_LOADER
//     Storage* storage = furi_record_open(RECORD_STORAGE);
//     bool success = fap_loader_load_name_and_icon(file_path, storage, icon_ptr, item_name);
//     furi_record_close(RECORD_STORAGE);
// #else
    UNUSED(file_path);
    UNUSED(icon_ptr);
    UNUSED(item_name);
    bool success = false;
// #endif
    return success;
}

void xtreme_app_scene_mainmenu_add_on_enter(void* context) {
    XtremeApp* app = context;
    FuriString* file_path = furi_string_alloc_set_str(EXT_PATH("apps"));

    const DialogsFileBrowserOptions browser_options = {
        .extension = ".fap",
        .skip_assets = true,
        .hide_ext = true,
        .item_loader_callback = xtreme_app_scene_mainmenu_add_file_browser_callback,
        .item_loader_context = app,
        .base_path = EXT_PATH("apps"),
    };

    if(dialog_file_browser_show(app->dialogs, file_path, file_path, &browser_options)) {
        CharList_push_back(app->mainmenu_apps, strdup(furi_string_get_cstr(file_path)));
        app->save_mainmenu_apps = true;
    }

    furi_string_free(file_path);

    view_dispatcher_send_custom_event(app->view_dispatcher, FileBrowserResultOk);
}

bool xtreme_app_scene_mainmenu_add_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case FileBrowserResultOk:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_mainmenu_add_on_exit(void* context) {
    UNUSED(context);
}
