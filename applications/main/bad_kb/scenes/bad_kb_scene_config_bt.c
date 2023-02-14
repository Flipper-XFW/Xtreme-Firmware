#include "../bad_kb_app_i.h"
#include "furi_hal_power.h"
#include "furi_hal_usb.h"
#include <xtreme/settings.h>

enum VarItemListIndex {
    VarItemListIndexConnection,
    VarItemListIndexKeyboardLayout,
    VarItemListIndexAdvertisementName,
    VarItemListIndexMacAddress,
};

void bad_kb_scene_config_bt_connection_callback(VariableItem* item) {
    BadKbApp* bad_kb = variable_item_get_context(item);
    bad_kb->is_bt = variable_item_get_current_value_index(item);
    XTREME_SETTINGS()->bad_bt = bad_kb->is_bt;
    XTREME_SETTINGS_SAVE();
    variable_item_set_current_value_text(item, bad_kb->is_bt ? "BT" : "USB");
    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, VarItemListIndexConnection);
}

void bad_kb_scene_config_bt_var_item_list_callback(void* context, uint32_t index) {
    BadKbApp* bad_kb = context;
    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, index);
}

void bad_kb_scene_config_bt_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    VariableItemList* var_item_list = bad_kb->var_item_list_bt;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Connection", 2, bad_kb_scene_config_bt_connection_callback, bad_kb);
    variable_item_set_current_value_index(item, bad_kb->is_bt);
    variable_item_set_current_value_text(item, bad_kb->is_bt ? "BT" : "USB");

    item = variable_item_list_add(var_item_list, "Keyboard layout", 0, NULL, bad_kb);

    item = variable_item_list_add(var_item_list, "BT device name", 0, NULL, bad_kb);

    item = variable_item_list_add(var_item_list, "BT MAC address", 0, NULL, bad_kb);

    variable_item_list_set_enter_callback(
        var_item_list, bad_kb_scene_config_bt_var_item_list_callback, bad_kb);

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewConfigBt);
}

bool bad_kb_scene_config_bt_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(bad_kb->scene_manager, BadKbSceneConfigBt, event.event);
        consumed = true;
        if(event.event == VarItemListIndexKeyboardLayout) {
            scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneConfigLayout);
        } else if(event.event == VarItemListIndexConnection) {
            bad_kb_script_close(bad_kb->bad_kb_script);
            bad_kb_connection_deinit(bad_kb->bt);
            bad_kb->bad_kb_script =
                bad_kb_script_open(bad_kb->file_path, bad_kb->is_bt ? bad_kb->bt : NULL);
            bad_kb_script_set_keyboard_layout(bad_kb->bad_kb_script, bad_kb->keyboard_layout);
            scene_manager_previous_scene(bad_kb->scene_manager);
            if(bad_kb->is_bt) {
                scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneConfigBt);
            } else {
                scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneConfigUsb);
            }
        } else if(event.event == VarItemListIndexAdvertisementName) {
            scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneConfigName);
        } else if(event.event == VarItemListIndexMacAddress) {
            scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneConfigMac);
            // } else {
            //     furi_crash("Unknown key type");
        }
    }

    return consumed;
}

void bad_kb_scene_config_bt_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    VariableItemList* var_item_list = bad_kb->var_item_list_bt;

    variable_item_list_reset(var_item_list);
}
