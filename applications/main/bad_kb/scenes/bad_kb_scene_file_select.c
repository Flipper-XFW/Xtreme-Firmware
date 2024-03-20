#include "../bad_kb_app_i.h"
#include <furi_hal_power.h>
#include <furi_hal_usb.h>
#include <storage/storage.h>

static bool bad_kb_file_select(BadKbApp* bad_kb) {
    furi_assert(bad_kb);

    bad_kb_app_show_loading_popup(bad_kb, true);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(storage_dir_exists(storage, EXT_PATH("badkb"))) {
        DialogMessage* message = dialog_message_alloc();
        dialog_message_set_header(message, "Migrate Scripts", 64, 0, AlignCenter, AlignTop);
        dialog_message_set_buttons(message, NULL, NULL, "Ok");
        dialog_message_set_text(
            message,
            "Scripts will be moved to\n"
            "'badusb' folder\n"
            "for compatibility.",
            64,
            32,
            AlignCenter,
            AlignCenter);
        DialogMessageButton res = dialog_message_show(furi_record_open(RECORD_DIALOGS), message);
        dialog_message_free(message);
        furi_record_close(RECORD_DIALOGS);
        if(res == DialogMessageButtonRight) {
            storage_common_migrate(storage, EXT_PATH("badkb"), BAD_KB_APP_BASE_FOLDER);
            if(bad_kb->conn_init_thread) {
                furi_thread_join(bad_kb->conn_init_thread);
            }
            bad_kb_load_settings(bad_kb);
            bad_kb_config_adjust(&bad_kb->config);
        }
    }
    storage_simply_mkdir(storage, BAD_KB_APP_BASE_FOLDER);
    furi_record_close(RECORD_STORAGE);
    bad_kb_app_show_loading_popup(bad_kb, false);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, BAD_KB_APP_SCRIPT_EXTENSION, &I_badkb_10px);
    browser_options.base_path = BAD_KB_APP_BASE_FOLDER;
    browser_options.skip_assets = true;

    // Input events and views are managed by file_browser
    bool res = dialog_file_browser_show(
        bad_kb->dialogs, bad_kb->file_path, bad_kb->file_path, &browser_options);

    return res;
}

void bad_kb_scene_file_select_on_enter(void* context) {
    BadKbApp* bad_kb = context;

    if(bad_kb->bad_kb_script) {
        bad_kb_script_close(bad_kb->bad_kb_script);
        bad_kb->bad_kb_script = NULL;
    }

    if(bad_kb_file_select(bad_kb)) {
        bad_kb->bad_kb_script =
            bad_kb_script_open(bad_kb->file_path, bad_kb->is_bt ? bad_kb->bt : NULL, bad_kb);
        bad_kb_script_set_keyboard_layout(bad_kb->bad_kb_script, bad_kb->keyboard_layout);

        scene_manager_next_scene(bad_kb->scene_manager, BadKbSceneWork);
    } else {
        view_dispatcher_stop(bad_kb->view_dispatcher);
    }
}

bool bad_kb_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void bad_kb_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
