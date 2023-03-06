#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexUseDefaults,
    VarItemListIndexStaticFrequency,
    VarItemListIndexDeleteStatic,
    VarItemListIndexHopperFrequency,
    VarItemListIndexDeleteHopper,
};

void xtreme_app_scene_protocols_frequencies_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_protocols_frequencies_use_defaults_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->subghz_use_defaults = value;
    app->save_subghz_frequencies = true;
}

static void xtreme_app_scene_protocols_frequencies_static_frequency_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_static_index = variable_item_get_current_value_index(item);
    uint32_t value = *FrequencyList_get(app->subghz_static_frequencies, app->subghz_static_index);
    char text[10] = {0};
    snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
    variable_item_set_current_value_text(item, text);
}

static void xtreme_app_scene_protocols_frequencies_hopper_frequency_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_hopper_index = variable_item_get_current_value_index(item);
    uint32_t value = *FrequencyList_get(app->subghz_hopper_frequencies, app->subghz_hopper_index);
    char text[10] = {0};
    snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
    variable_item_set_current_value_text(item, text);
}

void xtreme_app_scene_protocols_frequencies_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Use Defaults", 2, xtreme_app_scene_protocols_frequencies_use_defaults_changed, app);
    variable_item_set_current_value_index(item, app->subghz_use_defaults);
    variable_item_set_current_value_text(item, app->subghz_use_defaults ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "Static Freq", FrequencyList_size(app->subghz_static_frequencies), xtreme_app_scene_protocols_frequencies_static_frequency_changed, app);
    app->subghz_static_index = 0;
    variable_item_set_current_value_index(item, app->subghz_static_index);
    if(FrequencyList_size(app->subghz_static_frequencies)) {
        uint32_t value = *FrequencyList_get(app->subghz_static_frequencies, app->subghz_static_index);
        char text[10] = {0};
        snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
        variable_item_set_current_value_text(item, text);
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Delete Static Freq", 0, NULL, app);

    item = variable_item_list_add(var_item_list, "Hopper Freq", FrequencyList_size(app->subghz_hopper_frequencies), xtreme_app_scene_protocols_frequencies_hopper_frequency_changed, app);
    app->subghz_hopper_index = 0;
    variable_item_set_current_value_index(item, app->subghz_hopper_index);
    if(FrequencyList_size(app->subghz_hopper_frequencies)) {
        uint32_t value = *FrequencyList_get(app->subghz_hopper_frequencies, app->subghz_hopper_index);
        char text[10] = {0};
        snprintf(text, sizeof(text), "%lu.%02lu", value / 1000000, (value % 1000000) / 10000);
        variable_item_set_current_value_text(item, text);
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Delete Hopper Freq", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_protocols_frequencies_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneProtocolsFrequencies));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

void remove_frequency(XtremeApp* app, FrequencyList_t list, uint8_t index) {
    if(!FrequencyList_size(list)) return;
    uint32_t value = *FrequencyList_get(list, index);
    FrequencyList_it_t it;
    FrequencyList_it(it, list);
    while(!FrequencyList_end_p(it)) {
        if(*FrequencyList_ref(it) == value) {
            FrequencyList_remove(list, it);
        } else {
            FrequencyList_next(it);
        }
    }
    app->save_subghz_frequencies = true;
}

bool xtreme_app_scene_protocols_frequencies_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneProtocolsFrequencies, event.event);
        consumed = true;
        bool redraw = true;
        switch(event.event) {
        case VarItemListIndexDeleteStatic:
            remove_frequency(app, app->subghz_static_frequencies, app->subghz_static_index);
            break;
        case VarItemListIndexDeleteHopper:
            remove_frequency(app, app->subghz_hopper_frequencies, app->subghz_hopper_index);
            break;
        default:
            redraw = false;
            break;
        }
        if(redraw) {
            scene_manager_previous_scene(app->scene_manager);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocolsFrequencies);
        }
    }

    return consumed;
}

void xtreme_app_scene_protocols_frequencies_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
