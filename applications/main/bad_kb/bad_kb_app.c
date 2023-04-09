#include "bad_kb_app.h"
#include "bad_kb_settings_filename.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <xtreme/settings.h>
#include <lib/flipper_format/flipper_format.h>

#include <bt/bt_service/bt_i.h>
#include <bt/bt_service/bt.h>

#define BAD_KB_SETTINGS_PATH BAD_KB_APP_BASE_FOLDER "/" BAD_KB_SETTINGS_FILE_NAME

// this is the MAC address used when we do not forget paired device (BOUND STATE)
const uint8_t BAD_KB_BOUND_MAC_ADDRESS[BAD_KB_MAC_ADDRESS_LEN] =
    {0x41, 0x4a, 0xef, 0xb6, 0xa9, 0xd4};
const uint8_t BAD_KB_EMPTY_MAC_ADDRESS[BAD_KB_MAC_ADDRESS_LEN] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static bool bad_kb_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    BadKbApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool bad_kb_app_back_event_callback(void* context) {
    furi_assert(context);
    BadKbApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void bad_kb_app_tick_event_callback(void* context) {
    furi_assert(context);
    BadKbApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

static void bad_kb_load_settings(BadKbApp* app) {
    furi_string_reset(app->keyboard_layout);
    strcpy(app->config.bt_name, "");
    memcpy(
        app->config.bt_mac,
        furi_hal_bt_get_profile_mac_addr(FuriHalBtProfileHidKeyboard),
        BAD_KB_MAC_ADDRESS_LEN);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, BAD_KB_SETTINGS_PATH)) {
        FuriString* tmp_str = furi_string_alloc();
        if(!flipper_format_read_string(file, "Keyboard_Layout", app->keyboard_layout)) {
            furi_string_reset(app->keyboard_layout);
        }
        if(flipper_format_read_string(file, "Bt_Name", tmp_str) && !furi_string_empty(tmp_str)) {
            strcpy(app->config.bt_name, furi_string_get_cstr(tmp_str));
        } else {
            strcpy(app->config.bt_name, "");
        }
        if(!flipper_format_read_hex(
               file, "Bt_Mac", (uint8_t*)&app->config.bt_mac, BAD_KB_MAC_ADDRESS_LEN)) {
            memcpy(
                app->config.bt_mac,
                furi_hal_bt_get_profile_mac_addr(FuriHalBtProfileHidKeyboard),
                BAD_KB_MAC_ADDRESS_LEN);
        }
        furi_string_free(tmp_str);
        flipper_format_file_close(file);
    }
    flipper_format_free(file);

    if(!furi_string_empty(app->keyboard_layout)) {
        FileInfo layout_file_info;
        FS_Error file_check_err = storage_common_stat(
            storage, furi_string_get_cstr(app->keyboard_layout), &layout_file_info);
        if(file_check_err != FSE_OK) {
            furi_string_reset(app->keyboard_layout);
            return;
        }
        if(layout_file_info.size != 256) {
            furi_string_reset(app->keyboard_layout);
        }
    }

    furi_record_close(RECORD_STORAGE);
}

static void bad_kb_save_settings(BadKbApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_always(file, BAD_KB_SETTINGS_PATH)) {
        flipper_format_write_string(file, "Keyboard_Layout", app->keyboard_layout);
        flipper_format_write_string_cstr(file, "Bt_Name", app->config.bt_name);
        flipper_format_write_hex(
            file, "Bt_Mac", (uint8_t*)&app->config.bt_mac, BAD_KB_MAC_ADDRESS_LEN);
        flipper_format_file_close(file);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

void bad_kb_reload_worker(BadKbApp* app) {
    bad_kb_script_close(app->bad_kb_script);
    app->bad_kb_script = bad_kb_script_open(app->file_path, app->is_bt ? app->bt : NULL);
    bad_kb_script_set_keyboard_layout(app->bad_kb_script, app->keyboard_layout);
}

void bad_kb_config_switch_mode(BadKbApp* app) {
    scene_manager_previous_scene(app->scene_manager);
    if(app->is_bt) {
        furi_hal_bt_start_advertising();
    } else {
        furi_hal_bt_stop_advertising();
    }
    scene_manager_next_scene(app->scene_manager, BadKbSceneConfig);
    bad_kb_reload_worker(app);
}

void bad_kb_config_switch_remember_mode(BadKbApp* app) {
    if(app->bt_remember) {
        furi_hal_bt_set_profile_pairing_method(
            FuriHalBtProfileHidKeyboard, GapPairingPinCodeVerifyYesNo);
        bt_set_profile_mac_address(app->bt, (uint8_t*)&BAD_KB_BOUND_MAC_ADDRESS);
        bt_enable_peer_key_update(app->bt);
    } else {
        furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, GapPairingNone);
        bt_set_profile_mac_address(app->bt, app->config.bt_mac);
        bt_disable_peer_key_update(app->bt);
    }
    bad_kb_reload_worker(app);
}

