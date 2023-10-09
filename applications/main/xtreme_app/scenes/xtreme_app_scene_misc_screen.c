#include "../xtreme_app.h"

enum VarItemListIndex {
    VarItemListIndexDarkMode,
    VarItemListIndexLeftHanded,
    VarItemListIndexRgbBacklight,
    VarItemListIndexLcdColor0,
    VarItemListIndexLcdColor1,
    VarItemListIndexLcdColor2,
    VarItemListIndexRainbowLcd,
    VarItemListIndexRainbowSpeed,
    VarItemListIndexRainbowInterval,
    VarItemListIndexRainbowSaturation,
};

void xtreme_app_scene_misc_screen_var_item_list_callback(void* context, uint32_t index) {
    XtremeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void xtreme_app_scene_misc_screen_dark_mode_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->dark_mode = value;
    app->save_settings = true;
}

static void xtreme_app_scene_misc_screen_hand_orient_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    if(value) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagHandOrient);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagHandOrient);
    }
}

static const struct {
    char* name;
    RgbColor color;
} lcd_colors[] = {
    {"Off", {0, 0, 0}},         {"Orange", {255, 69, 0}},  {"Red", {255, 0, 0}},
    {"Maroon", {128, 0, 0}},    {"Yellow", {255, 255, 0}}, {"Olive", {128, 128, 0}},
    {"Lime", {0, 255, 0}},      {"Green", {0, 128, 0}},    {"Aqua", {0, 255, 127}},
    {"Cyan", {0, 210, 210}},    {"Azure", {0, 127, 255}},  {"Teal", {0, 128, 128}},
    {"Blue", {0, 0, 255}},      {"Navy", {0, 0, 128}},     {"Purple", {128, 0, 128}},
    {"Fuchsia", {255, 0, 255}}, {"Pink", {173, 31, 173}},  {"Brown", {165, 42, 42}},
    {"White", {255, 192, 203}},
};
static void xtreme_app_scene_misc_screen_lcd_color_changed(VariableItem* item, uint8_t led) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, lcd_colors[index].name);
    rgb_backlight_set_color(led, &lcd_colors[index].color);
    app->save_backlight = true;
}
static void xtreme_app_scene_misc_screen_lcd_color_0_changed(VariableItem* item) {
    xtreme_app_scene_misc_screen_lcd_color_changed(item, 0);
}
static void xtreme_app_scene_misc_screen_lcd_color_1_changed(VariableItem* item) {
    xtreme_app_scene_misc_screen_lcd_color_changed(item, 1);
}
static void xtreme_app_scene_misc_screen_lcd_color_2_changed(VariableItem* item) {
    xtreme_app_scene_misc_screen_lcd_color_changed(item, 2);
}

const char* const rainbow_lcd_names[RGBBacklightRainbowModeCount] = {
    "OFF",
    "Wave",
    "Static",
};
static void xtreme_app_scene_misc_screen_rainbow_lcd_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_lcd_names[index]);
    rgb_backlight_set_rainbow_mode(index);
    app->save_backlight = true;
}

static void xtreme_app_scene_misc_screen_rainbow_speed_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_speed(index);
    app->save_backlight = true;
}

const char* const rainbow_interval_names[] = {
    "0.1 S",
    "0.2 S",
    "0.25 S",
    "0.5 S",
    "0.75 S",
    "1 S",
    "1.25 S",
    "1.5 S",
    "1.75 S",
    "2 S",
    "2.5 S",
    "3 S",
    "4 S",
    "5 S",
};
const uint32_t rainbow_interval_values[COUNT_OF(rainbow_interval_names)] = {
    100,
    200,
    250,
    500,
    750,
    1000,
    1250,
    1500,
    1750,
    2000,
    2500,
    3000,
    4000,
    5000,
};
static void xtreme_app_scene_misc_screen_rainbow_interval_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_interval_names[index]);
    rgb_backlight_set_rainbow_interval(rainbow_interval_values[index]);
    app->save_backlight = true;
}

static void xtreme_app_scene_misc_screen_rainbow_saturation_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_saturation(index);
    app->save_backlight = true;
}

