#include "../bad_kb_app_i.h"

#define TAG "BadKbConfigMac"

static uint8_t* reverse_mac_addr(uint8_t* mac) {
    uint8_t tmp;
    for(int i = 0; i < 3; i++) {
        tmp = mac[i];
        mac[i] = mac[5 - i];
        mac[5 - i] = tmp;
    }
    return mac;
}

void bad_kb_scene_config_mac_byte_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventByteInputDone);
}

void bad_kb_scene_config_mac_on_enter(void* context) {
    BadKbApp* bad_kb = context;

    // Setup view
    ByteInput* byte_input = bad_kb->byte_input;
    byte_input_set_header_text(byte_input, "Enter new MAC address");
    byte_input_set_result_callback(
        byte_input,
        bad_kb_scene_config_mac_byte_input_callback,
        NULL,
        bad_kb,
        reverse_mac_addr(bad_kb->mac),
        GAP_MAC_ADDR_SIZE);
    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewConfigMac);
}

bool bad_kb_scene_config_mac_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BadKbAppCustomEventByteInputDone) {
            bt_set_profile_mac_address(bad_kb->bt, reverse_mac_addr(bad_kb->mac));
            scene_manager_previous_scene(bad_kb->scene_manager);
            consumed = true;
        }
    }
    return consumed;
}

void bad_kb_scene_config_mac_on_exit(void* context) {
    BadKbApp* bad_kb = context;

    // Clear view
    byte_input_set_result_callback(bad_kb->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(bad_kb->byte_input, "");
}
