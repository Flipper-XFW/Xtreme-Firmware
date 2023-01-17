#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>
#include <gui/modules/variable_item_list.h>
#include "xtreme_settings.h"
#include "scenes/xtreme_settings_scene.h"

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
} XtremeSettingsApp;

typedef enum {
    XtremeSettingsAppViewVarItemList,
} XtremeSettingsAppView;
