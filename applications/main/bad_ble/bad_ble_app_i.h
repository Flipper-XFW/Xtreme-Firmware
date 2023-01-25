#pragma once

#include "bad_ble_app.h"
#include "scenes/bad_ble_scene.h"
#include "bad_ble_script.h"
#include "bad_ble_custom_event.h"

#include <gui/gui.h>
#include <bad_ble_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include "views/bad_ble_view.h"

#define BAD_BLE_APP_BASE_FOLDER ANY_PATH("BadUsb")
#define BAD_BLE_APP_PATH_LAYOUT_FOLDER BAD_BLE_APP_BASE_FOLDER "/layouts"
#define BAD_BLE_APP_SCRIPT_EXTENSION ".txt"
#define BAD_BLE_APP_LAYOUT_EXTENSION ".kl"

#define BAD_BLE_MAC_ADDRESS_LEN     6   // need replace with MAC size maccro
#define BAD_BLE_ADV_NAME_MAX_LEN    18

typedef enum {
    BadBleAppErrorNoFiles,
    BadBleAppErrorCloseRpc,
} BadBleAppError;

struct BadBleApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    Submenu* submenu;

    TextInput *text_input;
    ByteInput *byte_input;
    uint8_t mac[BAD_BLE_MAC_ADDRESS_LEN];
    char name[BAD_BLE_ADV_NAME_MAX_LEN + 1];


    BadBleAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadBle* bad_ble_view;
    BadBleScript* bad_ble_script;

    Bt* bt;
};

typedef enum {
    BadBleAppViewError,
    BadBleAppViewWork,
    BadBleAppViewConfig,
    BadBleAppViewConfigMac,
    BadBleAppViewConfigName
} BadBleAppView;