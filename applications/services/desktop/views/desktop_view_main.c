#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/elements.h>
#include <gui/canvas.h>
#include <furi.h>
#include <input/input.h>
#include <dolphin/dolphin.h>

#include "../desktop_i.h"
#include "desktop_view_main.h"

struct DesktopMainView {
    View* view;
    DesktopMainViewCallback callback;
    void* context;
    FuriTimer* _poweroff_timer; // Unused, kept for compatibility
    bool _dummy_mode; // Unused, kept for compatibility
};

void desktop_main_set_callback(
    DesktopMainView* main_view,
    DesktopMainViewCallback callback,
    void* context) {
    furi_assert(main_view);
    furi_assert(callback);
    main_view->callback = callback;
    main_view->context = context;
}

View* desktop_main_get_view(DesktopMainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

bool desktop_main_input_callback(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DesktopMainView* main_view = context;

    if(event->type == InputTypeShort || event->type == InputTypeLong) {
        if(event->key == InputKeyOk) {
            main_view->callback(
                event->type == InputTypeShort ? DesktopMainEventOpenMenu :
                                                DesktopAnimationEventNewIdleAnimation,
                main_view->context);
        } else {
            desktop_run_keybind((Desktop*)main_view->context, event->type, event->key);
        }
    }

    if(event->key == InputKeyBack) {
        if(event->type == InputTypeLong) {
            main_view->callback(DesktopMainEventOpenPowerOff, main_view->context);
        }
    }

    return true;
}

DesktopMainView* desktop_main_alloc() {
    DesktopMainView* main_view = malloc(sizeof(DesktopMainView));

    main_view->view = view_alloc();
    view_set_context(main_view->view, main_view);
    view_set_input_callback(main_view->view, desktop_main_input_callback);

    return main_view;
}

void desktop_main_free(DesktopMainView* main_view) {
    furi_assert(main_view);
    view_free(main_view->view);
    free(main_view);
}
