#include "../xtreme_app.h"

static void xtreme_app_scene_protocols_bad_bk_mode_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "BT" : "USB");
    XTREME_SETTINGS()->bad_bt = value;
    app->save_settings = true;
}

static void xtreme_app_scene_protocols_bad_bt_remember_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->bad_bt_remember = value;
    app->save_settings = true;
}

static void xtreme_app_scene_protocols_subghz_extend_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_extend = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    app->save_subghz = true;
}

static void xtreme_app_scene_protocols_subghz_bypass_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_bypass = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");
    app->save_subghz = true;
}

void xtreme_app_scene_protocols_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "BadKB Mode", 2, xtreme_app_scene_protocols_bad_bk_mode_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->bad_bt);
    variable_item_set_current_value_text(item, xtreme_settings->bad_bt ? "BT" : "USB");

    item = variable_item_list_add(
        var_item_list,
        "BadBT Rmembr",
        2,
        xtreme_app_scene_protocols_bad_bt_remember_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->bad_bt_remember);
    variable_item_set_current_value_text(item, xtreme_settings->bad_bt_remember ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "SubGHz Extend", 2, xtreme_app_scene_protocols_subghz_extend_changed, app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "SubGHz Bypass", 2, xtreme_app_scene_protocols_subghz_bypass_changed, app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");

    variable_item_list_set_selected_item(var_item_list, 0);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_protocols_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void xtreme_app_scene_protocols_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