void xtreme_app_scene_misc_screen_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list, "Dark Mode", 2, xtreme_app_scene_misc_screen_dark_mode_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->dark_mode);
    variable_item_set_current_value_text(item, xtreme_settings->dark_mode ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "Left Handed", 2, xtreme_app_scene_misc_screen_hand_orient_changed, app);
    bool value = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient);
    variable_item_set_current_value_index(item, value);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "RGB Backlight", 1, NULL, app);
    variable_item_set_current_value_text(item, xtreme_settings->rgb_backlight ? "ON" : "OFF");

    struct {
        uint8_t led;
        VariableItemChangeCallback cb;
    } lcd_cols[] = {
        {0, xtreme_app_scene_misc_screen_lcd_color_0_changed},
        {1, xtreme_app_scene_misc_screen_lcd_color_1_changed},
        {2, xtreme_app_scene_misc_screen_lcd_color_2_changed},
    };
    size_t lcd_sz = COUNT_OF(lcd_colors);

    RgbColor color;
    for(size_t i = 0; i < COUNT_OF(lcd_cols); i++) {
        char name[12];
        snprintf(name, sizeof(name), "LCD LED %u", lcd_cols[i].led + 1);
        item = variable_item_list_add(var_item_list, name, lcd_sz, lcd_cols[i].cb, app);
        rgb_backlight_get_color(lcd_cols[i].led, &color);
        bool found = false;
        for(size_t i = 0; i < lcd_sz; i++) {
            if(rgbcmp(&color, &lcd_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }
        variable_item_set_current_value_index(item, found ? value_index : lcd_sz);
        if(found) {
            variable_item_set_current_value_text(item, lcd_colors[value_index].name);
        } else {
            char str[7];
            snprintf(str, sizeof(str), "%02X%02X%02X", color.r, color.g, color.b);
            variable_item_set_current_value_text(item, str);
        }
        variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");
    }

    item = variable_item_list_add(
        var_item_list,
        "Rainbow LCD",
        RGBBacklightRainbowModeCount,
        xtreme_app_scene_misc_screen_rainbow_lcd_changed,
        app);
    value_index = rgb_backlight_get_rainbow_mode();
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rainbow_lcd_names[value_index]);
    variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");

    item = variable_item_list_add(
        var_item_list,
        "Rainbow Speed",
        25,
        xtreme_app_scene_misc_screen_rainbow_speed_changed,
        app);
    value_index = rgb_backlight_get_rainbow_speed();
    variable_item_set_current_value_index(item, value_index - 1);
    char speed_str[4];
    snprintf(speed_str, sizeof(speed_str), "%d", value_index);
    variable_item_set_current_value_text(item, speed_str);
    variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");

    item = variable_item_list_add(
        var_item_list,
        "Rainbow Interval",
        COUNT_OF(rainbow_interval_values),
        xtreme_app_scene_misc_screen_rainbow_interval_changed,
        app);
    value_index = value_index_uint32(
        rgb_backlight_get_rainbow_interval(),
        rainbow_interval_values,
        COUNT_OF(rainbow_interval_values));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rainbow_interval_names[value_index]);
    variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");

    item = variable_item_list_add(
        var_item_list,
        "Rainbow Saturation",
        255,
        xtreme_app_scene_misc_screen_rainbow_saturation_changed,
        app);
    value_index = rgb_backlight_get_rainbow_saturation();
    variable_item_set_current_value_index(item, value_index - 1);
    char saturation_str[4];
    snprintf(saturation_str, sizeof(saturation_str), "%d", value_index);
    variable_item_set_current_value_text(item, saturation_str);
    variable_item_set_locked(item, !xtreme_settings->rgb_backlight, "Needs RGB\nBacklight!");

    variable_item_list_set_enter_callback(
        var_item_list, xtreme_app_scene_misc_screen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, XtremeAppSceneMiscScreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_misc_screen_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, XtremeAppSceneMiscScreen, event.event);
        consumed = true;
        switch(event.event) {
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
                app->save_backlight = true;
                notification_message(app->notification, &sequence_display_backlight_on);
                rgb_backlight_reconfigure(XTREME_SETTINGS()->rgb_backlight);
                scene_manager_previous_scene(app->scene_manager);
                scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscScreen);
            }
            break;
        }
        case VarItemListIndexLcdColor0:
        case VarItemListIndexLcdColor1:
        case VarItemListIndexLcdColor2:
            scene_manager_set_scene_state(
                app->scene_manager,
                XtremeAppSceneMiscScreenColor,
                event.event - VarItemListIndexLcdColor0);
            scene_manager_next_scene(app->scene_manager, XtremeAppSceneMiscScreenColor);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void xtreme_app_scene_misc_screen_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
