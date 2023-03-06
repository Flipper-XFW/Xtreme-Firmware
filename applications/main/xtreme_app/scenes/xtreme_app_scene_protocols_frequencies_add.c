#include "../xtreme_app.h"

enum TextInputIndex {
    TextInputIndexResult,
};

static void xtreme_app_scene_protocols_frequencies_add_text_input_callback(void* context) {
    XtremeApp* app = context;

    do {
        char *end;
        uint32_t value = strtol(app->subghz_freq_buffer, &end, 0);
        if(*end) break;
        value *= 10000;
        if(!furi_hal_subghz_is_frequency_valid(value)) break;
        bool is_hopper = scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneProtocolsFrequenciesAdd);
        if(is_hopper) {
            FrequencyList_push_back(app->subghz_hopper_frequencies, value);
        } else {
            FrequencyList_push_back(app->subghz_static_frequencies, value);
        }
        app->save_subghz_frequencies = true;
    } while(false);
    view_dispatcher_send_custom_event(app->view_dispatcher, TextInputIndexResult);
}

void xtreme_app_scene_protocols_frequencies_add_on_enter(void* context) {
    XtremeApp* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Format: 12356");

    strlcpy(app->subghz_freq_buffer, "", XTREME_SUBGHZ_FREQ_BUFFER_SIZE);

    text_input_set_result_callback(
        text_input,
        xtreme_app_scene_protocols_frequencies_add_text_input_callback,
        app,
        app->subghz_freq_buffer,
        XTREME_SUBGHZ_FREQ_BUFFER_SIZE,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewTextInput);
}

bool xtreme_app_scene_protocols_frequencies_add_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case TextInputIndexResult:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_protocols_frequencies_add_on_exit(void* context) {
    XtremeApp* app = context;
    text_input_reset(app->text_input);
}
