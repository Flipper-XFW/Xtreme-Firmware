#include "../ble_spam.h"

#include "protocols/_protocols.h"

static void random_mac_changed(VariableItem* item) {
    Ctx* ctx = variable_item_get_context(item);
    ctx->attack->payload.random_mac = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, ctx->attack->payload.random_mac ? "ON" : "OFF");
}

static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    if(ctx->attack->protocol && ctx->attack->protocol->config_count) {
        uint8_t extra = ctx->attack->protocol->config_count(&ctx->attack->payload.cfg);
        if(index > extra) index -= extra;
    }

    switch(index) {
    case ConfigRandomMac:
        break;
    default:
        break;
    }
}
void scene_config_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    VariableItem* item;
    VariableItemList* list = ctx->variable_item_list;
    variable_item_list_reset(list);

    variable_item_list_set_header(list, ctx->attack->title);

    item = variable_item_list_add(list, "Random MAC", 2, random_mac_changed, ctx);
    variable_item_set_current_value_index(item, ctx->attack->payload.random_mac);
    variable_item_set_current_value_text(item, ctx->attack->payload.random_mac ? "ON" : "OFF");

    variable_item_list_set_enter_callback(list, config_callback, ctx);
    if(!ctx->attack->protocol) {
        variable_item_list_add(list, "None shall escape the S I N K", 0, NULL, NULL);
    } else if(ctx->attack->protocol->extra_config) {
        ctx->fallback_config_enter = config_callback;
        ctx->attack->protocol->extra_config(ctx);
    }

    variable_item_list_set_selected_item(
        list, scene_manager_get_scene_state(ctx->scene_manager, SceneConfig));

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewVariableItemList);
}

bool scene_config_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}

void scene_config_on_exit(void* _ctx) {
    UNUSED(_ctx);
}
