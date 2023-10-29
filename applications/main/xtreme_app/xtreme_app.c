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

bool xtreme_app_apply(XtremeApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(app->save_mainmenu_apps) {
        Stream* stream = file_stream_alloc(storage);
        if(file_stream_open(stream, XTREME_MENU_PATH, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
            stream_write_format(stream, "MenuAppList Version %u\n", 0);
            CharList_it_t it;
            CharList_it(it, app->mainmenu_app_exes);
            for(size_t i = 0; i < CharList_size(app->mainmenu_app_exes); i++) {
                stream_write_format(stream, "%s\n", *CharList_get(app->mainmenu_app_exes, i));
            }
        }
        file_stream_close(stream);
        stream_free(stream);
    }

    if(app->save_subghz_freqs) {
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
            FrequencyList_it(it, app->subghz_static_freqs);
            for(uint i = 0; i < FrequencyList_size(app->subghz_static_freqs); i++) {
                flipper_format_write_uint32(
                    file, "Frequency", FrequencyList_get(app->subghz_static_freqs, i), 1);
            }

            if(!flipper_format_rewind(file)) break;
            while(flipper_format_delete_key(file, "Hopper_frequency"))
                ;
            for(uint i = 0; i < FrequencyList_size(app->subghz_hopper_freqs); i++) {
                flipper_format_write_uint32(
                    file, "Hopper_frequency", FrequencyList_get(app->subghz_hopper_freqs, i), 1);
            }
        } while(false);
        flipper_format_free(file);
    }

    if(app->save_subghz) {
        FlipperFormat* file = flipper_format_file_alloc(storage);
        do {
            if(!flipper_format_file_open_always(file, "/ext/subghz/assets/extend_range.txt"))
                break;
            if(!flipper_format_write_header_cstr(file, "Flipper SubGhz Setting File", 1)) break;
            if(!flipper_format_write_comment_cstr(
                   file, "Whether to allow extended ranges that can break your flipper"))
                break;
            if(!flipper_format_write_bool(
                   file, "use_ext_range_at_own_risk", &app->subghz_extend, 1))
                break;
        } while(0);
        flipper_format_free(file);
    }

    if(app->save_name) {
        if(strcmp(app->device_name, "") == 0) {
            storage_simply_remove(storage, NAMESPOOF_PATH);
        } else {
            FlipperFormat* file = flipper_format_file_alloc(storage);

            do {
                if(!flipper_format_file_open_always(file, NAMESPOOF_PATH)) break;
                if(!flipper_format_write_header_cstr(file, NAMESPOOF_HEADER, NAMESPOOF_VERSION))
                    break;
                if(!flipper_format_write_string_cstr(file, "Name", app->device_name)) break;
            } while(0);

            flipper_format_free(file);
        }
    }

    if(app->save_level || app->save_angry) {
        Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
        if(app->save_level) {
            int32_t xp = app->dolphin_level > 1 ? DOLPHIN_LEVELS[app->dolphin_level - 2] : 0;
            dolphin->state->data.icounter = xp + 1;
        }
        if(app->save_angry) {
            dolphin->state->data.butthurt = app->dolphin_angry;
        }
        dolphin->state->dirty = true;
        dolphin_state_save(dolphin->state);
        furi_record_close(RECORD_DOLPHIN);
    }

    if(app->save_backlight) {
        rgb_backlight_save_settings();
    }

    if(app->save_settings) {
        XTREME_SETTINGS_SAVE();
    }

    if(app->show_slideshow) {
        callback_reboot(NULL);
    } else if(app->require_reboot) {
        popup_set_header(app->popup, "Rebooting...", 64, 26, AlignCenter, AlignCenter);
        popup_set_text(app->popup, "Applying changes...", 64, 40, AlignCenter, AlignCenter);
        popup_set_callback(app->popup, callback_reboot);
        popup_set_context(app->popup, app);
        popup_set_timeout(app->popup, 1000);
        popup_enable_timeout(app->popup);
        view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewPopup);
        return true;
    } else if(app->apply_pack) {
        popup_set_header(app->popup, "Reloading...", 64, 26, AlignCenter, AlignCenter);
        popup_set_text(app->popup, "Applying asset pack...", 64, 40, AlignCenter, AlignCenter);
        popup_set_callback(app->popup, NULL);
        popup_set_context(app->popup, NULL);
        popup_set_timeout(app->popup, 0);
        popup_disable_timeout(app->popup);
        view_dispatcher_switch_to_view(app->view_dispatcher, XtremeAppViewPopup);
        XTREME_ASSETS_FREE();
        XTREME_ASSETS_LOAD();
    }

    furi_record_close(RECORD_STORAGE);
    return false;
}

static bool xtreme_app_back_event_callback(void* context) {
    furi_assert(context);
    XtremeApp* app = context;

    if(!scene_manager_has_previous_scene(app->scene_manager, XtremeAppSceneStart)) {
        if(xtreme_app_apply(app)) {
            return true;
        }
    }

    return scene_manager_handle_back_event(app->scene_manager);
}

