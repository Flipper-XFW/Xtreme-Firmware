#include "../bad_ble_script.h"
#include "../bad_ble_app_i.h"
#include "../views/bad_ble_view.h"
#include "furi_hal.h"
#include "toolbox/path.h"

void bad_ble_scene_work_button_callback(InputKey key, void* context) {
    furi_assert(context);
    BadBleApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, key);
}

bool bad_ble_scene_work_on_event(void* context, SceneManagerEvent event) {
    BadBleApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == InputKeyLeft) {
            scene_manager_next_scene(app->scene_manager, BadBleSceneConfig);
            consumed = true;
        } else if(event.event == InputKeyOk) {
            bad_ble_script_toggle(app->bad_ble_script);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        bad_ble_set_state(app->bad_ble_view, bad_ble_script_get_state(app->bad_ble_script));
    }
    return consumed;
}

void bad_ble_scene_work_on_enter(void* context) {
    BadBleApp* app = context;

    FuriString* file_name;
    file_name = furi_string_alloc();
    path_extract_filename(app->file_path, file_name, true);
    bad_ble_set_file_name(app->bad_ble_view, furi_string_get_cstr(file_name));
    furi_string_free(file_name);

    FuriString* layout;
    layout = furi_string_alloc();
    path_extract_filename(app->keyboard_layout, layout, true);
    bad_ble_set_layout(app->bad_ble_view, furi_string_get_cstr(layout));
    furi_string_free(layout);

    bad_ble_set_state(app->bad_ble_view, bad_ble_script_get_state(app->bad_ble_script));

    bad_ble_set_button_callback(app->bad_ble_view, bad_ble_scene_work_button_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BadBleAppViewWork);
}

void bad_ble_scene_work_on_exit(void* context) {
    UNUSED(context);
}
