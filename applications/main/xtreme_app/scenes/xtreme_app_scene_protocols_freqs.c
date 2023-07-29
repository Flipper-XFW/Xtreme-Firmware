#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexUseDefaults,
    VarItemListIndexStaticFreqs,
    VarItemListIndexHopperFreqs,
};

void xtreme_app_scene_protocols_freqs_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_protocols_freqs_use_defaults_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->subghz_use_defaults = value;
    app->save_subghz_freqs = true;
}

void xtreme_app_scene_protocols_freqs_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Use Defaults",
        2,
        xtreme_app_scene_protocols_freqs_use_defaults_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_use_defaults);
    variable_item_set_current_value_text(item, app->subghz_use_defaults ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "Static Freqs", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Hopper Freqs", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_protocols_freqs_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneProtocolsFreqs));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_protocols_freqs_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneProtocolsFreqs, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexStaticFreqs:
            scene_manager_set_scene_state(
                app->scene_manager, XtremeAppSceneProtocolsFreqsStatic, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocolsFreqsStatic);
            break;
        case VarItemListIndexHopperFreqs:
            scene_manager_set_scene_state(
                app->scene_manager, XtremeAppSceneProtocolsFreqsHopper, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocolsFreqsHopper);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_protocols_freqs_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
