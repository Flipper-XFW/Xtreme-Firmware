#include "../desktop_settings_app.h"
#include "applications.h"
#include "desktop_settings_scene.h"
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <flipper_application/flipper_application.h>

static const char* EXTRA_OPTIONS[] = {
    "Apps",
    "Archive",
    "Device Info",
    "Lock Menu",
    "Lock Keypad",
    "Lock with PIN",
    "Passport",
};
#define EXTRA_OPTIONS_COUNT COUNT_OF(EXTRA_OPTIONS)

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

static void desktop_settings_scene_keybinds_choose_main_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    KeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    KeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    char* keybind = app->desktop->keybinds[type][key].data;

    strncpy(keybind, FLIPPER_APPS[index].name, MAX_KEYBIND_LENGTH);
    DESKTOP_KEYBINDS_SAVE(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
    scene_manager_previous_scene(app->scene_manager);
    scene_manager_previous_scene(app->scene_manager);
    scene_manager_previous_scene(app->scene_manager);
}

static void desktop_settings_scene_keybinds_choose_ext_callback(void* context, uint32_t index) {
    UNUSED(index);
    DesktopSettingsApp* app = context;
    KeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    KeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    char* keybind = app->desktop->keybinds[type][key].data;

    const DialogsFileBrowserOptions browser_options = {
        .extension = ".fap",
        .icon = &I_unknown_10px,
        .skip_assets = true,
        .hide_ext = true,
        .item_loader_callback = keybinds_fap_selector_item_callback,
        .item_loader_context = app,
        .base_path = EXT_PATH("apps"),
    };

    // Select keybind fap in file browser
    FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));
    if(storage_file_exists(furi_record_open(RECORD_STORAGE), keybind)) {
        furi_string_set_str(temp_path, keybind);
    }
    furi_record_close(RECORD_STORAGE);

    if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
        submenu_reset(app->submenu); // Prevent menu from being shown when we exiting scene
        strncpy(keybind, furi_string_get_cstr(temp_path), MAX_KEYBIND_LENGTH);
        DESKTOP_KEYBINDS_SAVE(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
        scene_manager_previous_scene(app->scene_manager);
        scene_manager_previous_scene(app->scene_manager);
        scene_manager_previous_scene(app->scene_manager);
    }
    furi_string_free(temp_path);
}

static void desktop_settings_scene_keybinds_choose_extra_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    KeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    KeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    char* keybind = app->desktop->keybinds[type][key].data;

    strncpy(keybind, EXTRA_OPTIONS[index - FLIPPER_APPS_COUNT], MAX_KEYBIND_LENGTH);
    DESKTOP_KEYBINDS_SAVE(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
    scene_manager_previous_scene(app->scene_manager);
    scene_manager_previous_scene(app->scene_manager);
    scene_manager_previous_scene(app->scene_manager);
}

void desktop_settings_scene_keybinds_choose_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    KeybindType type =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
    KeybindKey key =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneKeybindsKey);
    uint32_t pre_select_item = 0;
    char* keybind = app->desktop->keybinds[type][key].data;
    size_t submenu_i = -1;

    for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS[i].name,
            ++submenu_i,
            desktop_settings_scene_keybinds_choose_main_callback,
            app);

        // Select keybind item in submenu
        FURI_LOG_I("URMOM", "%s %s", keybind, FLIPPER_APPS[i].name);
        if(!strncmp(FLIPPER_APPS[i].name, keybind, MAX_KEYBIND_LENGTH)) {
            pre_select_item = submenu_i;
        }
    }

    submenu_add_item(
        submenu,
        "[External Application]",
        ++submenu_i,
        desktop_settings_scene_keybinds_choose_ext_callback,
        app);
    if(storage_file_exists(furi_record_open(RECORD_STORAGE), keybind)) {
        pre_select_item = submenu_i;
    }
    furi_record_close(RECORD_STORAGE);

    for(size_t i = 0; i < EXTRA_OPTIONS_COUNT; i++) {
        submenu_add_item(
            submenu,
            EXTRA_OPTIONS[i],
            ++submenu_i,
            desktop_settings_scene_keybinds_choose_extra_callback,
            app);

        // Select keybind item in submenu
        if(!strncmp(EXTRA_OPTIONS[i], keybind, MAX_KEYBIND_LENGTH)) {
            pre_select_item = submenu_i;
        }
    }

    submenu_set_header(submenu, "Keybind action:");
    submenu_set_selected_item(submenu, pre_select_item); // If set during loop, visual glitch.

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_keybinds_choose_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void desktop_settings_scene_keybinds_choose_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_reset(app->submenu);
}
