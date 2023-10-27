#include "../desktop_settings_app.h"
#include "applications.h"
#include "desktop_settings_scene.h"
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <flipper_application/flipper_application.h>
#include <xtreme.h>

static bool keybinds_fap_selector_item_callback(
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
    desktop_settings_scene_keybinds_action_type_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager, DesktopSettingsAppSceneKeybindsActionType, index);
    char* keybind = desktop_settings_app_get_keybind(app);

    switch(index) {
    case DesktopSettingsAppKeybindActionTypeMainApp:
    case DesktopSettingsAppKeybindActionTypeMoreActions:
        scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsAction);
        break;
    case DesktopSettingsAppKeybindActionTypeExternalApp:
    case DesktopSettingsAppKeybindActionTypeOpenFile: {
        const char* base_path;
        const char* extension;
        bool hide_ext;
        if(index == DesktopSettingsAppKeybindActionTypeExternalApp) {
            base_path = EXT_PATH("apps");
            extension = ".fap";
            hide_ext = true;
        } else {
            base_path = STORAGE_EXT_PATH_PREFIX;
            extension = "*";
            hide_ext = false;
        }
        const DialogsFileBrowserOptions browser_options = {
            .extension = extension,
            .icon = &I_unknown_10px,
            .hide_dot_files = !xtreme_settings.show_hidden_files,
            .skip_assets = true,
            .hide_ext = hide_ext,
            .item_loader_callback = keybinds_fap_selector_item_callback,
            .item_loader_context = app,
            .base_path = base_path,
        };
        FuriString* temp_path = furi_string_alloc_set_str(base_path);
        if(storage_file_exists(furi_record_open(RECORD_STORAGE), keybind)) {
            furi_string_set_str(temp_path, keybind);
        }
        furi_record_close(RECORD_STORAGE);
        if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
            strncpy(keybind, furi_string_get_cstr(temp_path), MAX_KEYBIND_LENGTH);
            DESKTOP_KEYBINDS_SAVE(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, DesktopSettingsAppSceneStart);
        }
        furi_string_free(temp_path);
        break;
    }
    case DesktopSettingsAppKeybindActionTypeRemoveKeybind:
        strncpy(keybind, "", MAX_KEYBIND_LENGTH);
        DESKTOP_KEYBINDS_SAVE(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, DesktopSettingsAppSceneStart);
        break;
    default:
        break;
    }
}

void desktop_settings_scene_keybinds_action_type_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    char* keybind = desktop_settings_app_get_keybind(app);

    submenu_add_item(
        submenu,
        "Main App",
        DesktopSettingsAppKeybindActionTypeMainApp,
        desktop_settings_scene_keybinds_action_type_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "External App",
        DesktopSettingsAppKeybindActionTypeExternalApp,
        desktop_settings_scene_keybinds_action_type_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "Open File",
        DesktopSettingsAppKeybindActionTypeOpenFile,
        desktop_settings_scene_keybinds_action_type_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "More Actions",
        DesktopSettingsAppKeybindActionTypeMoreActions,
        desktop_settings_scene_keybinds_action_type_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "Remove Keybind",
        DesktopSettingsAppKeybindActionTypeRemoveKeybind,
        desktop_settings_scene_keybinds_action_type_submenu_callback,
        app);

    DesktopSettingsAppKeybindActionType selected = scene_manager_get_scene_state(
        app->scene_manager, DesktopSettingsAppSceneKeybindsActionType);
    if(selected == DesktopSettingsAppKeybindActionTypeRemoveKeybind) {
        for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
            if(!strncmp(FLIPPER_APPS[i].name, keybind, MAX_KEYBIND_LENGTH)) {
                selected = DesktopSettingsAppKeybindActionTypeMainApp;
            }
        }
        for(size_t i = 0; i < FLIPPER_EXTERNAL_APPS_COUNT; i++) {
            if(!strncmp(FLIPPER_EXTERNAL_APPS[i].name, keybind, MAX_KEYBIND_LENGTH)) {
                selected = DesktopSettingsAppKeybindActionTypeMainApp;
            }
        }

        if(storage_file_exists(furi_record_open(RECORD_STORAGE), keybind)) {
            FuriString* tmp = furi_string_alloc_set(keybind);
            if(furi_string_end_with_str(tmp, ".fap")) {
                selected = DesktopSettingsAppKeybindActionTypeExternalApp;
            } else {
                selected = DesktopSettingsAppKeybindActionTypeOpenFile;
            }
            furi_string_free(tmp);
        }
        furi_record_close(RECORD_STORAGE);

        for(size_t i = 0; i < EXTRA_KEYBINDS_COUNT; i++) {
            if(!strncmp(EXTRA_KEYBINDS[i], keybind, MAX_KEYBIND_LENGTH)) {
                selected = DesktopSettingsAppKeybindActionTypeMoreActions;
            }
        }

        if(!strnlen(keybind, MAX_KEYBIND_LENGTH)) {
            selected = DesktopSettingsAppKeybindActionTypeRemoveKeybind;
        }
    }

    submenu_set_header(submenu, "Keybind action:");
    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_keybinds_action_type_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void desktop_settings_scene_keybinds_action_type_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_reset(app->submenu);
}
