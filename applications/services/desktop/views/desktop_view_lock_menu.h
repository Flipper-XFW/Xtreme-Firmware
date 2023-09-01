#pragma once

#include <gui/view.h>
#include "desktop_events.h"
#include <bt/bt_service/bt.h>
#include <bt/bt_service/bt_i.h>

#define HINT_TIMEOUT 2

typedef struct DesktopLockMenuView DesktopLockMenuView;

typedef void (*DesktopLockMenuViewCallback)(DesktopEvent event, void* context);

struct DesktopLockMenuView {
    View* view;
    DesktopLockMenuViewCallback callback;
    void* context;

    NotificationApp* notification;
    Bt* bt;
    bool save_notification;
    bool save_xtreme;
    bool save_bt;
};

typedef struct {
    uint8_t idx;
    bool _dummy_mode; // Unused, kept for compatibility
    bool stealth_mode;

    bool pin_is_set;
    int pin_lock;
    bool show_lock_menu;
    DesktopLockMenuView* lock_menu;
} DesktopLockMenuViewModel;

void desktop_lock_menu_set_callback(
    DesktopLockMenuView* lock_menu,
    DesktopLockMenuViewCallback callback,
    void* context);

View* desktop_lock_menu_get_view(DesktopLockMenuView* lock_menu);
void desktop_lock_menu_set_pin_state(DesktopLockMenuView* lock_menu, bool pin_is_set);
void desktop_lock_menu_set_stealth_mode_state(DesktopLockMenuView* lock_menu, bool stealth_mode);
void desktop_lock_menu_set_idx(DesktopLockMenuView* lock_menu, uint8_t idx);
DesktopLockMenuView* desktop_lock_menu_alloc();
void desktop_lock_menu_free(DesktopLockMenuView* lock_menu);
