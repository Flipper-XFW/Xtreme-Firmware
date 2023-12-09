#pragma once

#include "scenes/bad_kb_scene.h"
#include "helpers/ducky_script.h"

#include <gui/gui.h>
#include <assets_icons.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

typedef enum BadKbCustomEvent {
    BadKbAppCustomEventTextInputDone,
    BadKbAppCustomEventByteInputDone,
    BadKbCustomEventErrorBack
} BadKbCustomEvent;

typedef enum {
    BadKbAppViewWidget,
    BadKbAppViewWork,
    BadKbAppViewVarItemList,
    BadKbAppViewByteInput,
    BadKbAppViewTextInput,
    BadKbAppViewLoading,
} BadKbAppView;

void bad_kb_app_show_loading_popup(BadKbApp* app, bool show);