XtremeApp* xtreme_app_alloc() {
    XtremeApp* app = malloc(sizeof(XtremeApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

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

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XtremeAppViewSubmenu, submenu_get_view(app->submenu));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XtremeAppViewTextInput, text_input_get_view(app->text_input));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XtremeAppViewByteInput, byte_input_get_view(app->byte_input));

    app->popup = popup_alloc();
    view_dispatcher_add_view(app->view_dispatcher, XtremeAppViewPopup, popup_get_view(app->popup));

    app->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, XtremeAppViewDialogEx, dialog_ex_get_view(app->dialog_ex));

    // Settings init

    app->asset_pack_index = 0;
    CharList_init(app->asset_pack_names);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* folder = storage_file_alloc(storage);
    FileInfo info;
    char* name = malloc(XTREME_ASSETS_PACK_NAME_LEN);
    if(storage_dir_open(folder, XTREME_ASSETS_PATH)) {
        while(storage_dir_read(folder, &info, name, XTREME_ASSETS_PACK_NAME_LEN)) {
            if(info.flags & FSF_DIRECTORY) {
                char* copy = strdup(name);
                size_t idx = 0;
                for(; idx < CharList_size(app->asset_pack_names); idx++) {
                    char* comp = *CharList_get(app->asset_pack_names, idx);
                    if(strcasecmp(copy, comp) < 0) {
                        break;
                    }
                }
                CharList_push_at(app->asset_pack_names, idx, copy);
                if(app->asset_pack_index != 0) {
                    if(idx < app->asset_pack_index) app->asset_pack_index++;
                } else {
                    if(strcmp(copy, xtreme_settings.asset_pack) == 0)
                        app->asset_pack_index = idx + 1;
                }
            }
        }
    }
    free(name);
    storage_file_free(folder);

    CharList_init(app->mainmenu_app_labels);
    CharList_init(app->mainmenu_app_exes);
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    if(file_stream_open(stream, XTREME_MENU_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        stream_read_line(stream, line);
        while(stream_read_line(stream, line)) {
            furi_string_replace_all(line, "\r", "");
            furi_string_replace_all(line, "\n", "");
            CharList_push_back(app->mainmenu_app_exes, strdup(furi_string_get_cstr(line)));
            flipper_application_load_name_and_icon(line, storage, NULL, line);
            if(furi_string_start_with_str(line, "[")) {
                size_t trim = furi_string_search_str(line, "] ", 1);
                if(trim != FURI_STRING_FAILURE) {
                    furi_string_right(line, trim + 2);
                }
            }
            CharList_push_back(app->mainmenu_app_labels, strdup(furi_string_get_cstr(line)));
        }
    }
    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);

    FlipperFormat* file = flipper_format_file_alloc(storage);
    FrequencyList_init(app->subghz_static_freqs);
    FrequencyList_init(app->subghz_hopper_freqs);
    app->subghz_use_defaults = true;
    do {
        uint32_t temp;
        if(!flipper_format_file_open_existing(file, EXT_PATH("subghz/assets/setting_user"))) break;

        flipper_format_read_bool(file, "Add_standard_frequencies", &app->subghz_use_defaults, 1);

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_static_freqs, temp);
            }
        }

        if(!flipper_format_rewind(file)) break;
        while(flipper_format_read_uint32(file, "Hopper_frequency", &temp, 1)) {
            if(furi_hal_subghz_is_frequency_valid(temp)) {
                FrequencyList_push_back(app->subghz_hopper_freqs, temp);
            }
        }
    } while(false);
    flipper_format_free(file);

    file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(file, "use_ext_range_at_own_risk", &app->subghz_extend, 1);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    strlcpy(app->device_name, furi_hal_version_get_name_ptr(), FURI_HAL_VERSION_ARRAY_NAME_LENGTH);

    Dolphin* dolphin = furi_record_open(RECORD_DOLPHIN);
    DolphinStats stats = dolphin_stats(dolphin);
    app->dolphin_level = stats.level;
    app->dolphin_angry = stats.butthurt;
    furi_record_close(RECORD_DOLPHIN);

    app->version_tag =
        furi_string_alloc_printf("%s  %s", version_get_version(NULL), version_get_builddate(NULL));

    return app;
}

void xtreme_app_free(XtremeApp* app) {
    furi_assert(app);

    // Gui modules
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewVarItemList);
    variable_item_list_free(app->var_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewTextInput);
    text_input_free(app->text_input);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewByteInput);
    byte_input_free(app->byte_input);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewPopup);
    popup_free(app->popup);
    view_dispatcher_remove_view(app->view_dispatcher, XtremeAppViewDialogEx);
    dialog_ex_free(app->dialog_ex);

    // View Dispatcher and Scene Manager
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Settings deinit

    CharList_it_t it;
    for(CharList_it(it, app->asset_pack_names); !CharList_end_p(it); CharList_next(it)) {
        free(*CharList_cref(it));
    }
    CharList_clear(app->asset_pack_names);

    for(CharList_it(it, app->mainmenu_app_labels); !CharList_end_p(it); CharList_next(it)) {
        free(*CharList_cref(it));
    }
    CharList_clear(app->mainmenu_app_labels);
    for(CharList_it(it, app->mainmenu_app_exes); !CharList_end_p(it); CharList_next(it)) {
        free(*CharList_cref(it));
    }
    CharList_clear(app->mainmenu_app_exes);

    FrequencyList_clear(app->subghz_static_freqs);
    FrequencyList_clear(app->subghz_hopper_freqs);

    furi_string_free(app->version_tag);

    // Records
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);
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
