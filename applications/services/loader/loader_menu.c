#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/submenu.h>
#include <assets_icons.h>
#include <applications.h>

#include "loader.h"
#include "loader_menu.h"
#include "loader_menuapp.h"

#define TAG "LoaderMenu"

struct LoaderMenu {
    FuriThread* thread;
    bool settings;
    void (*closed_cb)(void*);
    void* context;
};

static int32_t loader_menu_thread(void* p);

LoaderMenu* loader_menu_alloc(void (*closed_cb)(void*), void* context, bool settings) {
    LoaderMenu* loader_menu = malloc(sizeof(LoaderMenu));
    loader_menu->closed_cb = closed_cb;
    loader_menu->context = context;
    loader_menu->settings = settings;
    loader_menu->thread = furi_thread_alloc_ex(TAG, 1024, loader_menu_thread, loader_menu);
    furi_thread_start(loader_menu->thread);
    return loader_menu;
}

void loader_menu_free(LoaderMenu* loader_menu) {
    furi_assert(loader_menu);
    furi_thread_join(loader_menu->thread);
    furi_thread_free(loader_menu->thread);
    free(loader_menu);
}

typedef enum {
    LoaderMenuViewPrimary,
    LoaderMenuViewSettings,
} LoaderMenuView;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Menu* primary_menu;
    Submenu* settings_menu;
    bool settings;
} LoaderMenuApp;

static void loader_menu_start(const char* name) {
    Loader* loader = furi_record_open(RECORD_LOADER);
    loader_start_with_gui_error(loader, name, NULL);
    furi_record_close(RECORD_LOADER);
}

static void loader_menu_callback(void* context, uint32_t index) {
    UNUSED(context);
    loader_menu_start((const char*)index);
}

static void loader_menu_switch_to_settings(void* context, uint32_t index) {
    UNUSED(index);
    LoaderMenuApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, LoaderMenuViewSettings);
}

static uint32_t loader_menu_switch_to_primary(void* context) {
    UNUSED(context);
    return LoaderMenuViewPrimary;
}

static uint32_t loader_menu_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void loader_menu_build_menu(LoaderMenuApp* app, LoaderMenu* menu) {
    menu_add_item(
        app->primary_menu,
        LOADER_APPLICATIONS_NAME,
        &A_Plugins_14,
        (uint32_t)LOADER_APPLICATIONS_NAME,
        loader_menu_callback,
        (void*)menu);

    Loader* loader = furi_record_open(RECORD_LOADER);
    MenuAppList_t* menu_apps = loader_get_menu_apps(loader);
    for(size_t i = 0; i < MenuAppList_size(*menu_apps); i++) {
        const MenuApp* menu_app = MenuAppList_get(*menu_apps, i);
        menu_add_item(
            app->primary_menu,
            menu_app->label,
            menu_app->icon,
            (uint32_t)menu_app->exe,
            loader_menu_callback,
            (void*)menu);
    }
    furi_record_close(RECORD_LOADER);

    const FlipperExternalApplication* last =
        &FLIPPER_EXTERNAL_APPS[FLIPPER_EXTERNAL_APPS_COUNT - 1];
    menu_add_item(
        app->primary_menu, last->name, last->icon, (uint32_t)last->path, loader_menu_callback, app);
    menu_add_item(
        app->primary_menu, "Settings", &A_Settings_14, 0, loader_menu_switch_to_settings, app);
};

static void loader_menu_build_submenu(LoaderMenuApp* app, LoaderMenu* loader_menu) {
    for(size_t i = 0; i < FLIPPER_SETTINGS_APPS_COUNT; i++) {
        submenu_add_item(
            app->settings_menu,
            FLIPPER_SETTINGS_APPS[i].name,
            (uint32_t)FLIPPER_SETTINGS_APPS[i].name,
            loader_menu_callback,
            loader_menu);
    }
}

static LoaderMenuApp* loader_menu_app_alloc(LoaderMenu* loader_menu) {
    LoaderMenuApp* app = malloc(sizeof(LoaderMenuApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->settings = loader_menu->settings;

    // Primary menu
    if(!app->settings) {
        app->primary_menu = menu_alloc();
        loader_menu_build_menu(app, loader_menu);
        View* primary_view = menu_get_view(app->primary_menu);
        view_set_context(primary_view, app->primary_menu);
        view_set_previous_callback(primary_view, loader_menu_exit);
        view_dispatcher_add_view(app->view_dispatcher, LoaderMenuViewPrimary, primary_view);
    }

    // Settings menu
    app->settings_menu = submenu_alloc();
    loader_menu_build_submenu(app, loader_menu);
    View* settings_view = submenu_get_view(app->settings_menu);
    view_set_context(settings_view, app->settings_menu);
    view_set_previous_callback(
        settings_view, app->settings ? loader_menu_exit : loader_menu_switch_to_primary);
    view_dispatcher_add_view(app->view_dispatcher, LoaderMenuViewSettings, settings_view);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_switch_to_view(
        app->view_dispatcher, app->settings ? LoaderMenuViewSettings : LoaderMenuViewPrimary);

    return app;
}

static void loader_menu_app_free(LoaderMenuApp* app) {
    if(!app->settings) {
        view_dispatcher_remove_view(app->view_dispatcher, LoaderMenuViewPrimary);
        menu_free(app->primary_menu);
    }
    view_dispatcher_remove_view(app->view_dispatcher, LoaderMenuViewSettings);
    submenu_free(app->settings_menu);

    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    free(app);
}

static int32_t loader_menu_thread(void* p) {
    LoaderMenu* loader_menu = p;
    furi_assert(loader_menu);

    LoaderMenuApp* app = loader_menu_app_alloc(loader_menu);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_run(app->view_dispatcher);

    if(loader_menu->closed_cb) {
        loader_menu->closed_cb(loader_menu->context);
    }

    loader_menu_app_free(app);

    return 0;
}
