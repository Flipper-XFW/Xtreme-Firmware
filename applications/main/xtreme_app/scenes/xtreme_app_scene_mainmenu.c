#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexApp,
    VarItemListIndexRemoveApp,
    VarItemListIndexAddApp,
};

void xtreme_app_scene_mainmenu_var_item_list_callback(
    void* context,
    uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_mainmenu_app_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->mainmenu_app_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, *CharList_get(app->mainmenu_apps, app->mainmenu_app_index));
}

void xtreme_app_scene_mainmenu_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "App",
        CharList_size(app->mainmenu_apps),
        xtreme_app_scene_mainmenu_app_changed,
        app);
    app->mainmenu_app_index = 0;
    variable_item_set_current_value_index(item, app->mainmenu_app_index);
    if(CharList_size(app->mainmenu_apps)) {
        variable_item_set_current_value_text(item, *CharList_get(app->mainmenu_apps, app->mainmenu_app_index));
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Remove App", 0, NULL, app);

    variable_item_list_add(var_item_list, "Add App", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_mainmenu_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(
            app->scene_manager, XtremeAppSceneMainmenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_mainmenu_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, XtremeAppSceneMainmenu, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRemoveApp:
            if(!CharList_size(app->mainmenu_apps)) break;
            char* value =
                *CharList_get(app->mainmenu_apps, app->mainmenu_app_index);
            CharList_it_t it;
            CharList_it(it, app->mainmenu_apps);
            while(!CharList_end_p(it)) {
                if(strcmp(*CharList_ref(it), value) == 0) {
                    CharList_remove(app->mainmenu_apps, it);
                } else {
                    CharList_next(it);
                }
            }
            app->save_mainmenu_apps = true;
            scene_manager_previous_scene(app->scene_manager);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMainmenu);
            break;
        case VarItemListIndexAddApp:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMainmenuAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_mainmenu_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
