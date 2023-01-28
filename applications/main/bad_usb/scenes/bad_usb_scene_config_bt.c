#include "../bad_usb_app_i.h"
#include "furi_hal_power.h"
#include "furi_hal_usb.h"

enum VarItemListIndex {
    VarItemListIndexConnection,
    VarItemListIndexKeyboardLayout,
    VarItemListIndexAdvertisementName,
    VarItemListIndexMacAddress,
};

void bad_usb_scene_config_bt_connection_callback(VariableItem* item) {
    BadUsbApp* bad_usb = variable_item_get_context(item);
    bad_usb->is_bluetooth = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, bad_usb->is_bluetooth ? "BT" : "USB");
    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, VarItemListIndexConnection);
}

void bad_usb_scene_config_bt_var_item_list_callback(void* context, uint32_t index) {
    BadUsbApp* bad_usb = context;
    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, index);
}

void bad_usb_scene_config_bt_on_enter(void* context) {
    BadUsbApp* bad_usb = context;
    VariableItemList* var_item_list = bad_usb->var_item_list_bt;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Connection", 2, bad_usb_scene_config_bt_connection_callback, bad_usb);
    variable_item_set_current_value_index(item, bad_usb->is_bluetooth);
    variable_item_set_current_value_text(item, bad_usb->is_bluetooth ? "BT" : "USB");

    item = variable_item_list_add(
        var_item_list, "Keyboard layout", 0, NULL, bad_usb);

    item = variable_item_list_add(
        var_item_list, "Change adv name", 0, NULL, bad_usb);

    item = variable_item_list_add(
        var_item_list, "Change MAC address", 0, NULL, bad_usb);

    variable_item_list_set_enter_callback(var_item_list, bad_usb_scene_config_bt_var_item_list_callback, bad_usb);

    view_dispatcher_switch_to_view(bad_usb->view_dispatcher, BadUsbAppViewConfigBt);
}

bool bad_usb_scene_config_bt_on_event(void* context, SceneManagerEvent event) {
    BadUsbApp* bad_usb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfigBt, event.event);
        consumed = true;
        if(event.event == VarItemListIndexKeyboardLayout) {
            scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigLayout);
        } else if(event.event == VarItemListIndexConnection) {
            scene_manager_previous_scene(bad_usb->scene_manager);
            if (bad_usb->is_bluetooth) {
                scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigBt);
            } else {
                scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigUsb);
            }
        } else if(event.event == VarItemListIndexAdvertisementName) {
            scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigName);
        } else if(event.event == VarItemListIndexMacAddress) {
            scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigMac);
        // } else {
        //     furi_crash("Unknown key type");
        }
    }

    return consumed;
}

void bad_usb_scene_config_bt_on_exit(void* context) {
    BadUsbApp* bad_usb = context;
    VariableItemList* var_item_list = bad_usb->var_item_list_bt;

    variable_item_list_reset(var_item_list);
}
