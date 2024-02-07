#pragma once

#include <gui/gui.h>
#include <gui/modules/popup.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>
#include <dialogs/dialogs.h>
#include <assets_icons.h>

#include <desktop/desktop_i.h>
#include <desktop/views/desktop_view_pin_input.h>
#include "views/desktop_settings_view_pin_setup_howto.h"
#include "views/desktop_settings_view_pin_setup_howto2.h"

typedef enum {
    DesktopSettingsAppViewMenu,
    DesktopSettingsAppViewVarItemList,
    DesktopSettingsAppViewIdPopup,
    DesktopSettingsAppViewDialogEx,
    DesktopSettingsAppViewIdPinInput,
    DesktopSettingsAppViewIdPinSetupHowto,
    DesktopSettingsAppViewIdPinSetupHowto2,
} DesktopSettingsAppView;

typedef enum {
    DesktopSettingsAppKeybindActionTypeMainApp,
    DesktopSettingsAppKeybindActionTypeExternalApp,
    DesktopSettingsAppKeybindActionTypeOpenFile,
    DesktopSettingsAppKeybindActionTypeMoreActions,
    DesktopSettingsAppKeybindActionTypeRemoveKeybind,
} DesktopSettingsAppKeybindActionType;

extern const char* EXTRA_KEYBINDS[];
extern const size_t EXTRA_KEYBINDS_COUNT;

typedef struct {
    Gui* gui;
    Desktop* desktop;
    DialogsApp* dialogs;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* variable_item_list;
    Submenu* submenu;
    Popup* popup;
    DialogEx* dialog_ex;
    DesktopViewPinInput* pin_input_view;
    DesktopSettingsViewPinSetupHowto* pin_setup_howto_view;
    DesktopSettingsViewPinSetupHowto2* pin_setup_howto2_view;

    PinCode pincode_buffer;
    bool pincode_buffer_filled;

    uint8_t menu_idx;

    bool save_settings;
} DesktopSettingsApp;

const char* desktop_settings_app_get_keybind(DesktopSettingsApp* app);
bool desktop_settings_app_set_keybind(DesktopSettingsApp* app, const char* value);
