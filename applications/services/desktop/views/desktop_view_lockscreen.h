#pragma once

#include <gui/view.h>

#include "desktop_events.h"

typedef struct DesktopLockscreenView DesktopLockscreenView;

DesktopLockscreenView* desktop_view_lockscreen_alloc();

void desktop_view_lockscreen_free(DesktopLockscreenView* main_view);

View* desktop_view_lockscreen_get_view(DesktopLockscreenView* main_view);