int32_t bad_kb_connection_init(BadKbApp* app) {
    app->prev_config.usb_mode = furi_hal_usb_get_config();
    furi_hal_usb_set_config(NULL, NULL);

    strcpy(
        app->prev_config.bt_name, furi_hal_bt_get_profile_adv_name(FuriHalBtProfileHidKeyboard));
    memcpy(
        app->prev_config.bt_mac,
        furi_hal_bt_get_profile_mac_addr(FuriHalBtProfileHidKeyboard),
        BAD_KB_MAC_ADDRESS_LEN);
    app->prev_config.bt_mode = furi_hal_bt_get_profile_pairing_method(FuriHalBtProfileHidKeyboard);

    bt_timeout = bt_hid_delays[LevelRssi39_0];
    bt_disconnect(app->bt);
    bt_keys_storage_set_storage_path(app->bt, BAD_KB_APP_PATH_BOUND_KEYS_FILE);
    if(strcmp(app->config.bt_name, "") != 0) {
        furi_hal_bt_set_profile_adv_name(FuriHalBtProfileHidKeyboard, app->config.bt_name);
    }
    if(app->bt_remember) {
        furi_hal_bt_set_profile_mac_addr(
            FuriHalBtProfileHidKeyboard, (uint8_t*)&BAD_KB_BOUND_MAC_ADDRESS);
        furi_hal_bt_set_profile_pairing_method(
            FuriHalBtProfileHidKeyboard, GapPairingPinCodeVerifyYesNo);
    } else {
        if(memcmp(
               app->config.bt_mac, (uint8_t*)&BAD_KB_EMPTY_MAC_ADDRESS, BAD_KB_MAC_ADDRESS_LEN) !=
           0) {
            furi_hal_bt_set_profile_mac_addr(FuriHalBtProfileHidKeyboard, app->config.bt_mac);
        }
        furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, GapPairingNone);
    }
    bt_set_profile(app->bt, BtProfileHidKeyboard);
    if(strcmp(app->config.bt_name, "") == 0) {
        strcpy(app->config.bt_name, furi_hal_bt_get_profile_adv_name(FuriHalBtProfileHidKeyboard));
    }
    if(memcmp(app->config.bt_mac, (uint8_t*)&BAD_KB_EMPTY_MAC_ADDRESS, BAD_KB_MAC_ADDRESS_LEN) ==
       0) {
        memcpy(
            app->config.bt_mac,
            furi_hal_bt_get_profile_mac_addr(FuriHalBtProfileHidKeyboard),
            BAD_KB_MAC_ADDRESS_LEN);
    }
    if(app->is_bt) {
        furi_hal_bt_start_advertising();
        if(app->bt_remember) {
            bt_enable_peer_key_update(app->bt);
        } else {
            bt_disable_peer_key_update(app->bt);
        }
    } else {
        furi_hal_bt_stop_advertising();
    }

    return 0;
}

