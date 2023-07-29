#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexGraphics,
    VarItemListIndexMainmenu,
    VarItemListIndexLockscreen,
    VarItemListIndexStatusbar,
    VarItemListIndexFileBrowser,
};

void xtreme_app_scene_interface_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void xtreme_app_scene_interface_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(var_item_list, "Graphics", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Mainmenu", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Lockscreen", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Statusbar", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "File Browser", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_interface_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneInterface));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_interface_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneInterface, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexGraphics:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneInterfaceGraphics, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceGraphics);
            break;
        case VarItemListIndexMainmenu:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneInterfaceMainmenu, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceMainmenu);
            break;
        case VarItemListIndexLockscreen:
            scene_manager_set_scene_state(
                app->scene_manager, XtremeAppSceneInterfaceLockscreen, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceLockscreen);
            break;
        case VarItemListIndexStatusbar:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneInterfaceStatusbar, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceStatusbar);
            break;
        case VarItemListIndexFileBrowser:
            scene_manager_set_scene_state(
                app->scene_manager, XtremeAppSceneInterfaceFilebrowser, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneInterfaceFilebrowser);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_interface_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
