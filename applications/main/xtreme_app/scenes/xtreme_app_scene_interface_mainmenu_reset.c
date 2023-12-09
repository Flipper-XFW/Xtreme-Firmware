#include "../xtreme_app.h"

static void xtreme_app_scene_interface_mainmenu_reset_dialog_callback(
    DialogExResult result,
    void* context) {
    XtremeApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void xtreme_app_scene_interface_mainmenu_reset_on_enter(void* context) {
    XtremeApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_header(dialog_ex, "Reset Menu Apps?", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, "Your edits will be lost!", 64, 32, AlignCenter, AlignCenter);
    dialog_ex_set_left_button_text(dialog_ex, "Cancel");
    dialog_ex_set_right_button_text(dialog_ex, "Reset");

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(
        dialog_ex, xtreme_app_scene_interface_mainmenu_reset_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewDialogEx);
}

bool xtreme_app_scene_interface_mainmenu_reset_on_event(void* context, SceneManagerEvent event) {
    XtremeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultRight:
            storage_common_remove(furi_record_open(RECORD_STORAGE), XTREME_MENU_PATH);
            furi_record_close(RECORD_STORAGE);
            app->save_mainmenu_apps = false;
            app->require_reboot = true;
            xtreme_app_apply(app);
            break;
        case DialogExResultLeft:
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void xtreme_app_scene_interface_mainmenu_reset_on_exit(void* context) {
    XtremeApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);
}
