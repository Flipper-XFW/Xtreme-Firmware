#pragma once

#include "bad_usb_app.h"
#include "scenes/bad_usb_scene.h"
#include "bad_usb_script.h"
#include "bad_usb_custom_event.h"
#include "bad_usb_script.h"

#include <storage/storage.h>

#include <gui/gui.h>
#include <bad_usb_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include "views/bad_usb_view.h"

#define BAD_USB_APP_BASE_FOLDER ANY_PATH("badusb")
#define BAD_USB_APP_PATH_LAYOUT_FOLDER BAD_USB_APP_BASE_FOLDER "/layouts"
#define BAD_USB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_USB_APP_LAYOUT_EXTENSION ".kl"

#define NUM_CONF_ITEM 3 // modify here if you add new item in config menu
#define NUM_CONF_OPT BadUsbModeNb // for now, scaled on hid mode (ble or usb)

typedef enum {
    BadUsbAppErrorNoFiles,
    BadUsbAppErrorCloseRpc,
} BadUsbAppError;

#define F_DEBUG(app, s) do { if(app->debug_file) { storage_file_write(app->debug_file, s, strlen(s)); } } while(0)

struct BadUsbApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;

    //Submenu* submenu;
    VariableItemList* variable_item_list;
    uint8_t menu_idx;
    uint8_t menu_opt_idx[NUM_CONF_OPT];

    BadUsbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadUsb* bad_usb_view;
    BadUsbScript* bad_usb_script;
    BadUsbMode mode;

    File *debug_file;
};

typedef enum {
    BadUsbAppViewError,
    BadUsbAppViewWork,
    BadUsbAppViewConfig,
    // for ble hid related information
    BadUsbAppViewConfigBle
} BadUsbAppView;
