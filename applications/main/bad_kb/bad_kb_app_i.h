#pragma once

#include "bad_kb_app.h"
#include "scenes/bad_kb_scene.h"
#include "bad_kb_script.h"

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

#define BAD_KB_APP_BASE_FOLDER ANY_PATH("badkb")
#define BAD_KB_APP_PATH_LAYOUT_FOLDER BAD_KB_APP_BASE_FOLDER "/layouts"
#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

#define BAD_KB_MAC_ADDRESS_LEN 6 // need replace with MAC size maccro
#define BAD_KB_ADV_NAME_MAX_LEN 18

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
    uint8_t mac[BAD_KB_MAC_ADDRESS_LEN];
    char name[BAD_KB_ADV_NAME_MAX_LEN + 1];

    // number of bt keys before starting the app (all keys added in
    // the bt keys file then will be removed)
    uint16_t n_keys;
} BadKbBtConfig;

struct BadKbApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list_bt;
    VariableItemList* var_item_list_usb;

    Bt* bt;
    TextInput* text_input;
    ByteInput* byte_input;
    uint8_t mac[BAD_KB_MAC_ADDRESS_LEN];
    char name[BAD_KB_ADV_NAME_MAX_LEN + 1];
    BadKbBtConfig bt_old_config;

    BadKbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadKb* bad_kb_view;
    BadKbScript* bad_kb_script;

    bool is_bt;
};

typedef enum {
    BadKbAppViewError,
    BadKbAppViewWork,
    BadKbAppViewConfigBt,
    BadKbAppViewConfigUsb,
    BadKbAppViewConfigMac,
    BadKbAppViewConfigName
} BadKbAppView;
