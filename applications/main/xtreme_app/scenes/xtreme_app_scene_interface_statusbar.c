#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexBatteryIcon,
    VarItemListIndexShowClock,
    VarItemListIndexStatusIcons,
    VarItemListIndexBarBorders,
    VarItemListIndexBarBackground,
};

void xtreme_app_scene_interface_statusbar_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

const char* const battery_icon_names[BatteryIconCount] = {
    "OFF",
    "Bar",
    "%",
    "Inv. %",
    "Retro 3",
    "Retro 5",
    "Bar %",
};
static void xtreme_app_scene_interface_statusbar_battery_icon_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, battery_icon_names[index]);
    xtreme_settings.battery_icon = index;
    app->save_settings = true;
    power_set_battery_icon_enabled(furi_record_open(RECORD_POWER), index != BatteryIconOff);
    furi_record_close(RECORD_POWER);
}

static void xtreme_app_scene_interface_statusbar_statusbar_clock_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    xtreme_settings.statusbar_clock = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_statusbar_status_icons_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    xtreme_settings.status_icons = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_statusbar_bar_borders_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    xtreme_settings.bar_borders = value;
    app->save_settings = true;
}

static void xtreme_app_scene_interface_statusbar_bar_background_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    xtreme_settings.bar_background = value;
    app->save_settings = true;
}

void xtreme_app_scene_interface_statusbar_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Battery Icon",
        BatteryIconCount,
        xtreme_app_scene_interface_statusbar_battery_icon_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings.battery_icon);
    variable_item_set_current_value_text(item, battery_icon_names[xtreme_settings.battery_icon]);

    item = variable_item_list_add(
        var_item_list,
        "Show Clock",
        2,
        xtreme_app_scene_interface_statusbar_statusbar_clock_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings.statusbar_clock);
    variable_item_set_current_value_text(item, xtreme_settings.statusbar_clock ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Status Icons",
        2,
        xtreme_app_scene_interface_statusbar_status_icons_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings.status_icons);
    variable_item_set_current_value_text(item, xtreme_settings.status_icons ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Bar Borders",
        2,
        xtreme_app_scene_interface_statusbar_bar_borders_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings.bar_borders);
    variable_item_set_current_value_text(item, xtreme_settings.bar_borders ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Bar Background",
        2,
        xtreme_app_scene_interface_statusbar_bar_background_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings.bar_background);
    variable_item_set_current_value_text(item, xtreme_settings.bar_background ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_interface_statusbar_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneInterfaceStatusbar));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_interface_statusbar_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneInterfaceStatusbar, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_interface_statusbar_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
