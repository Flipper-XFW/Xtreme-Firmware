#include "../desktop_settings_app.h"
// #include <storage/storage.h>

static void
    desktop_settings_scene_keybinds_reset_dialog_callback(DialogExResult result, void* context) {
    DesktopSettingsApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void desktop_settings_scene_keybinds_reset_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_header(dialog_ex, "Reset Desktop Keybinds?", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, "Your edits will be lost!", 64, 32, AlignCenter, AlignCenter);
    dialog_ex_set_left_button_text(dialog_ex, "Cancel");
    dialog_ex_set_right_button_text(dialog_ex, "Reset");

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(
        dialog_ex, desktop_settings_scene_keybinds_reset_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewDialogEx);
}

bool desktop_settings_scene_keybinds_reset_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultRight:
            storage_common_remove(furi_record_open(RECORD_STORAGE), DESKTOP_KEYBINDS_PATH);
            furi_record_close(RECORD_STORAGE);
            DESKTOP_KEYBINDS_LOAD(&app->desktop->keybinds, sizeof(app->desktop->keybinds));
            /* fall through */
        case DialogExResultLeft:
            consumed = scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void desktop_settings_scene_keybinds_reset_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);
}
