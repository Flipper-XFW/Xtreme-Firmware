#include "../bad_kb_app.h"

static void bad_kb_scene_config_name_text_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventTextEditResult);
}

void bad_kb_scene_config_name_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    text_input_set_header_text(text_input, "Set BT device name");

    text_input_set_result_callback(
        text_input,
        bad_kb_scene_config_name_text_input_callback,
        bad_kb,
        bad_kb->config.bt_name,
        BAD_KB_ADV_NAME_MAX_LEN,
        true);

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewConfigName);
}

bool bad_kb_scene_config_name_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadKbAppCustomEventTextEditResult) {
            bt_set_profile_adv_name(bad_kb->bt, bad_kb->config.bt_name);
        }
        scene_manager_previous_scene(bad_kb->scene_manager);
    }
    return consumed;
}

void bad_kb_scene_config_name_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    text_input_reset(text_input);
}
