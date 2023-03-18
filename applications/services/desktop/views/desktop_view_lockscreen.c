#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <assets_icons.h>
#include <locale/locale.h>
#include <xtreme/assets.h>

#include "desktop_view_lockscreen.h"
#include "../desktop_i.h"

struct DesktopLockscreenView {
    View* view;
    void* context;
};

typedef struct {
    LocaleDateFormat date_format;
    LocaleTimeFormat time_format;
} DesktopLockscreenViewModel;

static void desktop_view_lockscreen_draw(Canvas* canvas, void* context) {
    DesktopLockscreenViewModel* model = context;
    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, XTREME_ASSETS()->I_Lockscreen);

    char time_str[9];
    char date_str[14];
    char meridian_str[3];
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);

    if(model->time_format == LocaleTimeFormat24h) {
        snprintf(time_str, 9, "%.2d:%.2d", datetime.hour, datetime.minute);
    } else {
        bool pm = datetime.hour > 12;
        bool pm12 = datetime.hour >= 12;
        snprintf(time_str, 9, "%.2d:%.2d", pm ? datetime.hour - 12 : datetime.hour, datetime.minute);
        snprintf(meridian_str, 3, pm12 ? "PM" : "AM");
    }

    if(model->date_format == LocaleDateFormatYMD) {
        snprintf(date_str, 14, "%.4d-%.2d-%.2d", datetime.year, datetime.month, datetime.day);
    } else if(model->date_format == LocaleDateFormatMDY) {
        snprintf(date_str, 14, "%.2d-%.2d-%.4d", datetime.month, datetime.day, datetime.year);
    } else {
        snprintf(date_str, 14, "%.2d-%.2d-%.4d", datetime.day, datetime.month, datetime.year);
    }

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str(canvas, 0, 64, time_str);
    int meridian_offset = canvas_string_width(canvas, time_str) + 2;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 48, date_str);
    if(model->time_format == LocaleTimeFormat12h) {
        canvas_draw_str(canvas, 0 + meridian_offset, 64, meridian_str);
    }
    canvas_set_font(canvas, FontBatteryPercent);
    canvas_draw_str_aligned(canvas, 64, 6, AlignCenter, AlignCenter, "UP to Unlock");
}

static bool desktop_view_lockscreen_input(InputEvent* event, void* context) {
    furi_assert(event);
    UNUSED(context);
    // DesktopLockscreenView* instance = context;

    // DesktopLockscreenViewModel* model = view_get_model(instance->view);
    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyOk:
        case InputKeyBack:
        default:
            break;
        }
    }
    // view_commit_model(instance->view, update_view);

    return true;
}

static void desktop_view_lockscreen_enter(void* context) {
    DesktopLockscreenView* instance = context;

    DesktopLockscreenViewModel* model = view_get_model(instance->view);
    model->time_format = locale_get_time_format();
    model->date_format = locale_get_date_format();
    view_commit_model(instance->view, false);
}

static void desktop_view_lockscreen_exit(void* context) {
    DesktopLockscreenView* instance = context;
    UNUSED(instance);

    // DesktopLockscreenViewModel* model = view_get_model(instance->view);
    // view_commit_model(instance->view, false);
}

DesktopLockscreenView* desktop_view_lockscreen_alloc() {
    DesktopLockscreenView* instance = malloc(sizeof(DesktopLockscreenView));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(DesktopLockscreenViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)desktop_view_lockscreen_draw);
    view_set_input_callback(instance->view, desktop_view_lockscreen_input);
    view_set_enter_callback(instance->view, desktop_view_lockscreen_enter);
    view_set_exit_callback(instance->view, desktop_view_lockscreen_exit);

    return instance;
}

void desktop_view_lockscreen_free(DesktopLockscreenView* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* desktop_view_lockscreen_get_view(DesktopLockscreenView* instance) {
    furi_assert(instance);
    return instance->view;
}
