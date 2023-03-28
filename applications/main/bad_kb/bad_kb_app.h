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

#define BAD_KB_MAC_ADDRESS_LEN 6 // need replace with MAC size maccro
#define BAD_KB_ADV_NAME_MAX_LEN 18

// this is the MAC address used when we do not forget paired device (BOUND STATE)
#define BAD_KB_BOUND_MAC_ADDRESS {0x41, 0x4a, 0xef, 0xb6, 0xa9, 0xd4};

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
    //uint8_t bounded_mac[BAD_KB_MAC_ADDRESS_LEN];
    uint8_t mac[BAD_KB_MAC_ADDRESS_LEN];
    char name[BAD_KB_ADV_NAME_MAX_LEN + 1];
} BadKbBtConfig;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list;

    Bt* bt;
    TextInput* text_input;
    ByteInput* byte_input;
    uint8_t mac[BAD_KB_MAC_ADDRESS_LEN];
    char name[BAD_KB_ADV_NAME_MAX_LEN + 1];
    bool bt_remember; // weither we remember paired devices or not
    BadKbBtConfig bt_old_config;

    BadKbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadKb* bad_kb_view;
    BadKbScript* bad_kb_script;

    bool is_bt;

    FuriHalUsbInterface* usb_prev_mode;
    GapPairing bt_prev_mode;

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
