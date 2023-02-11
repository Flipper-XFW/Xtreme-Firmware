#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <assets_icons.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/popup.h>
#include "scenes/xtreme_app_scene.h"
#include "dolphin/helpers/dolphin_state.h"
#include "dolphin/dolphin.h"
#include "dolphin/dolphin_i.h"
#include <lib/flipper_format/flipper_format.h>
#include <m-array.h>
#include "xtreme/settings.h"
#include "xtreme/assets.h"

ARRAY_DEF(asset_packs, char*)

typedef struct {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
    Popup* popup;
    int dolphin_level;
    bool subghz_extend;
    bool subghz_bypass;
    bool settings_changed;
    bool assets_changed;
    bool subghz_changed;
    bool level_changed;
    asset_packs_t asset_packs;
} XtremeApp;

typedef enum {
    XtremeAppViewVarItemList,
    XtremeAppViewPopup,
} XtremeAppView;
