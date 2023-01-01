#include "../bad_usb_app_i.h"
#include "furi_hal_power.h"
#include "furi_hal_usb.h"
#include "../bad_usb_script.h"

// enum SubmenuIndex {
//     SubmenuIndexKeyboardLayout,
//     SubmenuIndexInjectionMode,
//     SubmenuIndexBleConfig
// };

enum VariableListIndex {
    VariableListIndexKeyboardLayout,
    VariableListIndexInjectionMode,
    VariableListIndexBleConfig
};

typedef struct BadUsbConfigItem {
    const char* name;
    uint8_t num_options_menu;
    const char* options_menu[NUM_CONF_OPT];
} BadUsbConfigItem;

const BadUsbCustomEvent bad_usb_custom_events_mode[NUM_CONF_OPT] = {
    BadUsbCustomEventModeUSB,
    BadUsbCustomEventModeBLE,
    BadUsbCustomEventModeUART
};

#define NUM_BAD_USB_CONFIG_ITEMS 3
const BadUsbConfigItem bad_usb_config_items[NUM_BAD_USB_CONFIG_ITEMS] = {
    {"Keyboard layout", 1, {""}},
    {"Injection mode", 3, {"USB", "BLE", "UART"}},
    {"BLE Config", 1, {""}},
};

// void bad_usb_scene_config_submenu_callback(void* context, uint32_t index) {
//     BadUsbApp* bad_usb = context;
//     view_dispatcher_send_custom_event(bad_usb->view_dispatcher, index);
// }

void bad_usb_scene_config_var_list_callback(void* context, uint32_t index) {
    BadUsbApp* bad_usb = context;
    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, index);
}

void bad_usb_scene_config_var_list_enter_callback(void* context, uint32_t index) {
    BadUsbApp* bad_usb = context;

    F_DEBUG(bad_usb, "[+] bad_usb_scene_config_var_list_enter_callback\r\n");

    if (index == VariableListIndexKeyboardLayout) {
        view_dispatcher_send_custom_event(bad_usb->view_dispatcher, BadUsbCustomEventKeyboardLayout);
    } else if (index == VariableListIndexInjectionMode) {
        int menu_opt_idx = bad_usb->menu_opt_idx[index]; 
        view_dispatcher_send_custom_event(bad_usb->view_dispatcher, bad_usb_custom_events_mode[menu_opt_idx]);
    } else if (index == VariableListIndexBleConfig) {
        view_dispatcher_send_custom_event(bad_usb->view_dispatcher, BadUsbCustomEventConfigBle);
    }

    F_DEBUG(bad_usb, "[-] bad_usb_scene_config_var_list_enter_callback\r\n");
}

void bad_usb_scene_config_var_list_change_callback(VariableItem *item) {
    furi_assert(item);
    BadUsbApp* bad_usb = variable_item_get_context(item);
    furi_assert(bad_usb);

    F_DEBUG(bad_usb, "[+] bad_usb_scene_config_var_list_change_callback\r\n");
    BadUsbConfigItem *menu_item = &bad_usb_config_items[bad_usb->menu_idx];
    uint8_t item_index = variable_item_get_current_value_index(item); 
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    bad_usb->menu_opt_idx[bad_usb->menu_idx] = item_index;
    F_DEBUG(bad_usb, "[-] bad_usb_scene_config_var_list_change_callback\r\n");
}

