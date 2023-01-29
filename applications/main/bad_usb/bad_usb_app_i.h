#pragma once

#include "bad_usb_app.h"
#include "scenes/bad_usb_scene.h"
#include "bad_usb_script.h"

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
#include "views/bad_usb_view.h"

#define BAD_USB_APP_BASE_FOLDER ANY_PATH("badusb")
#define BAD_USB_APP_PATH_LAYOUT_FOLDER BAD_USB_APP_BASE_FOLDER "/layouts"
#define BAD_USB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_USB_APP_LAYOUT_EXTENSION ".kl"

#define BAD_USB_MAC_ADDRESS_LEN 6 // need replace with MAC size maccro
#define BAD_USB_ADV_NAME_MAX_LEN 18

typedef enum {
    BadUsbAppErrorNoFiles,
    BadUsbAppErrorCloseRpc,
} BadUsbAppError;

typedef enum BadUsbCustomEvent {
    BadUsbAppCustomEventTextEditResult,
    BadUsbAppCustomEventByteInputDone,
    BadUsbCustomEventErrorBack
} BadUsbCustomEvent;

struct BadUsbApp {
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
    uint8_t mac[BAD_USB_MAC_ADDRESS_LEN];
    char name[BAD_USB_ADV_NAME_MAX_LEN + 1];

    BadUsbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadUsb* bad_usb_view;
    BadUsbScript* bad_usb_script;

    bool is_bt;
};

typedef enum {
    BadUsbAppViewError,
    BadUsbAppViewWork,
    BadUsbAppViewConfigBt,
    BadUsbAppViewConfigUsb,
    BadUsbAppViewConfigMac,
    BadUsbAppViewConfigName
} BadUsbAppView;
