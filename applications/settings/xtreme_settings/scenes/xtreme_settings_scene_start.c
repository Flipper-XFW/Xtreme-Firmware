#include "../xtreme_settings_app.h"
#include <lib/toolbox/value_index.h>
#include <power/power_service/power.h>
#include <lib/toolbox/version.h>

static void xtreme_settings_scene_start_base_graphics_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "NSFW" : "SFW");
    XTREME_SETTINGS()->nsfw_mode = value;
    app->settings_changed = true;
    app->assets_changed = true;
}

static void xtreme_settings_scene_start_asset_pack_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, index == 0 ? "OFF" : *asset_packs_get(app->asset_packs, index - 1));
    strlcpy(XTREME_SETTINGS()->asset_pack, index == 0 ? "" : *asset_packs_get(app->asset_packs, index - 1), MAX_PACK_NAME_LEN);
    app->settings_changed = true;
    app->assets_changed = true;
}

const char* const anim_speed_names[] =
    {"25%", "50%", "75%", "100%", "125%", "150%", "175%", "200%", "225%", "250%", "275%", "300%"};
const int32_t anim_speed_values[COUNT_OF(anim_speed_names)] =
    {25, 50, 75, 0, 125, 150, 175, 200, 225, 250, 275, 300};
static void xtreme_settings_scene_start_anim_speed_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, anim_speed_names[index]);
    XTREME_SETTINGS()->anim_speed = anim_speed_values[index];
    app->settings_changed = true;
}

const char* const cycle_anims_names[] =
    {"OFF", "Meta.txt", "30 S", "1 M", "5 M", "10 M", "15 M", "30 M", "1 H", "2 H", "6 H", "12 H", "24 H"};
const int32_t cycle_anims_values[COUNT_OF(cycle_anims_names)] =
    {-1, 0, 30, 60, 300, 600, 900, 1800, 3600, 7200, 21600, 43200, 86400};
static void xtreme_settings_scene_start_cycle_anims_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, cycle_anims_names[index]);
    XTREME_SETTINGS()->cycle_anims = cycle_anims_values[index];
    app->settings_changed = true;
}

static void xtreme_settings_scene_start_unlock_anims_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->unlock_anims = value;
    app->settings_changed = true;
}

const char* const battery_style_names[] =
    {"OFF", "Bar", "%", "Inv. %", "Retro 3", "Retro 5", "Bar %"};
const int32_t battery_style_values[COUNT_OF(battery_style_names)] = {
    BatteryStyleOff,
    BatteryStyleBar,
    BatteryStylePercent,
    BatteryStyleInvertedPercent,
    BatteryStyleRetro3,
    BatteryStyleRetro5,
    BatteryStyleBarPercent
};
static void xtreme_settings_scene_start_battery_style_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, battery_style_names[index]);
    XTREME_SETTINGS()->battery_style = battery_style_values[index];
    app->settings_changed = true;
}

static void xtreme_settings_scene_start_xp_level_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    app->dolphin_level = variable_item_get_current_value_index(item) + 1;
    char level_str[4];
    snprintf(level_str, 4, "%i", app->dolphin_level);
    variable_item_set_current_value_text(item, level_str);
    app->level_changed = true;
}

static void xtreme_settings_scene_start_subghz_extend_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    app->subghz_extend = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    app->subghz_changed = true;
}

static void xtreme_settings_scene_start_subghz_bypass_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    app->subghz_bypass = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");
    app->subghz_changed = true;
}

