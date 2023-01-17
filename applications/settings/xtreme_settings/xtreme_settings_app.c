#include "xtreme_settings_app.h"

static bool xtreme_settings_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    XtremeSettingsApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool xtreme_settings_back_event_callback(void* context) {
    furi_assert(context);
    XtremeSettingsApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

XtremeSettingsApp* xtreme_settings_app_alloc() {
    XtremeSettingsApp* app = malloc(sizeof(XtremeSettingsApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->dolphin = furi_record_open(RECORD_DOLPHIN);
    app->dolphin_stats = dolphin_stats(app->dolphin);

    // View Dispatcher and Scene Manager
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&xtreme_settings_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, xtreme_settings_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, xtreme_settings_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Gui Modules
    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XtremeSettingsAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    // Set first scene
    scene_manager_next_scene(app->scene_manager, XtremeSettingsAppSceneStart);
    return app;
}

void xtreme_settings_app_free(XtremeSettingsApp* app) {
    furi_assert(app);

    // Gui modules
    view_dispatcher_remove_view(app->view_dispatcher, XtremeSettingsAppViewVarItemList);
    variable_item_list_free(app->var_item_list);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_DOLPHIN);
    free(app);
}

extern int32_t xtreme_settings_app(void* p) {
    UNUSED(p);
    XtremeSettingsApp* app = xtreme_settings_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    xtreme_settings_app_free(app);
    return 0;
}
