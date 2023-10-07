#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <assets_icons.h>

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, 1);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "Weather Station moved!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 3, 24, "Now you can use the SubGhz");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 3, 35, "app from main menu, to catch");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 3, 46, "weather, pocsag and tpms");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 3, 57, "signals too. Press Back to Exit.");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

// Main entry of the application
int32_t weather_station_app(void* p) {
    UNUSED(p);

    Event event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(Event));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(true) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
        if(event.type == EventTypeKey) {
            if(event.input.key == InputKeyBack) {
                break;
            }
        }
    }

    furi_message_queue_free(event_queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}