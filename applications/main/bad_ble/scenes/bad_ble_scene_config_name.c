#include "../bad_ble_app_i.h"

static void bad_ble_scene_config_name_text_input_callback(void* context) {
    BadBleApp* bad_ble = context;

    view_dispatcher_send_custom_event(
        bad_ble->view_dispatcher, BadBleAppCustomEventTextEditResult);
}

void bad_ble_scene_config_name_on_enter(void* context) {
    BadBleApp* bad_ble = context;
    TextInput* text_input = bad_ble->text_input;

    text_input_set_header_text(text_input, "Set BLE adv name");

    text_input_set_result_callback(
        text_input,
        bad_ble_scene_config_name_text_input_callback,
        bad_ble,
        bad_ble->name,
        BAD_BLE_ADV_NAME_MAX_LEN,
        true);

    view_dispatcher_switch_to_view(bad_ble->view_dispatcher, BadBleAppViewConfigName);
}

bool bad_ble_scene_config_name_on_event(void* context, SceneManagerEvent event) {
    BadBleApp* bad_ble = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadBleAppCustomEventTextEditResult) {
            bt_set_profile_adv_name(bad_ble->bt, bad_ble->name);
        }
        scene_manager_previous_scene(
            bad_ble->scene_manager);
    }
    return consumed;
}

void bad_ble_scene_config_name_on_exit(void* context) {
    BadBleApp* bad_ble = context;
    TextInput* text_input = bad_ble->text_input;

    text_input_reset(text_input);
}