void bad_kb_connection_deinit(BadKbApp* app) {
    furi_hal_usb_set_config(app->prev_config.usb_mode, NULL);

    bt_disconnect(app->bt);
    bt_keys_storage_set_default_path(app->bt);
    furi_hal_bt_set_profile_adv_name(FuriHalBtProfileHidKeyboard, app->prev_config.bt_name);
    furi_hal_bt_set_profile_mac_addr(FuriHalBtProfileHidKeyboard, app->prev_config.bt_mac);
    furi_hal_bt_set_profile_pairing_method(FuriHalBtProfileHidKeyboard, app->prev_config.bt_mode);
    bt_set_profile(app->bt, BtProfileSerial);
    bt_enable_peer_key_update(app->bt);
}

BadKbApp* bad_kb_app_alloc(char* arg) {
    BadKbApp* app = malloc(sizeof(BadKbApp));

    app->bad_kb_script = NULL;

    app->file_path = furi_string_alloc();
    app->keyboard_layout = furi_string_alloc();
    if(arg && strlen(arg)) {
        furi_string_set(app->file_path, arg);
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, BAD_KB_APP_BASE_FOLDER);
    furi_record_close(RECORD_STORAGE);

    bad_kb_load_settings(app);

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&bad_kb_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, bad_kb_app_tick_event_callback, 500);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, bad_kb_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, bad_kb_app_back_event_callback);

    Bt* bt = furi_record_open(RECORD_BT);
    app->bt = bt;
    app->bt->suppress_pin_screen = true;
    app->is_bt = XTREME_SETTINGS()->bad_bt;
    app->bt_remember = XTREME_SETTINGS()->bad_bt_remember;

    // Custom Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewError, widget_get_view(app->widget));

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewConfig, variable_item_list_get_view(app->var_item_list));

    app->bad_kb_view = bad_kb_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewWork, bad_kb_get_view(app->bad_kb_view));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewConfigName, text_input_get_view(app->text_input));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewConfigMac, byte_input_get_view(app->byte_input));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    if(furi_hal_usb_is_locked()) {
        app->error = BadKbAppErrorCloseRpc;
        app->conn_init_thread = NULL;
        scene_manager_next_scene(app->scene_manager, BadKbSceneError);
    } else {
        app->conn_init_thread = furi_thread_alloc_ex(
            "BadKbConnInit", 1024, (FuriThreadCallback)bad_kb_connection_init, app);
        furi_thread_start(app->conn_init_thread);
        if(!furi_string_empty(app->file_path)) {
            app->bad_kb_script = bad_kb_script_open(app->file_path, app->is_bt ? app->bt : NULL);
            bad_kb_script_set_keyboard_layout(app->bad_kb_script, app->keyboard_layout);
            scene_manager_next_scene(app->scene_manager, BadKbSceneWork);
        } else {
            furi_string_set(app->file_path, BAD_KB_APP_BASE_FOLDER);
            scene_manager_next_scene(app->scene_manager, BadKbSceneFileSelect);
        }
    }

    return app;
}

void bad_kb_app_free(BadKbApp* app) {
    furi_assert(app);

    if(app->bad_kb_script) {
        bad_kb_script_close(app->bad_kb_script);
        app->bad_kb_script = NULL;
    }

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewWork);
    bad_kb_free(app->bad_kb_view);

    // Custom Widget
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewError);
    widget_free(app->widget);

    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewConfig);
    variable_item_list_free(app->var_item_list);

    // Text Input
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewConfigName);
    text_input_free(app->text_input);

    // Byte Input
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewConfigMac);
    byte_input_free(app->byte_input);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Restore bt config
    app->bt->suppress_pin_screen = false;
    if(app->conn_init_thread) {
        furi_thread_join(app->conn_init_thread);
        furi_thread_free(app->conn_init_thread);
        bad_kb_connection_deinit(app);
    }

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_BT);

    bad_kb_save_settings(app);

    furi_string_free(app->file_path);
    furi_string_free(app->keyboard_layout);

    free(app);
}

int32_t bad_kb_app(void* p) {
    BadKbApp* bad_kb_app = bad_kb_app_alloc((char*)p);

    view_dispatcher_run(bad_kb_app->view_dispatcher);

    bad_kb_app_free(bad_kb_app);
    return 0;
}