void bad_usb_scene_config_on_enter(void* context) {
    BadUsbApp* bad_usb = context;
    //Submenu* submenu = bad_usb->submenu;
    char toggle_label[32];
    VariableItemList *item_list = bad_usb->variable_item_list;
    VariableItem *item = NULL;

    char debug_buf[256];

    F_DEBUG(bad_usb, "[+] BadUsbSceneConfig on_enter\r\n");

    F_DEBUG(bad_usb, "Setting up variable item list\r\n");
    variable_item_list_set_enter_callback(
        item_list, bad_usb_scene_config_var_list_enter_callback, bad_usb);

    for (int i=0; i<NUM_BAD_USB_CONFIG_ITEMS; i++) {
        item = variable_item_list_add(
            item_list,
            bad_usb_config_items[i].name,
            bad_usb_config_items[i].num_options_menu,
            bad_usb_scene_config_var_list_change_callback,
            bad_usb
        );
        variable_item_set_current_value_index(item, bad_usb->menu_opt_idx[i]);
        variable_item_set_current_value_text(
            item, bad_usb_config_items[i].options_menu[bad_usb->menu_opt_idx[i]]);

        snprintf(debug_buf, sizeof(debug_buf), "Added item %s, index %d, opt_idx %d\r\n",
            bad_usb_config_items[i].name, i, bad_usb->menu_opt_idx[i]);
        F_DEBUG(bad_usb, debug_buf);
    }

    variable_item_list_set_selected_item(
        item_list, scene_manager_get_scene_state(bad_usb->scene_manager, BadUsbSceneConfig));


    // submenu_add_item(
    //     submenu,        "Keyboard layout",
    //     SubmenuIndexKeyboardLayout,
    //     bad_usb_scene_config_submenu_callback,
    //     bad_usb);

    // furi_assert(bad_usb->bad_usb_script);
    // // switch for choising Bad-USB or Bad-BLE
    // BadUsbScriptMode hid_mode = bad_usb_script_get_hid_mode(bad_usb->bad_usb_script);
    // furi_assert(hid_mode < BadUsbScriptModeNb);
    // snprintf(toggle_label, sizeof(toggle_label), "Toggle Mode: %s", ducky_mode_names[hid_mode]);
    // submenu_add_item(
    //     submenu,
    //     toggle_label,
    //     SubmenuIndexInjectionMode,
    //     bad_usb_scene_config_submenu_callback,
    //     bad_usb);

    // submenu_add_item(
    //     submenu,        "BLE Config",
    //     SubmenuIndexBleConfig,
    //     bad_usb_scene_config_submenu_callback,
    //     bad_usb);

    // submenu_set_selected_item(
    //     submenu, scene_manager_get_scene_state(bad_usb->scene_manager, BadUsbSceneConfig));

    view_dispatcher_switch_to_view(bad_usb->view_dispatcher, BadUsbAppViewConfig);
    
    F_DEBUG(bad_usb, "[-] BadUsbSceneConfig on_enter\r\n");
}

bool bad_usb_scene_config_on_event(void* context, SceneManagerEvent event) {
    // BadUsbApp* bad_usb = context;
    // bool consumed = false;

    // if(event.type == SceneManagerEventTypeCustom) {
    //     scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfig, event.event);
    //     consumed = true;
    //     if(event.event == SubmenuIndexKeyboardLayout) {
    //         scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigLayout);
    //     } else if (event.event == SubmenuIndexInjectionMode) {
    //         BadUsbScriptMode hid_mode = bad_usb_script_get_hid_mode(bad_usb->bad_usb_script);
    //         if (hid_mode == BadUsbScriptModeUSB) {
    //             bad_usb_script_set_hid_mode(bad_usb->bad_usb_script, BadUsbScriptModeBLE);
    //         } else {
    //             bad_usb_script_set_hid_mode(bad_usb->bad_usb_script, BadUsbScriptModeUSB);
    //         }
    //         scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfig);
    //     } else if (event.event == SubmenuIndexBleConfig) {
    //         //scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigBleHid);
    //     } else {
    //         furi_crash("Unknown key type");
    //     }
    // }

    // return consumed;
    furi_assert(context);
    BadUsbApp* bad_usb = context;
    bool consumed = false;
    char debug_buf[256];

    F_DEBUG(bad_usb, "[+] BadUsbSceneConfig on_event\r\n");
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadUsbCustomEventKeyboardLayout) {
            scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfig, bad_usb->menu_idx);
            scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigLayout);
        } else if(event.event == BadUsbCustomEventModeUSB) {
            F_DEBUG(bad_usb, "mode usb triggered !\r\n");
            scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfig, bad_usb->menu_idx);
            bad_usb->mode = BadUsbModeUSB;
        } else if (event.event == BadUsbCustomEventModeBLE) {
            F_DEBUG(bad_usb, "mode ble triggered !\r\n");
            scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfig, bad_usb->menu_idx);
            bad_usb->mode = BadUsbModeBLE;
        } else if (event.event == BadUsbCustomEventModeUART) {
            F_DEBUG(bad_usb, "mode uart triggered !\r\n");
            scene_manager_set_scene_state(bad_usb->scene_manager, BadUsbSceneConfig, bad_usb->menu_idx);
            bad_usb->mode = BadUsbModeUART;
        } else if (event.event == BadUsbCustomEventConfigBle) {
            //scene_manager_next_scene(bad_usb->scene_manager, BadUsbSceneConfigBleHid);
        } else {
            furi_crash("Unknown key type");
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        bad_usb->menu_idx = variable_item_list_get_selected_item_index(bad_usb->variable_item_list);
        snprintf(debug_buf, sizeof(debug_buf), "menu_idx: %d\r\n", bad_usb->menu_idx);
        F_DEBUG(bad_usb, debug_buf);
        consumed = true;
    }

    F_DEBUG(bad_usb, "[-] BadUsbSceneConfig on_event\r\n");
    return consumed;
}

void bad_usb_scene_config_on_exit(void* context) {
    BadUsbApp* bad_usb = context;
    //Submenu* submenu = bad_usb->submenu;

    //submenu_reset(submenu);

    VariableItemList *item_list = bad_usb->variable_item_list;
    variable_item_list_reset(item_list);

}
