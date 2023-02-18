#include "../xtreme_app.h"
#include <lib/toolbox/value_index.h>
#include <power/power_service/power.h>
#include <lib/toolbox/version.h>

static void xtreme_app_scene_main_asset_pack_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, index == 0 ? "SFW" : *asset_packs_get(app->asset_packs, index - 1));
    strlcpy(
        XTREME_SETTINGS()->asset_pack,
        index == 0 ? "" : *asset_packs_get(app->asset_packs, index - 1),
        MAX_PACK_NAME_LEN);
    app->asset_pack = index;
    app->save_settings = true;
    app->require_reboot = true;
}

const char* const anim_speed_names[] =
    {"25%", "50%", "75%", "100%", "125%", "150%", "175%", "200%", "225%", "250%", "275%", "300%"};
const int32_t anim_speed_values[COUNT_OF(anim_speed_names)] =
    {25, 50, 75, 100, 125, 150, 175, 200, 225, 250, 275, 300};
static void xtreme_app_scene_main_anim_speed_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, anim_speed_names[index]);
    XTREME_SETTINGS()->anim_speed = anim_speed_values[index];
    app->save_settings = true;
}

const char* const cycle_anims_names[] = {
    "OFF",
    "Meta.txt",
    "30 S",
    "1 M",
    "5 M",
    "10 M",
    "15 M",
    "30 M",
    "1 H",
    "2 H",
    "6 H",
    "12 H",
    "24 H"};
const int32_t cycle_anims_values[COUNT_OF(cycle_anims_names)] =
    {-1, 0, 30, 60, 300, 600, 900, 1800, 3600, 7200, 21600, 43200, 86400};
static void xtreme_app_scene_main_cycle_anims_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, cycle_anims_names[index]);
    XTREME_SETTINGS()->cycle_anims = cycle_anims_values[index];
    app->save_settings = true;
}

static void xtreme_app_scene_main_unlock_anims_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->unlock_anims = value;
    app->save_settings = true;
}

const char* const battery_icon_names[] =
    {"OFF", "Bar", "%", "Inv. %", "Retro 3", "Retro 5", "Bar %"};
static void xtreme_app_scene_main_battery_icon_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, battery_icon_names[index]);
    XTREME_SETTINGS()->battery_icon = index;
    app->save_settings = true;
}

static void xtreme_app_scene_main_status_icons_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->status_icons = value;
    app->save_settings = true;
}

static void xtreme_app_scene_main_bar_borders_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->bar_borders = value;
    app->save_settings = true;
}

static void xtreme_app_scene_main_bar_background_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->bar_background = value;
    app->save_settings = true;
}

static void xtreme_app_scene_main_bad_bk_mode_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "BT" : "USB");
    XTREME_SETTINGS()->bad_bt = value;
    app->save_settings = true;
}

static void xtreme_app_scene_main_subghz_extend_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_extend = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    app->save_subghz = true;
}

static void xtreme_app_scene_main_subghz_bypass_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->subghz_bypass = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");
    app->save_subghz = true;
}

static void xtreme_app_scene_main_xp_level_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    app->dolphin_level = variable_item_get_current_value_index(item) + 1;
    char level_str[4];
    snprintf(level_str, 4, "%i", app->dolphin_level);
    variable_item_set_current_value_text(item, level_str);
    app->save_level = true;
}

const char* const butthurt_timer_names[] = {
    "OFF",
    "30 M",
    "1 H",
    "2 H",
    "4 H",
    "6 H",
    "8 H",
    "12 H",
    "24 H",
    "48 H"};
const int32_t butthurt_timer_values[COUNT_OF(butthurt_timer_names)] =
    {-1, 1800, 3600, 7200, 14400, 21600, 28800, 43200, 86400, 172800};
static void xtreme_app_scene_main_butthurt_timer_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, butthurt_timer_names[index]);
    XTREME_SETTINGS()->butthurt_timer = butthurt_timer_values[index];
    app->save_settings = true;
    app->require_reboot = true;
}

static void xtreme_app_scene_main_sort_folders_before_changed(VariableItem* item) {
    XtremeApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->sort_dirs_first = value;
    app->save_settings = true;
}

