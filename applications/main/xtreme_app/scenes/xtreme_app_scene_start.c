#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexGraphics,
    VarItemListIndexStatusbar,
    VarItemListIndexProtocols,
    VarItemListIndexDolphin,
    VarItemListIndexMisc,
};

void xtreme_app_scene_start_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void xtreme_app_scene_start_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_add(var_item_list, "Graphics", 0, NULL, app);
    variable_item_list_add(var_item_list, "Statusbar", 0, NULL, app);
    variable_item_list_add(var_item_list, "Protocols", 0, NULL, app);
    variable_item_list_add(var_item_list, "Dolphin", 0, NULL, app);
    variable_item_list_add(var_item_list, "Misc", 0, NULL, app);

    variable_item_list_add(var_item_list, furi_string_get_cstr(app->version_tag), 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_start_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_start_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneStart, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexGraphics:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneGraphics);
            break;
        case VarItemListIndexStatusbar:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneStatusbar);
            break;
        case VarItemListIndexProtocols:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneProtocols);
            break;
        case VarItemListIndexDolphin:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneDolphin);
            break;
        case VarItemListIndexMisc:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMisc);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_start_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
