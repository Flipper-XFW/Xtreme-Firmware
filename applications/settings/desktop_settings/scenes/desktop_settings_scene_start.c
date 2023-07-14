#include <applications.h>
#include <lib/toolbox/value_index.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"

enum VarItemListIndex {
    VarItemListIndexKeybinds,
    VarItemListIndexResetKeybinds,
    VarItemListIndexPinSetup,
    VarItemListIndexAutoLockTime,
    VarItemListIndexAutoLockPin,
};

#define AUTO_LOCK_DELAY_COUNT 9
const char* const auto_lock_delay_text[AUTO_LOCK_DELAY_COUNT] = {
    "OFF",
    "10s",
    "15s",
    "30s",
    "60s",
    "90s",
    "2min",
    "5min",
    "10min",
};
const uint32_t auto_lock_delay_value[AUTO_LOCK_DELAY_COUNT] =
    {0, 10000, 15000, 30000, 60000, 90000, 120000, 300000, 600000};

static void desktop_settings_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void desktop_settings_scene_start_auto_lock_delay_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, auto_lock_delay_text[index]);
    app->desktop->settings.auto_lock_delay_ms = auto_lock_delay_value[index];
    app->save_settings = true;
}

static void desktop_settings_scene_start_auto_lock_pin_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t value = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->desktop->settings.auto_lock_with_pin = value;
    app->save_settings = true;
}

void desktop_settings_scene_start_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(variable_item_list, "Keybinds Setup", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Reset Keybinds to Default", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "PIN Setup", 1, NULL, NULL);

    item = variable_item_list_add(
        variable_item_list,
        "Auto Lock Time",
        AUTO_LOCK_DELAY_COUNT,
        desktop_settings_scene_start_auto_lock_delay_changed,
        app);

    value_index = value_index_uint32(
        app->desktop->settings.auto_lock_delay_ms, auto_lock_delay_value, AUTO_LOCK_DELAY_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, auto_lock_delay_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Auto Lock Pin",
        2,
        desktop_settings_scene_start_auto_lock_pin_changed,
        app);

    variable_item_set_current_value_index(item, app->desktop->settings.auto_lock_with_pin);
    variable_item_set_current_value_text(
        item, app->desktop->settings.auto_lock_with_pin ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        variable_item_list, desktop_settings_scene_start_var_list_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewVarItemList);
}

bool desktop_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case VarItemListIndexKeybinds:
            scene_manager_set_scene_state(
                app->scene_manager, DesktopSettingsAppSceneKeybindsType, 0);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsType);
            consumed = true;
            break;
        case VarItemListIndexResetKeybinds:
            scene_manager_set_scene_state(
                app->scene_manager, DesktopSettingsAppSceneKeybindsType, 0);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneKeybindsReset);
            consumed = true;
            break;
        case VarItemListIndexPinSetup:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppScenePinMenu);
            consumed = true;
            break;
        }
    }
    return consumed;
}

void desktop_settings_scene_start_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
