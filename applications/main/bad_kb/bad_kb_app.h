#pragma once

#include "bad_kb_app.h"
#include "scenes/bad_kb_scene.h"
#include "helpers/ducky_script.h"

#include <gui/gui.h>
#include <assets_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include "views/bad_kb_view.h"

#define BAD_KB_APP_BASE_FOLDER EXT_PATH("badkb")
#define BAD_KB_APP_PATH_LAYOUT_FOLDER BAD_KB_APP_BASE_FOLDER "/assets/layouts"
#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

#define BAD_KB_ADV_NAME_MAX_LEN FURI_HAL_BT_ADV_NAME_LENGTH
#define BAD_KB_MAC_ADDRESS_LEN GAP_MAC_ADDR_SIZE

typedef enum {
    BadKbAppErrorNoFiles,
    BadKbAppErrorCloseRpc,
} BadKbAppError;

typedef enum BadKbCustomEvent {
    BadKbAppCustomEventTextEditResult,
    BadKbAppCustomEventByteInputDone,
    BadKbCustomEventErrorBack
} BadKbCustomEvent;

typedef struct {
    char bt_name[BAD_KB_ADV_NAME_MAX_LEN + 1];
    uint8_t bt_mac[BAD_KB_MAC_ADDRESS_LEN];
    FuriHalUsbInterface* usb_mode;
    GapPairing bt_mode;
} BadKbConfig;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list;
    TextInput* text_input;
    ByteInput* byte_input;

    BadKbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadKb* bad_kb_view;
    BadKbScript* bad_kb_script;

    Bt* bt;
    bool is_bt;
    bool bt_remember;
    BadKbConfig config;
    BadKbConfig prev_config;
    FuriThread* conn_init_thread;
} BadKbApp;

typedef enum {
    BadKbAppViewError,
    BadKbAppViewWork,
    BadKbAppViewConfig,
    BadKbAppViewConfigMac,
    BadKbAppViewConfigName
} BadKbAppView;

void bad_kb_config_switch_mode(BadKbApp* app);

void bad_kb_config_switch_remember_mode(BadKbApp* app);

int32_t bad_kb_connection_init(BadKbApp* app);

void bad_kb_connection_deinit(BadKbApp* app);
