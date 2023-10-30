#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexScreen,
    VarItemListIndexDolphin,
    VarItemListIndexChangeDeviceName,
    VarItemListIndexChargeCap,
    VarItemListIndexShowXtremeIntro,
};

void xtreme_app_scene_misc_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

#define CHARGE_CAP_INTV 5
static void xtreme_app_scene_misc_charge_cap_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    char cap_str[6];
    uint32_t value = (variable_item_get_current_value_index(item) + 1) * CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", value);
    variable_item_set_current_value_text(item, cap_str);
    xtreme_settings.charge_cap = value;
    app->save_settings = true;
}

void xtreme_app_scene_misc_on_enter(void* context) {
    XtremeApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(var_item_list, "Screen", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    item = variable_item_list_add(var_item_list, "Dolphin", 0, NULL, app);
    variable_item_set_current_value_text(item, ">");

    variable_item_list_add(var_item_list, "Change Device Name", 0, NULL, app);

    char cap_str[6];
    value_index = xtreme_settings.charge_cap / CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", (uint32_t)value_index * CHARGE_CAP_INTV);
    item = variable_item_list_add(
        var_item_list,
        "Charge Cap",
        100 / CHARGE_CAP_INTV,
        xtreme_app_scene_misc_charge_cap_changed,
        app);
    variable_item_set_current_value_index(item, value_index - 1);
    variable_item_set_current_value_text(item, cap_str);

    variable_item_list_add(var_item_list, "Show Xtreme Intro", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_misc_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneMisc));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_misc_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneMisc, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexScreen:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneMiscScreen, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscScreen);
            break;
        case VarItemListIndexDolphin:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneMiscDolphin, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscDolphin);
            break;
        case VarItemListIndexChangeDeviceName:
            scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneMiscRename, 0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscRename);
            break;
        case VarItemListIndexShowXtremeIntro: {
            for(int i = 0; i < 10; i++) {
                if(storage_common_copy(
                       furi_record_open(RECORD_STORAGE),
                       EXT_PATH("dolphin/xfwfirstboot.bin"),
                       EXT_PATH(".slideshow"))) {
                    app->show_slideshow = true;
                    xtreme_app_apply(app);
                    break;
                }
            }
            break;
        }
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_misc_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
