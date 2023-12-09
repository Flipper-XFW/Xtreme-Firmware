#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <gui/modules/dialog_ex.h>
#include <assets_icons.h>
#include <applications.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/popup.h>
#include <lib/toolbox/value_index.h>
#include <toolbox/stream/file_stream.h>
#include "scenes/xtreme_app_scene.h"
#include "dolphin/helpers/dolphin_state.h"
#include "dolphin/dolphin.h"
#include "dolphin/dolphin_i.h"
#include <lib/flipper_format/flipper_format.h>
#include <lib/subghz/subghz_setting.h>
#include <flipper_application/flipper_application.h>
#include <notification/notification_app.h>
#include <power/power_service/power.h>
#include <rgb_backlight.h>
#include <m-array.h>
#include <xtreme/namespoof.h>
#include <xtreme/xtreme.h>

#define XTREME_SUBGHZ_FREQ_BUFFER_SIZE 7

ARRAY_DEF(CharList, char*)

typedef struct {
    Gui* gui;
    DialogsApp* dialogs;
    NotificationApp* notification;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
    Submenu* submenu;
    TextInput* text_input;
    ByteInput* byte_input;
    Popup* popup;
    DialogEx* dialog_ex;

    CharList_t asset_pack_names;
    uint8_t asset_pack_index;
    CharList_t mainmenu_app_labels;
    CharList_t mainmenu_app_exes;
    uint8_t mainmenu_app_index;
    bool subghz_use_defaults;
    FrequencyList_t subghz_static_freqs;
    uint8_t subghz_static_index;
    FrequencyList_t subghz_hopper_freqs;
    uint8_t subghz_hopper_index;
    char subghz_freq_buffer[XTREME_SUBGHZ_FREQ_BUFFER_SIZE];
    bool subghz_extend;
    RgbColor lcd_color;
    char device_name[FURI_HAL_VERSION_ARRAY_NAME_LENGTH];
    int32_t dolphin_level;
    int32_t dolphin_angry;
    FuriString* version_tag;

    bool save_mainmenu_apps;
    bool save_subghz_freqs;
    bool save_subghz;
    bool save_name;
    bool save_level;
    bool save_angry;
    bool save_backlight;
    bool save_settings;
    bool apply_pack;
    bool show_slideshow;
    bool require_reboot;
} XtremeApp;

typedef enum {
    XtremeAppViewVarItemList,
    XtremeAppViewSubmenu,
    XtremeAppViewTextInput,
    XtremeAppViewByteInput,
    XtremeAppViewPopup,
    XtremeAppViewDialogEx,
} XtremeAppView;

bool xtreme_app_apply(XtremeApp* app);
