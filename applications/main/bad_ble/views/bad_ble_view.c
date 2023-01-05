#include "bad_ble_view.h"
#include "../bad_ble_script.h"
#include <toolbox/path.h>
#include <gui/elements.h>
#include <bad_ble_icons.h>
#include "../../../settings/desktop_settings/desktop_settings_app.h"

#define MAX_NAME_LEN 64

struct BadBle {
    View* view;
    BadBleButtonCallback callback;
    void* context;
};

typedef struct {
    char file_name[MAX_NAME_LEN];
    char layout[MAX_NAME_LEN];
    BadBleState state;
    uint8_t anim_frame;
} BadBleModel;

static void bad_ble_draw_callback(Canvas* canvas, void* _model) {
    BadBleModel* model = _model;

    FuriString* disp_str;
    disp_str = furi_string_alloc_set(model->file_name);
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, furi_string_get_cstr(disp_str));
    DesktopSettings* settings = malloc(sizeof(DesktopSettings));
    DESKTOP_SETTINGS_LOAD(settings);

    if(strlen(model->layout) == 0) {
        furi_string_set(disp_str, "(default)");
    } else {
        furi_string_reset(disp_str);
        furi_string_push_back(disp_str, '(');
        for(size_t i = 0; i < strlen(model->layout); i++)
            furi_string_push_back(disp_str, model->layout[i]);
        furi_string_push_back(disp_str, ')');
    }
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_draw_str(
        canvas, 2, 8 + canvas_current_font_height(canvas), furi_string_get_cstr(disp_str));

    furi_string_reset(disp_str);

    canvas_draw_icon(canvas, 22, 24, &I_UsbTree_48x22);

    if((model->state.state == BadBleStateIdle) || (model->state.state == BadBleStateDone) ||
       (model->state.state == BadBleStateNotConnected)) {
        if (settings->sfw_mode) {
            elements_button_center(canvas, "Start");
        }
        else {
            elements_button_center(canvas, "Cum");
        }
    } else if((model->state.state == BadBleStateRunning) || (model->state.state == BadBleStateDelay)) {
        elements_button_center(canvas, "Stop");
    } else if(model->state.state == BadBleStateWillRun) {
        elements_button_center(canvas, "Cancel");
    }

    if((model->state.state == BadBleStateNotConnected) ||
       (model->state.state == BadBleStateIdle) || (model->state.state == BadBleStateDone)) {
        elements_button_left(canvas, "Config");
    }

    if(model->state.state == BadBleStateNotConnected) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        if (settings->sfw_mode) {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Connect me");
            canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "to a computer");
        }
        else {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Plug me");
            canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "in, Daddy");
        }
    } else if(model->state.state == BadBleStateWillRun) {
        canvas_draw_icon(canvas, 4, 26, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        if (settings->sfw_mode) {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Will run");
        }
        else {
            canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "Will cum");
        }
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "on connect");
    } else if(model->state.state == BadBleStateFileError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 31, AlignRight, AlignBottom, "File");
        canvas_draw_str_aligned(canvas, 127, 43, AlignRight, AlignBottom, "ERROR");
    } else if(model->state.state == BadBleStateScriptError) {
        canvas_draw_icon(canvas, 4, 26, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 33, AlignRight, AlignBottom, "ERROR:");
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(disp_str, "line %u", model->state.error_line);
        canvas_draw_str_aligned(
            canvas, 127, 46, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_str_aligned(canvas, 127, 56, AlignRight, AlignBottom, model->state.error);
    } else if(model->state.state == BadBleStateIdle) {
        canvas_draw_icon(canvas, 4, 26, &I_Smile_18x18);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 40, AlignRight, AlignBottom, "0");
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
    } else if(model->state.state == BadBleStateRunning) {
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
    } else if(model->state.state == BadBleStateDone) {
        canvas_draw_icon(canvas, 4, 23, &I_EviSmile1_18x21);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 40, AlignRight, AlignBottom, "100");
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 26, &I_Percent_10x14);
    } else if(model->state.state == BadBleStateDelay) {
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
    free(settings);
}

static bool bad_ble_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BadBle* bad_ble = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if((event->key == InputKeyLeft) || (event->key == InputKeyOk)) {
            consumed = true;
            furi_assert(bad_ble->callback);
            bad_ble->callback(event->key, bad_ble->context);
        }
    }

    return consumed;
}

BadBle* bad_ble_alloc() {
    BadBle* bad_ble = malloc(sizeof(BadBle));

    bad_ble->view = view_alloc();
    view_allocate_model(bad_ble->view, ViewModelTypeLocking, sizeof(BadBleModel));
    view_set_context(bad_ble->view, bad_ble);
    view_set_draw_callback(bad_ble->view, bad_ble_draw_callback);
    view_set_input_callback(bad_ble->view, bad_ble_input_callback);

    return bad_ble;
}

void bad_ble_free(BadBle* bad_ble) {
    furi_assert(bad_ble);
    view_free(bad_ble->view);
    free(bad_ble);
}

View* bad_ble_get_view(BadBle* bad_ble) {
    furi_assert(bad_ble);
    return bad_ble->view;
}

void bad_ble_set_button_callback(BadBle* bad_ble, BadBleButtonCallback callback, void* context) {
    furi_assert(bad_ble);
    furi_assert(callback);
    with_view_model(
        bad_ble->view,
        BadBleModel * model,
        {
            UNUSED(model);
            bad_ble->callback = callback;
            bad_ble->context = context;
        },
        true);
}

void bad_ble_set_file_name(BadBle* bad_ble, const char* name) {
    furi_assert(name);
    with_view_model(
        bad_ble->view,
        BadBleModel * model,
        { strlcpy(model->file_name, name, MAX_NAME_LEN); },
        true);
}

void bad_ble_set_layout(BadBle* bad_ble, const char* layout) {
    furi_assert(layout);
    with_view_model(
        bad_ble->view,
        BadBleModel * model,
        { strlcpy(model->layout, layout, MAX_NAME_LEN); },
        true);
}

void bad_ble_set_state(BadBle* bad_ble, BadBleState* st) {
    furi_assert(st);
    with_view_model(
        bad_ble->view,
        BadBleModel * model,
        {
            memcpy(&(model->state), st, sizeof(BadBleState));
            model->anim_frame ^= 1;
        },
        true);
}
