#include "../xtreme_app.h"

enum SubmenuIndex {
    SubmenuIndexMainApp,
    SubmenuIndexExternalApp,
};

static bool fap_selector_item_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool success = flipper_application_load_name_and_icon(file_path, storage, icon_ptr, item_name);
    furi_record_close(RECORD_STORAGE);
    return success;
}

static void
    xtreme_app_scene_interface_mainmenu_add_submenu_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneInterfaceMainmenuAdd, index);

    switch(index) {
    case SubmenuIndexMainApp:
        scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceMainmenuAddMain);
        break;
    case SubmenuIndexExternalApp: {
        const DialogsFileBrowserOptions browser_options = {
            .extension = ".fap",
            .icon = &I_unknown_10px,
            .skip_assets = true,
            .hide_ext = true,
            .item_loader_callback = fap_selector_item_callback,
            .item_loader_context = app,
            .base_path = EXT_PATH("apps"),
        };
        FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));
        if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
            CharList_push_back(app->mainmenu_app_exes, strdup(furi_string_get_cstr(temp_path)));
            Storage* storage = furi_record_open(RECORD_STORAGE);
            flipper_application_load_name_and_icon(temp_path, storage, NULL, temp_path);
            furi_record_close(RECORD_STORAGE);
            if(furi_string_start_with_str(temp_path, "[")) {
                size_t trim = furi_string_search_str(temp_path, "] ", 1);
                if(trim != FURI_STRING_FAILURE) {
                    furi_string_right(temp_path, trim + 2);
                }
            }
            CharList_push_back(app->mainmenu_app_labels, strdup(furi_string_get_cstr(temp_path)));
            app->mainmenu_app_index = CharList_size(app->mainmenu_app_labels) - 1;
            app->save_mainmenu_apps = true;
            app->require_reboot = true;
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, XtremeAppSceneInterfaceMainmenu);
        }
        furi_string_free(temp_path);
        break;
    }
    default:
        break;
    }
}

void xtreme_app_scene_interface_mainmenu_add_on_enter(void* context) {
    XtremeApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_set_header(submenu, "Add Menu App:");

    submenu_add_item(
        submenu,
        "Main App",
        SubmenuIndexMainApp,
        xtreme_app_scene_interface_mainmenu_add_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "External App",
        SubmenuIndexExternalApp,
        xtreme_app_scene_interface_mainmenu_add_submenu_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewSubmenu);
}

bool xtreme_app_scene_interface_mainmenu_add_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void xtreme_app_scene_interface_mainmenu_add_on_exit(void* context) {
    XtremeApp* app = context;
    submenu_reset(app->submenu);
}
