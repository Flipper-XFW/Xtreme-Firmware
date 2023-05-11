#pragma once

#include "bad_kb_app.h"
#include "scenes/bad_kb_scene.h"
#include "helpers/ducky_script.h"

#include <gui/gui.h>
#include <assets_icons.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#define BAD_KB_APP_BASE_FOLDER EXT_PATH("badkb")
#define BAD_KB_APP_PATH_LAYOUT_FOLDER BAD_KB_APP_BASE_FOLDER "/assets/layouts"
#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

typedef enum BadKbCustomEvent {
    BadKbAppCustomEventTextEditResult,
    BadKbAppCustomEventByteInputDone,
    BadKbCustomEventErrorBack
} BadKbCustomEvent;

typedef enum {
    BadKbAppViewError,
    BadKbAppViewWork,
    BadKbAppViewConfig,
    BadKbAppViewConfigMac,
    BadKbAppViewConfigName
} BadKbAppView;

void bad_kb_config_switch_remember_mode(BadKbApp* app);

int32_t bad_kb_connection_init(BadKbApp* app);

void bad_kb_connection_deinit(BadKbApp* app);
