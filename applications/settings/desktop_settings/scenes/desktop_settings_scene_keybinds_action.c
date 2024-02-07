#include "../desktop_settings_app.h"
#include "applications.h"
#include "desktop_settings_scene.h"

static void
    desktop_settings_scene_keybinds_action_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;

    if(desktop_settings_app_set_keybind(app, (const char*)index)) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, DesktopSettingsAppSceneStart);
    }
}

void desktop_settings_scene_keybinds_action_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    const char* keybind = desktop_settings_app_get_keybind(app);
    submenu_reset(submenu);

    uint32_t pre_select_item = 0;
    DesktopSettingsAppKeybindActionType action_type = scene_manager_get_scene_state(
        app->scene_manager, DesktopSettingsAppSceneKeybindsActionType);

    if(action_type == DesktopSettingsAppKeybindActionTypeMainApp) {
        for(size_t i = 0; i < FLIPPER_APPS_COUNT; i++) {
            submenu_add_item(
                submenu,
                FLIPPER_APPS[i].name,
                (uint32_t)FLIPPER_APPS[i].name,
                desktop_settings_scene_keybinds_action_submenu_callback,
                app);

            // Select keybind item in submenu
            if(!strncmp(FLIPPER_APPS[i].name, keybind, MAX_KEYBIND_LENGTH)) {
                pre_select_item = (uint32_t)FLIPPER_APPS[i].name;
            }
        }
        for(size_t i = 0; i < FLIPPER_EXTERNAL_APPS_COUNT; i++) {
            submenu_add_item(
                submenu,
                FLIPPER_EXTERNAL_APPS[i].name,
                (uint32_t)FLIPPER_EXTERNAL_APPS[i].name,
                desktop_settings_scene_keybinds_action_submenu_callback,
                app);

            // Select keybind item in submenu
            if(!strncmp(FLIPPER_EXTERNAL_APPS[i].name, keybind, MAX_KEYBIND_LENGTH)) {
                pre_select_item = (uint32_t)FLIPPER_EXTERNAL_APPS[i].name;
            }
        }
    } else if(action_type == DesktopSettingsAppKeybindActionTypeMoreActions) {
        for(size_t i = 0; i < EXTRA_KEYBINDS_COUNT; i++) {
            submenu_add_item(
                submenu,
                EXTRA_KEYBINDS[i],
                (uint32_t)EXTRA_KEYBINDS[i],
                desktop_settings_scene_keybinds_action_submenu_callback,
                app);

            // Select keybind item in submenu
            if(!strncmp(EXTRA_KEYBINDS[i], keybind, MAX_KEYBIND_LENGTH)) {
                pre_select_item = (uint32_t)EXTRA_KEYBINDS[i];
            }
        }
    }

    submenu_set_selected_item(submenu, pre_select_item); // If set during loop, visual glitch.

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_keybinds_action_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void desktop_settings_scene_keybinds_action_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    submenu_reset(app->submenu);
}
