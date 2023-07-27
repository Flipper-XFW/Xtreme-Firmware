#pragma once

#include <gui/icon.h>
#include <m-list.h>

typedef struct {
    const char* label;
    const Icon* icon;
    const char* exe;
} MenuApp;

LIST_DEF(MenuAppList, MenuApp, M_POD_OPLIST)

MenuAppList_t* loader_get_menu_apps(Loader* loader);
