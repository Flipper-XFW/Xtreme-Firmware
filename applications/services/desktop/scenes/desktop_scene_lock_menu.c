#include <gui/scene_manager.h>
#include <applications.h>
#include <furi_hal.h>
#include <toolbox/saved_struct.h>
#include <stdbool.h>
#include <loader/loader.h>
// #include <loader/loader_i.h>
#include <xtreme/settings.h>

#include "../desktop_i.h"
#include <desktop/desktop_settings.h>
#include "../views/desktop_view_lock_menu.h"
#include "desktop_scene_i.h"
#include "desktop_scene.h"
#include "../helpers/pin_lock.h"
#include <power/power_service/power.h>
#define TAG "DesktopSceneLock"

void desktop_scene_lock_menu_callback(DesktopEvent event, void* context) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, event);
}

void desktop_scene_lock_menu_on_enter(void* context) {
    Desktop* desktop = (Desktop*)context;

    DESKTOP_SETTINGS_LOAD(&desktop->settings);
    scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
    desktop_lock_menu_set_callback(desktop->lock_menu, desktop_scene_lock_menu_callback, desktop);
    desktop_lock_menu_set_pin_state(desktop->lock_menu, desktop->settings.pin_code.length > 0);
    desktop_lock_menu_set_idx(desktop->lock_menu, 3);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_set_hide_statusbar(gui, true);
    furi_record_close(RECORD_GUI);

    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewIdLockMenu);
}

void desktop_scene_lock_menu_save_settings(Desktop* desktop) {
    if(desktop->lock_menu->save_notification) {
        notification_message_save_settings(desktop->lock_menu->notification);
        desktop->lock_menu->save_notification = false;
    }
    if(desktop->lock_menu->save_xtreme) {
        XTREME_SETTINGS_SAVE();
        desktop->lock_menu->save_xtreme = false;
    }
    if(desktop->lock_menu->save_bt) {
        bt_settings_save(&desktop->lock_menu->bt->bt_settings);
        desktop->lock_menu->save_bt = false;
    }
}

bool desktop_scene_lock_menu_on_event(void* context, SceneManagerEvent event) {
    Desktop* desktop = (Desktop*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        bool check_pin_changed =
            scene_manager_get_scene_state(desktop->scene_manager, DesktopSceneLockMenu);
        if(check_pin_changed) {
            DESKTOP_SETTINGS_LOAD(&desktop->settings);
            if(desktop->settings.pin_code.length > 0) {
                desktop_lock_menu_set_pin_state(desktop->lock_menu, true);
                scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
                desktop_pin_lock(&desktop->settings);
                desktop_lock(desktop);
            }
        }
    } else if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DesktopLockMenuEventSettings:
            desktop_scene_lock_menu_save_settings(desktop);
            loader_show_settings();
            consumed = true;
            break;
        case DesktopLockMenuEventLock:
            desktop_scene_lock_menu_save_settings(desktop);
            scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
            desktop_lock(desktop);
            consumed = true;
            break;
        case DesktopLockMenuEventLockPin:
            desktop_scene_lock_menu_save_settings(desktop);
            if(desktop->settings.pin_code.length > 0) {
                desktop_pin_lock(&desktop->settings);
                desktop_lock(desktop);
            } else {
                LoaderStatus status =
                    loader_start(desktop->loader, "Desktop", DESKTOP_SETTINGS_RUN_PIN_SETUP_ARG);
                if(status == LoaderStatusOk) {
                    scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 1);
                } else {
                    FURI_LOG_E(TAG, "Unable to start desktop settings");
                }
            }
            consumed = true;
            break;
        case DesktopLockMenuEventXtreme:
            desktop_scene_lock_menu_save_settings(desktop);
            loader_start(
                desktop->loader, FAP_LOADER_APP_NAME, EXT_PATH("apps/.Main/xtreme_app.fap"));
            consumed = true;
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(desktop->scene_manager, DesktopSceneLockMenu, 0);
    }
    return consumed;
}

void desktop_scene_lock_menu_on_exit(void* context) {
    Desktop* desktop = (Desktop*)context;
    desktop_scene_lock_menu_save_settings(desktop);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_set_hide_statusbar(gui, false);
    furi_record_close(RECORD_GUI);
}
