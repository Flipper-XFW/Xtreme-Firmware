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

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            main_view->callback(DesktopMainEventOpenMenu, main_view->context);
        } else if(event->key == InputKeyUp) {
            main_view->callback(DesktopMainEventOpenLockMenu, main_view->context);
        } else if(event->key == InputKeyDown) {
            main_view->callback(DesktopMainEventOpenArchive, main_view->context);
        } else if(event->key == InputKeyRight) {
            Loader* loader = furi_record_open(RECORD_LOADER);
            loader_start(loader, "Passport", NULL, NULL);
            furi_record_close(RECORD_LOADER);
        } else if(event->key == InputKeyLeft) {
            main_view->callback(DesktopMainEventOpenClock, main_view->context);
        }
        // Right key is handled by animation manager
    } else if(event->type == InputTypeLong) {
        if(event->key == InputKeyOk) {
            main_view->callback(DesktopAnimationEventNewIdleAnimation, main_view->context);
        } else if(event->key == InputKeyUp) {
            main_view->callback(DesktopMainEventOpenFavoritePrimary, main_view->context);
        } else if(event->key == InputKeyDown) {
            main_view->callback(DesktopMainEventOpenFavoriteSecondary, main_view->context);
        } else if(event->key == InputKeyRight) {
            Loader* loader = furi_record_open(RECORD_LOADER);
            loader_start(loader, "Power", "about_battery", NULL);
            furi_record_close(RECORD_LOADER);
        } else if(event->key == InputKeyLeft) {
            main_view->callback(DesktopMainEventLock, main_view->context);
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
