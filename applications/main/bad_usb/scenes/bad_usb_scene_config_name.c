#include "../bad_usb_app_i.h"

static void bad_usb_scene_config_name_text_input_callback(void* context) {
    BadUsbApp* bad_usb = context;

    view_dispatcher_send_custom_event(
        bad_usb->view_dispatcher, BadUsbAppCustomEventTextEditResult);
}

void bad_usb_scene_config_name_on_enter(void* context) {
    BadUsbApp* bad_usb = context;
    TextInput* text_input = bad_usb->text_input;

    text_input_set_header_text(text_input, "Set BLE adv name");

    text_input_set_result_callback(
        text_input,
        bad_usb_scene_config_name_text_input_callback,
        bad_usb,
        bad_usb->name,
        BAD_USB_ADV_NAME_MAX_LEN,
        true);

    view_dispatcher_switch_to_view(bad_usb->view_dispatcher, BadUsbAppViewConfigName);
}

bool bad_usb_scene_config_name_on_event(void* context, SceneManagerEvent event) {
    BadUsbApp* bad_usb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadUsbAppCustomEventTextEditResult) {
            bt_set_profile_adv_name(bad_usb->bt, bad_usb->name);
        }
        scene_manager_previous_scene(bad_usb->scene_manager);
    }
    return consumed;
}

void bad_usb_scene_config_name_on_exit(void* context) {
    BadUsbApp* bad_usb = context;
    TextInput* text_input = bad_usb->text_input;

    text_input_reset(text_input);
}