void xtreme_app_scene_main_on_enter(void* context) {
    XtremeApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list,
        "Asset Pack",
        asset_packs_size(app->asset_packs) + 1,
        xtreme_app_scene_main_asset_pack_changed,
        app);
    variable_item_set_current_value_index(item, app->asset_pack);
    variable_item_set_current_value_text(
        item, app->asset_pack == 0 ? "SFW" : *asset_packs_get(app->asset_packs, app->asset_pack - 1));

    item = variable_item_list_add(
        var_item_list,
        "Anim Speed",
        COUNT_OF(anim_speed_names),
        xtreme_app_scene_main_anim_speed_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->anim_speed, anim_speed_values, COUNT_OF(anim_speed_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, anim_speed_names[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Cycle Anims",
        COUNT_OF(cycle_anims_names),
        xtreme_app_scene_main_cycle_anims_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->cycle_anims, cycle_anims_values, COUNT_OF(cycle_anims_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, cycle_anims_names[value_index]);

    item = variable_item_list_add(
        var_item_list, "Unlock Anims", 2, xtreme_app_scene_main_unlock_anims_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->unlock_anims);
    variable_item_set_current_value_text(item, xtreme_settings->unlock_anims ? "ON" : "OFF");


    variable_item_list_add(var_item_list, "             = Status Bar =", 0, NULL, app);

    item = variable_item_list_add(
        var_item_list,
        "Battery Icon",
        BatteryIconCount,
        xtreme_app_scene_main_battery_icon_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->battery_icon);
    variable_item_set_current_value_text(item, battery_icon_names[xtreme_settings->battery_icon]);

    item = variable_item_list_add(
        var_item_list, "Status Icons", 2, xtreme_app_scene_main_status_icons_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->status_icons);
    variable_item_set_current_value_text(item, xtreme_settings->status_icons ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "Bar Borders", 2, xtreme_app_scene_main_bar_borders_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->bar_borders);
    variable_item_set_current_value_text(item, xtreme_settings->bar_borders ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "Bar Background", 2, xtreme_app_scene_main_bar_background_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->bar_background);
    variable_item_set_current_value_text(item, xtreme_settings->bar_background ? "ON" : "OFF");


    variable_item_list_add(var_item_list, "               = Protocols =", 0, NULL, app);

    item = variable_item_list_add(
        var_item_list, "Bad KB Mode", 2, xtreme_app_scene_main_bad_bk_mode_changed, app);
    variable_item_set_current_value_index(item, xtreme_settings->bad_bt);
    variable_item_set_current_value_text(item, xtreme_settings->bad_bt ? "BT" : "USB");

    item = variable_item_list_add(
        var_item_list, "SubGHz Extend", 2, xtreme_app_scene_main_subghz_extend_changed, app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "SubGHz Bypass", 2, xtreme_app_scene_main_subghz_bypass_changed, app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");


    variable_item_list_add(var_item_list, "                  = Dolphin =", 0, NULL, app);

    char level_str[4];
    snprintf(level_str, 4, "%i", app->dolphin_level);
    item = variable_item_list_add(
        var_item_list,
        "XP Level",
        DOLPHIN_LEVEL_COUNT + 1,
        xtreme_app_scene_main_xp_level_changed,
        app);
    variable_item_set_current_value_index(item, app->dolphin_level - 1);
    variable_item_set_current_value_text(item, level_str);

    item = variable_item_list_add(
        var_item_list,
        "Butthurt Timer",
        COUNT_OF(butthurt_timer_names),
        xtreme_app_scene_main_butthurt_timer_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->butthurt_timer, butthurt_timer_values, COUNT_OF(butthurt_timer_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, butthurt_timer_names[value_index]);


    variable_item_list_add(var_item_list, "                     = Misc =", 0, NULL, app);

    item = variable_item_list_add(
        var_item_list,
        "Sort Dirs First",
        2,
        xtreme_app_scene_main_sort_folders_before_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->sort_dirs_first);
    variable_item_set_current_value_text(item, xtreme_settings->sort_dirs_first ? "ON" : "OFF");

    variable_item_list_add(var_item_list, furi_string_get_cstr(app->version_tag), 0, NULL, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewVarItemList);
}

bool xtreme_app_scene_main_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void xtreme_app_scene_main_on_exit(void* context) {
    XtremeApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
