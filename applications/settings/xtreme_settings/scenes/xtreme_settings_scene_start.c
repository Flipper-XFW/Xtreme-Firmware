#include "../xtreme_settings_app.h"
#include <lib/toolbox/value_index.h>
#include <lib/flipper_format/flipper_format.h>

static void xtreme_settings_scene_start_base_mode_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "SFW" : "NSFW");
    XTREME_SETTINGS()->sfw_mode = value;
    XTREME_SETTINGS_SAVE();
}

#define CYCLE_ANIMS_COUNT 13
const char* const cycle_anims_names[CYCLE_ANIMS_COUNT] = {
    "OFF",
    "Manifest",
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
    "24 H",
};

const int32_t cycle_anims_values[CYCLE_ANIMS_COUNT] =
    {-1, 0, 30, 60, 300, 600, 900, 1800, 3600, 7200, 21600, 43200, 86400};

static void xtreme_settings_scene_start_cycle_anims_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, cycle_anims_names[index]);
    XTREME_SETTINGS()->cycle_anims = cycle_anims_values[index];
    XTREME_SETTINGS_SAVE();
}

static void xtreme_settings_scene_start_unlock_anims_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    XTREME_SETTINGS()->unlock_anims = value;
    XTREME_SETTINGS_SAVE();
}

static void xtreme_settings_scene_start_xp_level_changed(VariableItem* item) {
    XtremeSettingsApp* app = variable_item_get_context(item);
    uint8_t level = variable_item_get_current_value_index(item) + 1;
    uint32_t xp = app->dolphin->state->data.icounter;
    if (level > app->dolphin_stats.level) {
        app->dolphin->state->data.icounter = xp + dolphin_state_xp_to_levelup(xp) + 1;
    } else {
        xp = xp - dolphin_state_xp_above_last_levelup(xp) - 1;  // Get to top of previous level
        app->dolphin->state->data.icounter = xp - dolphin_state_xp_above_last_levelup(xp) + 1;  // Get to bottom of it
    }
    app->dolphin->state->dirty = true;
    dolphin_state_save(app->dolphin->state);
    char level_str[4];
    snprintf(level_str, 4, "%i", level);
    variable_item_set_current_value_text(item, level_str);
}

static void xtreme_settings_scene_start_subghz_extend_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* subghz_range = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(subghz_range, "/ext/subghz/assets/extend_range.txt") &&
    flipper_format_insert_or_update_bool(subghz_range, "use_ext_range_at_own_risk", &value, 1)) {
        variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    } else {
        variable_item_set_current_value_index(item, !value);
    }
    flipper_format_free(subghz_range);
    furi_record_close(RECORD_STORAGE);
}

static void xtreme_settings_scene_start_subghz_bypass_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* subghz_range = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(subghz_range, "/ext/subghz/assets/extend_range.txt") &&
    flipper_format_insert_or_update_bool(subghz_range, "ignore_default_tx_region", &value, 1)) {
        variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    } else {
        variable_item_set_current_value_index(item, !value);
    }
    flipper_format_free(subghz_range);
    furi_record_close(RECORD_STORAGE);
}

void xtreme_settings_scene_start_on_enter(void* context) {
    XtremeSettingsApp* app = context;
    XtremeSettings* xtreme = XTREME_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* subghz_range = flipper_format_file_alloc(storage);
    bool subghz_extend = false;
    bool subghz_bypass = false;
    if(flipper_format_file_open_existing(subghz_range, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(subghz_range, "use_ext_range_at_own_risk", &subghz_extend, 1);
        flipper_format_read_bool(subghz_range, "ignore_default_tx_region", &subghz_bypass, 1);
    }
    flipper_format_free(subghz_range);
    furi_record_close(RECORD_STORAGE);

    item = variable_item_list_add(
        var_item_list,
        "Graphics",
        2,
        xtreme_settings_scene_start_base_mode_changed,
        app);
    variable_item_set_current_value_index(item, xtreme->sfw_mode);
    variable_item_set_current_value_text(item, xtreme->sfw_mode ? "SFW" : "NSFW");

    item = variable_item_list_add(
        var_item_list,
        "Cycle Anims",
        CYCLE_ANIMS_COUNT,
        xtreme_settings_scene_start_cycle_anims_changed,
        app);
    value_index = value_index_int32(
        xtreme->cycle_anims, cycle_anims_values, CYCLE_ANIMS_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, cycle_anims_names[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Unlock Anims",
        2,
        xtreme_settings_scene_start_unlock_anims_changed,
        app);
    variable_item_set_current_value_index(item, xtreme->unlock_anims);
    variable_item_set_current_value_text(item, xtreme->unlock_anims ? "ON" : "OFF");

    char level_str[4];
    snprintf(level_str, 4, "%i", app->dolphin_stats.level);
    item = variable_item_list_add(
        var_item_list,
        "XP Level",
        DOLPHIN_LEVEL_COUNT + 1,
        xtreme_settings_scene_start_xp_level_changed,
        app);
    variable_item_set_current_value_index(item, app->dolphin_stats.level - 1);
    variable_item_set_current_value_text(item, level_str);

    item = variable_item_list_add(
        var_item_list,
        "SubGHz Extend",
        2,
        xtreme_settings_scene_start_subghz_extend_changed,
        app);
    variable_item_set_current_value_index(item, subghz_extend);
    variable_item_set_current_value_text(item, subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "SubGHz Bypass",
        2,
        xtreme_settings_scene_start_subghz_bypass_changed,
        app);
    variable_item_set_current_value_index(item, subghz_bypass);
    variable_item_set_current_value_text(item, subghz_bypass ? "ON" : "OFF");

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
    variable_item_list_reset(app->var_item_list);
}
