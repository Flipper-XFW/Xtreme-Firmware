#include "../bad_usb_app_i.h"
#include "furi_hal_power.h"
#include "furi_hal_usb.h"

enum VarItemListIndex {
    VarItemListIndexConnection,
    VarItemListIndexKeyboardLayout,
};

void bad_usb_scene_config_usb_connection_callback(VariableItem* item) {
    BadUsbApp* bad_usb = variable_item_get_context(item);
    bad_usb->is_bt = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, bad_usb->is_bt ? "BT" : "USB");
    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, VarItemListIndexConnection);
}

void bad_usb_scene_config_usb_var_item_list_callback(void* context, uint32_t index) {
    BadUsbApp* bad_usb = context;
    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, index);
}

void bad_usb_scene_config_usb_on_enter(void* context) {
    BadUsbApp* bad_usb = context;
    VariableItemList* var_item_list = bad_usb->var_item_list_usb;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Connection", 2, bad_usb_scene_config_usb_connection_callback, bad_usb);
    variable_item_set_current_value_index(item, bad_usb->is_bt);
    variable_item_set_current_value_text(item, bad_usb->is_bt ? "BT" : "USB");

    item = variable_item_list_add(
        var_item_list, "Keyboard layout", 0, NULL, bad_usb);

    variable_item_list_set_enter_callback(var_item_list, bad_usb_scene_config_usb_var_item_list_callback, bad_usb);

    view_dispatcher_switch_to_view(bad_usb->view_dispatcher, BadUsbAppViewConfigUsb);
}

bool bad_usb_scene_config_usb_on_event(void* context, SceneManagerEvent event) {
    BadUsbApp* bad_usb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfigUsb, event.event);
        consumed = true;
        if(event.event == VarItemListIndexKeyboardLayout) {
            scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigLayout);
        } else if(event.event == VarItemListIndexConnection) {
            bad_usb_script_close(bad_usb->bad_usb_script);
            bad_usb->bad_usb_script = bad_usb_script_open(bad_usb->file_path, bad_usb->is_bt ? bad_usb->bt : NULL);
            bad_usb_script_set_keyboard_layout(bad_usb->bad_usb_script, bad_usb->keyboard_layout);
            scene_manager_previous_scene(bad_usb->scene_manager);
            if (bad_usb->is_bt) {
                scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigBt);
            } else {
                scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigUsb);
            }
        // } else {
        //     furi_crash("Unknown key type");
        }
    }

    return consumed;
}

void bad_usb_scene_config_usb_on_exit(void* context) {
    BadUsbApp* bad_usb = context;
    VariableItemList* var_item_list = bad_usb->var_item_list_usb;

    variable_item_list_reset(var_item_list);
}
