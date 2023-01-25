#include "../bad_ble_app_i.h"

#define TAG "BadBleConfigMac"

void bad_ble_scene_config_mac_byte_input_callback(void* context) {
    BadBleApp* bad_ble = context;

    view_dispatcher_send_custom_event(bad_ble->view_dispatcher, BadBleAppCustomEventByteInputDone);
}

void bad_ble_scene_config_mac_on_enter(void* context) {
    BadBleApp* bad_ble = context;

    // Setup view
    ByteInput* byte_input = bad_ble->byte_input;
    byte_input_set_header_text(byte_input, "Enter new MAC address");
    byte_input_set_result_callback(
        byte_input,
        bad_ble_scene_config_mac_byte_input_callback,
        NULL,
        bad_ble,
        bad_ble->mac,
        GAP_MAC_ADDR_SIZE);
    view_dispatcher_switch_to_view(bad_ble->view_dispatcher, BadBleAppViewConfigMac);
}

bool bad_ble_scene_config_mac_on_event(void* context, SceneManagerEvent event) {
    BadBleApp* bad_ble = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BadBleAppCustomEventByteInputDone) {
            bt_set_profile_mac_address(bad_ble->bt, bad_ble->mac);
            scene_manager_previous_scene(bad_ble->scene_manager);
            consumed = true;
        }
    }
    return consumed;
}

void bad_ble_scene_config_mac_on_exit(void* context) {
    BadBleApp* bad_ble = context;

    // Clear view
    byte_input_set_result_callback(bad_ble->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(bad_ble->byte_input, "");
}