void xtreme_settings_scene_start_on_enter(void* context) {
    XtremeSettingsApp* app = context;
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
    DolphinStats stats = dolphin_stats(dolphin);
    furi_record_close(RECORD_DOLPHIN);
    app->dolphin_level = stats.level;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* subghz_range = flipper_format_file_alloc(storage);
    app->subghz_extend = false;
    app->subghz_bypass = false;
    if(flipper_format_file_open_existing(subghz_range, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(subghz_range, "use_ext_range_at_own_risk", &app->subghz_extend, 1);
        flipper_format_read_bool(subghz_range, "ignore_default_tx_region", &app->subghz_bypass, 1);
    }
    flipper_format_free(subghz_range);

    uint current_pack = 0;
    asset_packs_init(app->asset_packs);
    File* folder = storage_file_alloc(storage);
    FileInfo info;
    char* name = malloc(MAX_PACK_NAME_LEN);
    do {
        if (!storage_dir_open(folder, PACKS_DIR)) break;
        while(true) {
            if (!storage_dir_read(folder, &info, name, MAX_PACK_NAME_LEN)) break;
            if(info.flags & FSF_DIRECTORY) {
                char* copy = malloc(MAX_PACK_NAME_LEN);
                strlcpy(copy, name, MAX_PACK_NAME_LEN);
                asset_packs_push_back(app->asset_packs, copy);
                if (strcmp(name, xtreme_settings->asset_pack) == 0) current_pack = asset_packs_size(app->asset_packs);
            }
        }
    } while(false);
    free(name);
    storage_file_free(folder);
    furi_record_close(RECORD_STORAGE);

    item = variable_item_list_add(
        var_item_list,
        "Base Graphics",
        2,
        xtreme_settings_scene_start_base_graphics_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->nsfw_mode);
    variable_item_set_current_value_text(item, xtreme_settings->nsfw_mode ? "NSFW" : "SFW");

    item = variable_item_list_add(
        var_item_list,
        "Asset Pack",
        asset_packs_size(app->asset_packs) + 1,
        xtreme_settings_scene_start_asset_pack_changed,
        app);
    variable_item_set_current_value_index(item, current_pack);
    variable_item_set_current_value_text(item, current_pack == 0 ? "OFF" : *asset_packs_get(app->asset_packs, current_pack - 1));

    item = variable_item_list_add(
        var_item_list,
        "Anim Speed",
        COUNT_OF(anim_speed_names),
        xtreme_settings_scene_start_anim_speed_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->anim_speed, anim_speed_values, COUNT_OF(anim_speed_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, anim_speed_names[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Cycle Anims",
        COUNT_OF(cycle_anims_names),
        xtreme_settings_scene_start_cycle_anims_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->cycle_anims, cycle_anims_values, COUNT_OF(cycle_anims_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, cycle_anims_names[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Unlock Anims",
        2,
        xtreme_settings_scene_start_unlock_anims_changed,
        app);
    variable_item_set_current_value_index(item, xtreme_settings->unlock_anims);
    variable_item_set_current_value_text(item, xtreme_settings->unlock_anims ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "Battery Style",
        COUNT_OF(battery_style_names),
        xtreme_settings_scene_start_battery_style_changed,
        app);
    value_index = value_index_int32(
        xtreme_settings->battery_style, battery_style_values, COUNT_OF(battery_style_names));
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, battery_style_names[value_index]);

    char level_str[4];
    snprintf(level_str, 4, "%i", app->dolphin_level);
    item = variable_item_list_add(
        var_item_list,
        "XP Level",
        DOLPHIN_LEVEL_COUNT + 1,
        xtreme_settings_scene_start_xp_level_changed,
        app);
    variable_item_set_current_value_index(item, app->dolphin_level - 1);
    variable_item_set_current_value_text(item, level_str);

    item = variable_item_list_add(
        var_item_list,
        "SubGHz Extend",
        2,
        xtreme_settings_scene_start_subghz_extend_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "SubGHz Bypass",
        2,
        xtreme_settings_scene_start_subghz_bypass_changed,
        app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");

    FuriString* version_tag = furi_string_alloc_printf("%s  %s", version_get_gitbranchnum(NULL), version_get_builddate(NULL));
    item = variable_item_list_add(
        var_item_list,
        furi_string_get_cstr(version_tag),
        0,
        NULL,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeSettingsAppViewVarItemList);
}

bool xtreme_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void xtreme_settings_scene_start_on_exit(void* context) {
    XtremeSettingsApp* app = context;
    asset_packs_it_t it;
    for (asset_packs_it(it, app->asset_packs); !asset_packs_end_p(it); asset_packs_next(it)) {
        free(*asset_packs_cref(it));
    }
    asset_packs_clear(app->asset_packs);
    variable_item_list_reset(app->var_item_list);
}
