#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexChangeDeviceName,
    VarItemListIndexXpLevel,
    VarItemListIndexButthurtTimer,
    VarItemListIndexRgbBacklight,
    VarItemListIndexLcdColor,
};

void xtreme_app_scene_misc_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_misc_xp_level_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->xp_level = variable_item_get_current_value_index(item) + 1;
    char level_str[4];
    snprintf(level_str, 4, "%li", app->xp_level);
    variable_item_set_current_value_text(item, level_str);
    app->save_level = true;
}

const char* const butthurt_timer_names[] =
    {"OFF", "30 M", "1 H", "2 H", "4 H", "6 H", "8 H", "12 H", "24 H", "48 H"};
const int32_t butthurt_timer_values[COUNT_OF(butthurt_timer_names)] =
    {-1, 1800, 3600, 7200, 14400, 21600, 28800, 43200, 86400, 172800};
static void xtreme_app_scene_misc_butthurt_timer_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, butthurt_timer_names[index]);
    XTREME_SETTINGS()->butthurt_timer = butthurt_timer_values[index];
    app->save_settings = true;
    app->require_reboot = true;
}

static void xtreme_app_scene_misc_lcd_color_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rgb_backlight_get_color_text(index));
    rgb_backlight_set_color(index);
    app->save_backlight = true;
    notification_message(app->notification, &sequence_display_backlight_on);
}

void xtreme_app_scene_misc_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(var_item_list, "Change Device Name", 0, NULL, app);

    char level_str[4];
    snprintf(level_str, 4, "%li", app->xp_level);
    item = variable_item_list_add(
        var_item_list,
        "XP Level",
        DOLPHIN_LEVEL_COUNT + 1,
        xtreme_app_scene_misc_xp_level_changed,
        app);
    variable_item_set_current_value_index(item, app->xp_level - 1);
    variable_item_set_current_value_text(item, level_str);

    item = variable_item_list_add(
        var_item_list,
        "Butthurt Timer",
        COUNT_OF(butthurt_timer_names),
        xtreme_app_scene_misc_butthurt_timer_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->butthurt_timer, butthurt_timer_values, COUNT_OF(butthurt_timer_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, butthurt_timer_names[value_index]);

    item = variable_item_list_add(var_item_list, "RGB Backlight", 1, NULL, app);
    variable_item_set_current_value_text(item, xtreme_settings->rgb_backlight ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "LCD Color",
        rgb_backlight_get_color_count(),
        xtreme_app_scene_misc_lcd_color_changed,
        app);
    value_index = rgb_backlight_get_settings()->display_color_index;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rgb_backlight_get_color_text(value_index));
    variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");

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
        case VarItemListIndexChangeDeviceName:
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscRename);
            break;
        case VarItemListIndexRgbBacklight: {
            bool change = XTREME_SETTINGS()->rgb_backlight;
            if(!change) {
                DialogMessage* msg = dialog_message_alloc();
                dialog_message_set_header(msg, "RGB Backlight", 64, 0, AlignCenter, AlignTop);
                dialog_message_set_buttons(msg, "No", NULL, "Yes");
                dialog_message_set_text(
                    msg,
                    "This option requires installing\na hardware modification!\nIs it installed?",
                    64,
                    32,
                    AlignCenter,
                    AlignCenter);
                if(dialog_message_show(app->dialogs, msg) == DialogMessageButtonRight) {
                    change = true;
                }
                dialog_message_free(msg);
            }
            if(change) {
                XTREME_SETTINGS()->rgb_backlight = !XTREME_SETTINGS()->rgb_backlight;
                app->save_settings = true;
                notification_message(app->notification, &sequence_display_backlight_on);
                scene_manager_previous_scene(app->scene_manager);
                scene_manager_next_scene(app->scene_manager, XtremeAppSceneMisc);
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
