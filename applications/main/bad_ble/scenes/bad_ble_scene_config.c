#include "../bad_ble_app_i.h"
#include "furi_hal_power.h"

enum SubmenuIndex {
    SubmenuIndexKeyboardLayout,
    SubmenuIndexAdvertisementName,
    SubmenuIndexMacAddress,
};

void bad_ble_scene_config_submenu_callback(void* context, uint32_t index) {
    BadBleApp* bad_ble = context;
    view_dispatcher_send_custom_event(bad_ble->view_dispatcher, index);
}

void bad_ble_scene_config_on_enter(void* context) {
    BadBleApp* bad_ble = context;
    Submenu* submenu = bad_ble->submenu;

    submenu_add_item(
        submenu,
        "Keyboard layout",
        SubmenuIndexKeyboardLayout,
        bad_ble_scene_config_submenu_callback,
        bad_ble);

    submenu_add_item(
        submenu,
        "Change adv name",
        SubmenuIndexAdvertisementName,
        bad_ble_scene_config_submenu_callback,
        bad_ble);

    submenu_add_item(
        submenu,
        "Change MAC address",
        SubmenuIndexMacAddress,
        bad_ble_scene_config_submenu_callback,
        bad_ble);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(bad_ble->scene_manager, BadBleSceneConfig));

    view_dispatcher_switch_to_view(bad_ble->view_dispatcher, BadBleAppViewConfig);
}

bool bad_ble_scene_config_on_event(void* context, SceneManagerEvent event) {
    BadBleApp* bad_ble = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(bad_ble->scene_manager, BadBleSceneConfig, event.event);
        consumed = true;
        if(event.event == SubmenuIndexKeyboardLayout) {
            scene_manager_next_scene(bad_ble->scene_manager, BadBleSceneConfigLayout);
        } else if (event.event == SubmenuIndexAdvertisementName) {
            scene_manager_next_scene(bad_ble->scene_manager, BadBleSceneConfigName);
        } else if (event.event == SubmenuIndexMacAddress) {
            scene_manager_next_scene(bad_ble->scene_manager, BadBleSceneConfigMac);
        } else {
            furi_crash("Unknown key type");
        }
    }

    return consumed;
}

void bad_ble_scene_config_on_exit(void* context) {
    BadBleApp* bad_ble = context;
    Submenu* submenu = bad_ble->submenu;

    submenu_reset(submenu);
}
