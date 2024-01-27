#include "../infrared_app_i.h"

uint8_t value_index_ir;

#define DEB_PINS_COUNT (sizeof(infrared_debug_cfg_variables_text) / sizeof(char* const))
const char* const infrared_debug_cfg_variables_text[] = {
    "Internal",
    "2 (A7)",
};

static void infrared_scene_debug_settings_changed(VariableItem* item) {
    value_index_ir = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, infrared_debug_cfg_variables_text[value_index_ir]);

    if(value_index_ir == 0) {
        furi_hal_infrared_set_debug_out(false);
        furi_hal_infrared_block_external_output(true);
        if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
    } else {
        furi_hal_infrared_block_external_output(false);
    }
}

static void infrared_scene_debug_settings_power_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    if(value) {
        for(int i = 0; i < 5 && !furi_hal_power_is_otg_enabled(); i++) {
            furi_hal_power_enable_otg();
            furi_delay_ms(10);
        }
    } else {
        if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
    }
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
}

static void infrared_debug_settings_start_var_list_enter_callback(void* context, uint32_t index) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_debug_settings_on_enter(void* context) {
    InfraredApp* infrared = context;

    VariableItemList* variable_item_list = infrared->variable_item_list;

    value_index_ir =
        (furi_hal_infrared_is_external_connected() &&
         !furi_hal_infrared_is_external_output_blocked());

    VariableItem* item = variable_item_list_add(
        variable_item_list,
        "Send signal to",
        DEB_PINS_COUNT,
        infrared_scene_debug_settings_changed,
        infrared);

    variable_item_list_set_enter_callback(
        variable_item_list, infrared_debug_settings_start_var_list_enter_callback, infrared);

    variable_item_set_current_value_index(item, value_index_ir);
    variable_item_set_current_value_text(item, infrared_debug_cfg_variables_text[value_index_ir]);

    item = variable_item_list_add(
        variable_item_list,
        "Ext Module 5v",
        2,
        infrared_scene_debug_settings_power_changed,
        infrared);
    bool enabled = furi_hal_power_is_otg_enabled() ||
                   furi_hal_power_is_charging() || // 5v is enabled via hardware if charging
                   furi_hal_infrared_is_external_connected();
    variable_item_set_current_value_index(item, enabled);
    variable_item_set_current_value_text(item, enabled ? "ON" : "OFF");

    if(furi_hal_infrared_is_external_connected() && !furi_hal_power_is_otg_enabled()) {
        uint8_t attempts = 0;
        while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
            furi_hal_power_enable_otg();
            furi_delay_ms(10);
        }
    }

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewVariableItemList);
}

bool infrared_scene_debug_settings_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;
    UNUSED(infrared);
    UNUSED(event);

    return false;
}

void infrared_scene_debug_settings_on_exit(void* context) {
    InfraredApp* infrared = context;
    variable_item_list_reset(infrared->variable_item_list);
}
