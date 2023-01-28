#include "../bad_usb_app_i.h"

#define TAG "BadUsbConfigMac"

static uint8_t* reverse_mac_addr(uint8_t* mac) {
    uint8_t tmp;
    for(int i = 0; i < 3; i++) {
        tmp = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmp;
    }
    return mac;
}

void bad_usb_scene_config_mac_byte_input_callback(void* context) {
    BadUsbApp* bad_usb = context;

    view_dispatcher_send_custom_event(bad_usb->view_dispatcher, BadUsbAppCustomEventByteInputDone);
}

void bad_usb_scene_config_mac_on_enter(void* context) {
    BadUsbApp* bad_usb = context;

    // Setup view
    ByteInput* byte_input = bad_usb->byte_input;
    byte_input_set_header_text(byte_input, "Enter new MAC address");
    byte_input_set_result_callback(
        byte_input,
        bad_usb_scene_config_mac_byte_input_callback,
        NULL,
        bad_usb,
        reverse_mac_addr(bad_usb->mac),
        GAP_MAC_ADDR_SIZE);
    view_dispatcher_switch_to_view(bad_usb->view_dispatcher, BadUsbAppViewConfigMac);
}

bool bad_usb_scene_config_mac_on_event(void* context, SceneManagerEvent event) {
    BadUsbApp* bad_usb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BadUsbAppCustomEventByteInputDone) {
            bt_set_profile_mac_address(bad_usb->bt, reverse_mac_addr(bad_usb->mac));
            scene_manager_previous_scene(bad_usb->scene_manager);
            consumed = true;
        }
    }
    return consumed;
}

void bad_usb_scene_config_mac_on_exit(void* context) {
    BadUsbApp* bad_usb = context;

    // Clear view
    byte_input_set_result_callback(bad_usb->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(bad_usb->byte_input, "");
}
