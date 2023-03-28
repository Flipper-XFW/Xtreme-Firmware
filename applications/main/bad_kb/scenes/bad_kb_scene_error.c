#include "../bad_kb_app.h"
#include "xtreme/assets.h"

static void
    bad_kb_scene_error_event_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    BadKbApp* app = context;

    if((result == GuiButtonTypeLeft) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(app->view_dispatcher, BadKbCustomEventErrorBack);
    }
}

void bad_kb_scene_error_on_enter(void* context) {
    BadKbApp* app = context;

    if(app->error == BadKbAppErrorNoFiles) {
        widget_add_icon_element(app->widget, 0, 0, &I_SDQuestion_35x43);
        widget_add_string_multiline_element(
            app->widget,
            81,
            4,
            AlignCenter,
            AlignTop,
            FontSecondary,
            "No SD card or\napp data found.\nThis app will not\nwork without\nrequired files.");
        widget_add_button_element(
            app->widget, GuiButtonTypeLeft, "Back", bad_kb_scene_error_event_callback, app);
    } else if(app->error == BadKbAppErrorCloseRpc) {
        widget_add_icon_element(app->widget, 78, 0, &I_ActiveConnection_50x64);
        if(XTREME_ASSETS()->is_nsfw) {
            widget_add_string_multiline_element(
                app->widget, 3, 2, AlignLeft, AlignTop, FontPrimary, "I am not\na whore!");
            widget_add_string_multiline_element(
                app->widget,
                3,
                30,
                AlignLeft,
                AlignTop,
                FontSecondary,
                "Pull out from\nPC or phone to\nuse me like this.");
        } else {
            widget_add_string_multiline_element(
                app->widget, 3, 2, AlignLeft, AlignTop, FontPrimary, "Connection\nis active!");
            widget_add_string_multiline_element(
                app->widget,
                3,
                30,
                AlignLeft,
                AlignTop,
                FontSecondary,
                "Disconnect from\nPC or phone to\nuse this function.");
        }
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, BadKbAppViewError);
}

bool bad_kb_scene_error_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == BadKbCustomEventErrorBack) {
            view_dispatcher_stop(app->view_dispatcher);
            consumed = true;
        }
    }
    return consumed;
}

void bad_kb_scene_error_on_exit(void* context) {
    BadKbApp* app = context;
    widget_reset(app->widget);
}
