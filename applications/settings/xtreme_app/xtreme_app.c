#include "xtreme_app.h"

static bool xtreme_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    XtremeApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void xtreme_app_reboot(void* context) {
    UNUSED(context);
    power_reboot(PowerBootModeNormal);
}

static bool xtreme_app_back_event_callback(void* context) {
    furi_assert(context);
    XtremeApp* app = context;

    if(app->level_changed) {
        Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
        DolphinStats stats = dolphin_stats(dolphin);
        if(app->dolphin_level != stats.level) {
            int xp = app->dolphin_level > 1 ? dolphin_get_levels()[app->dolphin_level - 2] : 0;
            dolphin->state->data.icounter = xp + 1;
            dolphin->state->dirty = true;
            dolphin_state_save(dolphin->state);
        }
        furi_record_close(RECORD_DOLPHIN);
    }

    if(app->subghz_changed) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        FlipperFormat* subghz_range = flipper_format_file_alloc(storage);
        if(flipper_format_file_open_existing(subghz_range, "/ext/subghz/assets/extend_range.txt")) {
            flipper_format_insert_or_update_bool(
                subghz_range, "use_ext_range_at_own_risk", &app->subghz_extend, 1);
            flipper_format_insert_or_update_bool(
                subghz_range, "ignore_default_tx_region", &app->subghz_bypass, 1);
        }
        flipper_format_free(subghz_range);
        furi_record_close(RECORD_STORAGE);
    }

    if(app->settings_changed) {
        XTREME_SETTINGS_SAVE();
        if(app->assets_changed) {
            popup_set_header(app->popup, "Rebooting...", 64, 26, AlignCenter, AlignCenter);
            popup_set_text(app->popup, "Swapping assets...", 64, 40, AlignCenter, AlignCenter);
            popup_set_callback(app->popup, xtreme_app_reboot);
            popup_set_context(app->popup, app);
            popup_set_timeout(app->popup, 1000);
            popup_enable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewPopup);
            return true;
        }
    }

    return scene_manager_handle_back_event(app->scene_manager);
}

XtremeApp* xtreme_app_alloc() {
    XtremeApp* app = malloc(sizeof(XtremeApp));
    app->gui = furi_record_open(RECORD_GUI);

    // View Dispatcher and Scene Manager
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&xtreme_app_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, xtreme_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, xtreme_app_back_event_callback);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Gui Modules
    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        XtremeAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, XtremeAppViewPopup, popup_get_view(app->popup));

    return app;
}

void xtreme_app_free(XtremeApp* app) {
    furi_assert(app);

    // Gui modules
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewVarItemList);
    variable_item_list_free(app->var_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewPopup);
    popup_free(app->popup);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Records
    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t xtreme_app(void* p) {
    UNUSED(p);
    XtremeApp* app = xtreme_app_alloc();
    scene_manager_next_scene(app->scene_manager, XtremeAppSceneMain);
    view_dispatcher_run(app->view_dispatcher);
    xtreme_app_free(app);
    return 0;
}
