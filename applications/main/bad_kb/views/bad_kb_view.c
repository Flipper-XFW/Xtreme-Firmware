#include "bad_kb_view.h"
#include "../helpers/ducky_script.h"
#include "../bad_kb_app.h"
#include <toolbox/path.h>
#include <gui/elements.h>
#include <assets_icons.h>
#include "xtreme/assets.h"

#define MAX_NAME_LEN 64

struct BadKb {
    View* view;
    BadKbButtonCallback callback;
    void* context;
};

typedef struct {
    char file_name[MAX_NAME_LEN];
    char layout[MAX_NAME_LEN];
    BadKbState state;
    uint8_t anim_frame;
} BadKbModel;

static void bad_kb_draw_callback(Canvas* canvas, void* _model) {
    BadKbModel* model = _model;

    FuriString* disp_str;
    disp_str = furi_string_alloc_set(model->state.is_bt ? "(BT) " : "(USB) ");
    furi_string_cat_str(disp_str, model->file_name);
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, furi_string_get_cstr(disp_str));

    if(strlen(model->layout) == 0) {
        furi_string_set(disp_str, "(default)");
    } else {
        furi_string_reset(disp_str);
        furi_string_push_back(disp_str, '(');
        for(size_t i = 0; i < strlen(model->layout); i++)
            furi_string_push_back(disp_str, model->layout[i]);
        furi_string_push_back(disp_str, ')');
    }
    if(model->state.pin) {
        furi_string_cat_printf(disp_str, "  PIN: %ld", model->state.pin);
    }
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_draw_str(
        canvas, 2, 8 + canvas_current_font_height(canvas), furi_string_get_cstr(disp_str));

    furi_string_reset(disp_str);

    canvas_draw_icon(canvas, 22, 24, &I_UsbTree_48x22);

    if((model->state.state == BadKbStateIdle) || (model->state.state == BadKbStateDone) ||
       (model->state.state == BadKbStateNotConnected)) {
        if(XTREME_ASSETS()->is_nsfw) {
            elements_button_center(canvas, "Cum");
        } else {
            elements_button_center(canvas, "Run");
        }
        elements_button_left(canvas, "Config");
    } else if((model->state.state == BadKbStateRunning) || (model->state.state == BadKbStateDelay)) {
        elements_button_center(canvas, "Stop");
    } else if(model->state.state == BadKbStateWaitForBtn) {
        elements_button_center(canvas, "Press to continue");
    } else if(model->state.state == BadKbStateWillRun) {
        elements_button_center(canvas, "Cancel");
    }

    if(model->state.state == BadKbStateNotConnected) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        if(XTREME_ASSETS()->is_nsfw) {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Plug me");
            canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "in, Daddy");
        } else {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Connect to");
            canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "a device");
        }
    } else if(model->state.state == BadKbStateWillRun) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        if(XTREME_ASSETS()->is_nsfw) {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Will cum");
        } else {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Will run");
        }
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "on connect");
    } else if(model->state.state == BadKbStateFileError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "File");
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "ERROR");
    } else if(model->state.state == BadKbStateScriptError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 33, AlignRight, AlignBottom, "ERROR:");
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(disp_str, "line %u", model->state.error_line);
        canvas_draw_str_aligned(
            canvas, 127, 46, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        furi_string_set_str(disp_str, model->state.error);
        elements_string_fit_width(canvas, disp_str, canvas_width(canvas));
        canvas_draw_str_aligned(
            canvas, 127, 56, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
    } else if(model->state.state == BadKbStateIdle) {
        canvas_draw_icon(canvas, 4, 26, &I_Smile_18x18);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 40, AlignRight, AlignBottom, "0");
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
    } else if(model->state.state == BadKbStateRunning) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 23, &I_EviSmile1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 23, &I_EviSmile2_18x21);
        }
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%u", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 114, 40, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
    } else if(model->state.state == BadKbStateDone) {
        canvas_draw_icon(canvas, 4, 23, &I_EviSmile1_18x21);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 40, AlignRight, AlignBottom, "100");
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
    } else if(model->state.state == BadKbStateDelay) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 23, &I_EviWaiting2_18x21);
        }
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%u", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 114, 40, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(disp_str, "delay %lus", model->state.delay_remain);
        canvas_draw_str_aligned(
            canvas, 127, 50, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
    } else {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
    }

    furi_string_free(disp_str);
}

static bool bad_kb_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BadKb* bad_kb = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if((event->key == InputKeyLeft) || (event->key == InputKeyOk)) {
            consumed = true;
            furi_assert(bad_kb->callback);
            bad_kb->callback(event->key, bad_kb->context);
        }
    }

    return consumed;
}

BadKb* bad_kb_alloc() {
    BadKb* bad_kb = malloc(sizeof(BadKb));

    bad_kb->view = view_alloc();
    view_allocate_model(bad_kb->view, ViewModelTypeLocking, sizeof(BadKbModel));
    view_set_context(bad_kb->view, bad_kb);
    view_set_draw_callback(bad_kb->view, bad_kb_draw_callback);
    view_set_input_callback(bad_kb->view, bad_kb_input_callback);

    return bad_kb;
}

void bad_kb_free(BadKb* bad_kb) {
    furi_assert(bad_kb);
    view_free(bad_kb->view);
    free(bad_kb);
}

View* bad_kb_get_view(BadKb* bad_kb) {
    furi_assert(bad_kb);
    return bad_kb->view;
}

void bad_kb_set_button_callback(BadKb* bad_kb, BadKbButtonCallback callback, void* context) {
    furi_assert(bad_kb);
    furi_assert(callback);
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            UNUSED(model);
            bad_kb->callback = callback;
            bad_kb->context = context;
        },
        true);
}

void bad_kb_set_file_name(BadKb* bad_kb, const char* name) {
    furi_assert(name);
    with_view_model(
        bad_kb->view, BadKbModel * model, { strlcpy(model->file_name, name, MAX_NAME_LEN); }, true);
}

void bad_kb_set_layout(BadKb* bad_kb, const char* layout) {
    furi_assert(layout);
    with_view_model(
        bad_kb->view, BadKbModel * model, { strlcpy(model->layout, layout, MAX_NAME_LEN); }, true);
}

void bad_kb_set_state(BadKb* bad_kb, BadKbState* st) {
    furi_assert(st);
    uint32_t pin = 0;
    if(bad_kb->context != NULL) {
        BadKbApp* app = bad_kb->context;
        if(app->bt != NULL) {
            pin = app->bt->pin;
        }
    }
    st->pin = pin;
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            memcpy(&(model->state), st, sizeof(BadKbState));
            model->anim_frame ^= 1;
        },
        true);
}

bool bad_kb_is_idle_state(BadKb* bad_kb) {
    bool is_idle = false;
    with_view_model(
        bad_kb->view,
        BadKbModel * model,
        {
            if((model->state.state == BadKbStateIdle) || (model->state.state == BadKbStateDone) ||
               (model->state.state == BadKbStateNotConnected)) {
                is_idle = true;
            }
        },
        false);
    return is_idle;
}
