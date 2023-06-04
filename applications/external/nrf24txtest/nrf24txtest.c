#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_spi.h>
#include <gui/gui.h>

#include <nrf24.h>

#define appname "nrf24txtest"

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

typedef struct {
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    FuriString* buffer;
    bool isNRF24active;
    bool isNRF24connected;
    bool isSpectrumSweeping;
    bool connectedTone;
    FuriHalSpiBusHandle* nrf24handle;
    uint8_t channel;
} Context;

static void tick_callback(void* ctx_q) {
    furi_assert(ctx_q);
    FuriMessageQueue* queue = ctx_q;
    Event event = {.type = EventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* canvas, void* context) {
    Context* ctx = context;
    if(furi_mutex_acquire(ctx->mutex, 200) != FuriStatusOk) {
        return;
    }

    // hier teken je hoe je projectje eruit ziet

    if(ctx->isSpectrumSweeping) {
        canvas_draw_str(canvas, 10, 10, "NRF24 spectrum sweep");
    } else {
        canvas_draw_frame(canvas, 0, 0, 128, 64);

        canvas_draw_str(canvas, 10, 10, "NRF24 connected: ");

        if(ctx->isNRF24connected) {
            canvas_draw_str(canvas, 90, 10, "yes");
        } else {
            canvas_draw_str(canvas, 90, 10, "no");
        }

        canvas_draw_str(canvas, 10, 20, "NRF24 active: ");

        if(ctx->isNRF24active) {
            canvas_draw_str(canvas, 90, 20, "yes");
        } else {
            canvas_draw_str(canvas, 90, 20, "no");
        }

        furi_string_printf(ctx->buffer, "Channel: %d", ctx->channel);

        canvas_draw_str(canvas, 10, 30, furi_string_get_cstr(ctx->buffer));
    }

    furi_mutex_release(ctx->mutex);
}

void spectrum_sweep(FuriHalSpiBusHandle* handle) {
    furi_hal_light_set(LightRed, 255);

    for(uint8_t i = 0; i < 100; i++) {
        nrf24_start_testtx(i, handle);
        furi_delay_ms(50);
        nrf24_stop_testtx(handle);
    }

    furi_hal_light_set(LightRed, 0);
}

int32_t nrf24txtest_app(void* p) {
    UNUSED(p);

    // alloc everything

    Context* ctx = malloc(sizeof(Context));
    ctx->queue = furi_message_queue_alloc(8, sizeof(Event));
    ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ctx->buffer = furi_string_alloc();
    ctx->isNRF24active = false;
    ctx->isNRF24connected = false;
    ctx->isSpectrumSweeping = false;
    ctx->channel = 69;
    ctx->connectedTone = false;
    ctx->nrf24handle = &furi_hal_spi_bus_handle_external;

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, ctx);
    view_port_input_callback_set(view_port, input_callback, ctx->queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(!furi_hal_speaker_acquire(100)) {
        furi_delay_ms(100);
    }

    // timer to check if nrf24 is connected
    FuriTimer* timer = furi_timer_alloc(tick_callback, FuriTimerTypePeriodic, ctx->queue);
    furi_timer_start(timer, 500);

    nrf24_init(ctx->nrf24handle);
    nrf24_set_tx_mode(ctx->nrf24handle);

    // application loop
    Event event;
    bool processing = true;
    do {
        if(furi_message_queue_get(ctx->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
            case EventTypeKey:
                // applicatie verlaten
                if(event.input.type == InputTypeLong && event.input.key == InputKeyBack) {
                    processing = false;
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyOk) {
                    if(!nrf24_checkconnected(ctx->nrf24handle)) {
                        ctx->isNRF24active = false;
                    } else {
                        if(!ctx->isNRF24active) {
                            nrf24_start_testtx(ctx->channel, ctx->nrf24handle);
                            furi_hal_light_set(LightGreen, 255);
                        } else {
                            nrf24_stop_testtx(ctx->nrf24handle);
                            furi_hal_light_set(LightGreen, 0);
                        }
                        ctx->isNRF24active = !ctx->isNRF24active;
                    }
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyLeft) {
                    if(ctx->isNRF24active) {
                        nrf24_stop_testtx(ctx->nrf24handle);
                    }

                    if(ctx->channel == 127) {
                        ctx->channel = 0;
                    } else {
                        if(ctx->channel < 11) {
                            ctx->channel = 0;
                        } else {
                            ctx->channel = ctx->channel - 10;
                        }
                    }

                    if(ctx->isNRF24active) {
                        nrf24_start_testtx(ctx->channel, ctx->nrf24handle);
                    }
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyRight) {
                    if(ctx->isNRF24active) {
                        nrf24_stop_testtx(ctx->nrf24handle);
                    }

                    if(ctx->channel == 0) {
                        ctx->channel = 127;
                    } else {
                        if(ctx->channel > 116) {
                            ctx->channel = 127;
                        } else {
                            ctx->channel = ctx->channel + 10;
                        }
                    }

                    if(ctx->isNRF24active) {
                        nrf24_start_testtx(ctx->channel, ctx->nrf24handle);
                    }
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyLeft) {
                    if(ctx->isNRF24active) {
                        nrf24_stop_testtx(ctx->nrf24handle);
                    }

                    if(ctx->channel == 0) {
                        ctx->channel = 127;
                    } else {
                        ctx->channel--;
                    }

                    if(ctx->isNRF24active) {
                        nrf24_start_testtx(ctx->channel, ctx->nrf24handle);
                    }
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyRight) {
                    if(ctx->isNRF24active) {
                        nrf24_stop_testtx(ctx->nrf24handle);
                    }

                    if(ctx->channel == 127) {
                        ctx->channel = 0;
                    } else {
                        ctx->channel++;
                    }

                    if(ctx->isNRF24active) {
                        nrf24_start_testtx(ctx->channel, ctx->nrf24handle);
                    }

                } else if(event.input.type == InputTypePress && event.input.key == InputKeyUp) {
                    if(ctx->isNRF24connected) {
                        if(ctx->isNRF24active) {
                            nrf24_stop_testtx(ctx->nrf24handle);
                        }

                        if(!ctx->isSpectrumSweeping) {
                            ctx->isSpectrumSweeping = true;
                            view_port_update(view_port);

                            spectrum_sweep(ctx->nrf24handle);

                            ctx->isSpectrumSweeping = false;
                        }
                    } else {
                        furi_hal_speaker_start(100, 100);
                        furi_delay_ms(100);
                        furi_hal_speaker_stop();
                    }
                }
                break;
            case EventTypeTick:
                if(nrf24_checkconnected(ctx->nrf24handle)) {
                    if(!ctx->connectedTone) {
                        furi_hal_speaker_start(300, 100);
                        furi_delay_ms(50);
                        furi_hal_speaker_start(400, 100);
                        furi_delay_ms(50);
                        furi_hal_speaker_start(500, 100);
                    } else {
                        furi_hal_speaker_stop();
                    }

                    ctx->isNRF24connected = true;
                    ctx->connectedTone = true;

                    // furi_delay_ms(100);
                } else {
                    if(ctx->connectedTone) {
                        FURI_LOG_I(appname, "NRF24 disconnected");
                        furi_hal_light_set(LightBlue, 0);
                        furi_hal_speaker_start(500, 100);
                        furi_delay_ms(50);
                        furi_hal_speaker_start(400, 100);
                        furi_delay_ms(50);
                        furi_hal_speaker_start(300, 100);
                        ctx->connectedTone = false;
                    } else {
                        furi_hal_speaker_stop();
                    }

                    ctx->isNRF24connected = false;
                    ctx->isNRF24active = false;
                }
                break;
            default:
                break;
            }
            view_port_update(view_port);
        } else {
            processing = false;
        }
    } while(processing);

    // free everything

    if(ctx->isNRF24active) {
        nrf24_stop_testtx(ctx->nrf24handle);
    }

    nrf24_deinit(ctx->nrf24handle);

    furi_hal_speaker_release();

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(ctx->queue);
    furi_mutex_free(ctx->mutex);
    furi_timer_free(timer);
    free(ctx);

    return 0;
}
