#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexShowClock,
    VarItemListIndexShowDate,
};

void xtreme_app_scene_interface_lockscreen_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_interface_lockscreen_show_time_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->lockscreen_time = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_lockscreen_show_date_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->lockscreen_date = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_lockscreen_show_statusbar_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->lockscreen_statusbar = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_lockscreen_unlock_prompt_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->lockscreen_prompt = value;
    app->save_settings = true;
}

void xtreme_app_scene_interface_lockscreen_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Show Time",
        2,
        xtreme_app_scene_interface_lockscreen_show_time_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->lockscreen_time);
    variable_item_set_current_value_text(item, xtreme_settings->lockscreen_time ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Show Date",
        2,
        xtreme_app_scene_interface_lockscreen_show_date_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->lockscreen_date);
    variable_item_set_current_value_text(item, xtreme_settings->lockscreen_date ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Show Statusbar",
        2,
        xtreme_app_scene_interface_lockscreen_show_statusbar_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->lockscreen_statusbar);
    variable_item_set_current_value_text(
        item, xtreme_settings->lockscreen_statusbar ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Unlock Prompt",
        2,
        xtreme_app_scene_interface_lockscreen_unlock_prompt_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->lockscreen_prompt);
    variable_item_set_current_value_text(item, xtreme_settings->lockscreen_prompt ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_interface_lockscreen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneInterfaceLockscreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_interface_lockscreen_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneInterfaceLockscreen, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_interface_lockscreen_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
