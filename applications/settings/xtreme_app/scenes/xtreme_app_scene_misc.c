#include "../xtreme_app.h"

static void xtreme_app_scene_misc_sort_folders_before_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->sort_dirs_first = value;
    app->save_settings = true;
}

void xtreme_app_scene_misc_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Sort Dirs First",
        2,
        xtreme_app_scene_misc_sort_folders_before_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->sort_dirs_first);
    variable_item_set_current_value_text(item, xtreme_settings->sort_dirs_first ? "ON" : "OFF");

    variable_item_list_set_selected_item(var_item_list, 0);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_misc_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void xtreme_app_scene_misc_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
