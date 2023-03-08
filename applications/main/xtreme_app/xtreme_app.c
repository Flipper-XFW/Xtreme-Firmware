#include "xtreme_app.h"

static bool xtreme_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    XtremeApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void callback_reboot(void* context) {
    UNUSED(context);
    power_reboot(PowerBootModeNormal);
}

static bool xtreme_app_back_event_callback(void* context) {
    furi_assert(context);
    XtremeApp* app = context;

    if(!scene_manager_has_previous_scene(app->scene_manager, XtremeAppSceneStart)) {
        Storage* storage = furi_record_open(RECORD_STORAGE);

        if(app->save_subghz) {
            furi_hal_subghz_set_extend_settings(app->subghz_extend, app->subghz_bypass);
        }

        if(app->save_subghz_frequencies) {
            FlipperFormat* file = flipper_format_file_alloc(storage);
            do {
                FrequencyList_it_t it;
                if(!flipper_format_file_open_always(file, EXT_PATH("subghz/assets/setting_user")))
                    break;

                if(!flipper_format_write_header_cstr(
                       file, SUBGHZ_SETTING_FILE_TYPE, SUBGHZ_SETTING_FILE_VERSION))
                    break;

                while(flipper_format_delete_key(file, "Add_standard_frequencies"))
                    ;
                flipper_format_write_bool(
                    file, "Add_standard_frequencies", &app->subghz_use_defaults, 1);

                if(!flipper_format_rewind(file)) break;
                while(flipper_format_delete_key(file, "Frequency"))
                    ;
                FrequencyList_it(it, app->subghz_static_frequencies);
                for(uint i = 0; i < FrequencyList_size(app->subghz_static_frequencies); i++) {
                    flipper_format_write_uint32(
                        file, "Frequency", FrequencyList_get(app->subghz_static_frequencies, i), 1);
                }

                if(!flipper_format_rewind(file)) break;
                while(flipper_format_delete_key(file, "Hopper_frequency"))
                    ;
                for(uint i = 0; i < FrequencyList_size(app->subghz_hopper_frequencies); i++) {
                    flipper_format_write_uint32(
                        file,
                        "Hopper_frequency",
                        FrequencyList_get(app->subghz_hopper_frequencies, i),
                        1);
                }
            } while(false);
            flipper_format_free(file);
        }

        if(app->save_level) {
            Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
            int xp = app->dolphin_level > 1 ? dolphin_get_levels()[app->dolphin_level - 2] : 0;
            dolphin->state->data.icounter = xp + 1;
            dolphin->state->dirty = true;
            dolphin_state_save(dolphin->state);
            furi_record_close(RECORD_DOLPHIN);
        }

        if(app->save_name) {
            if(strcmp(app->device_name, "") == 0) {
                storage_simply_remove(storage, NAMECHANGER_PATH);
            } else {
                FlipperFormat* file = flipper_format_file_alloc(storage);

                do {
                    if(!flipper_format_file_open_always(file, NAMECHANGER_PATH)) break;

                    if(!flipper_format_write_header_cstr(file, NAMECHANGER_HEADER, 1)) break;

                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Changing the value below will change your FlipperZero device name."))
                        break;
                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _"))
                        break;
                    if(!flipper_format_write_comment_cstr(
                           file, "It cannot contain any other characters."))
                        break;

                    if(!flipper_format_write_string_cstr(file, "Name", app->device_name)) break;

                } while(0);

                flipper_format_free(file);
            }
        }

        if(app->save_settings) {
            XTREME_SETTINGS_SAVE();
        }

        if(app->require_reboot) {
            popup_set_header(app->popup, "Rebooting...", 64, 26, AlignCenter, AlignCenter);
            popup_set_text(app->popup, "Applying changes...", 64, 40, AlignCenter, AlignCenter);
            popup_set_callback(app->popup, callback_reboot);
            popup_set_context(app->popup, app);
            popup_set_timeout(app->popup, 1000);
            popup_enable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewPopup);
            return true;
        }

        furi_record_close(RECORD_STORAGE);
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

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XtremeAppViewTextInput, text_input_get_view(app->text_input));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, XtremeAppViewPopup, popup_get_view(app->popup));

    // Settings init

    XtremeSettings* xtreme_settings = XTREME_SETTINGS();

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    FrequencyList_init(app->subghz_static_frequencies);
    FrequencyList_init(app->subghz_hopper_frequencies);
    app->subghz_use_defaults = true;
    do {
        uint32_t temp;
        if(!flipper_format_file_open_existing(file, EXT_PATH("subghz/assets/setting_user"))) break;

        flipper_format_read_bool(file, "Add_standard_frequencies", &app->subghz_use_defaults, 1);

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_static_frequencies, temp);
            }
        }

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Hopper_frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_hopper_frequencies, temp);
            }
        }
    } while(false);
    flipper_format_free(file);

    furi_hal_subghz_get_extend_settings(&app->subghz_extend, &app->subghz_bypass);

    Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
    DolphinStats stats = dolphin_stats(dolphin);
    app->dolphin_level = stats.level;
    furi_record_close(RECORD_DOLPHIN);

    strlcpy(app->device_name, furi_hal_version_get_name_ptr(), NAMECHANGER_TEXT_STORE_SIZE);

    app->asset_pack = 0;
    asset_packs_init(app->asset_packs);
    File* folder = storage_file_alloc(storage);
    FileInfo info;
    char* name = malloc(MAX_PACK_NAME_LEN);
    if(storage_dir_open(folder, PACKS_DIR)) {
        while(storage_dir_read(folder, &info, name, MAX_PACK_NAME_LEN)) {
            if(info.flags & FSF_DIRECTORY) {
                char* copy = malloc(MAX_PACK_NAME_LEN);
                strlcpy(copy, name, MAX_PACK_NAME_LEN);
                uint idx = 0;
                if(strcmp(copy, "NSFW") != 0) {
                    for(; idx < asset_packs_size(app->asset_packs); idx++) {
                        char* comp = *asset_packs_get(app->asset_packs, idx);
                        if(strcasecmp(copy, comp) < 0 && strcmp(comp, "NSFW") != 0) {
                            break;
                        }
                    }
                }
                asset_packs_push_at(app->asset_packs, idx, copy);
                if(app->asset_pack != 0) {
                    if(idx < app->asset_pack) app->asset_pack++;
                } else {
                    if(strcmp(copy, xtreme_settings->asset_pack) == 0) app->asset_pack = idx + 1;
                }
            }
        }
    }
    free(name);
    storage_file_free(folder);
    furi_record_close(RECORD_STORAGE);

    app->version_tag =
        furi_string_alloc_printf("%s  %s", version_get_version(NULL), version_get_builddate(NULL));

    return app;
}

void xtreme_app_free(XtremeApp* app) {
    furi_assert(app);

    // Gui modules
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewVarItemList);
    variable_item_list_free(app->var_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewTextInput);
    text_input_free(app->text_input);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewPopup);
    popup_free(app->popup);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Settings deinit

    FrequencyList_clear(app->subghz_static_frequencies);
    FrequencyList_clear(app->subghz_hopper_frequencies);

    asset_packs_it_t it;
    for(asset_packs_it(it, app->asset_packs); !asset_packs_end_p(it); asset_packs_next(it)) {
        free(*asset_packs_cref(it));
    }
    asset_packs_clear(app->asset_packs);

    furi_string_free(app->version_tag);

    // Records
    furi_record_close(RECORD_GUI);
    free(app);
}

extern int32_t xtreme_app(void* p) {
    UNUSED(p);
    XtremeApp* app = xtreme_app_alloc();
    scene_manager_next_scene(app->scene_manager, XtremeAppSceneStart);
    view_dispatcher_run(app->view_dispatcher);
    xtreme_app_free(app);
    return 0;
}
