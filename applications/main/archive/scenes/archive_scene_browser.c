#include "../archive_i.h"
#include "../helpers/archive_files.h"
#include "../helpers/archive_apps.h"
#include "../helpers/archive_favorites.h"
#include "../helpers/archive_browser.h"
#include "../views/archive_browser_view.h"
#include "archive/scenes/archive_scene.h"

#define TAG "ArchiveSceneBrowser"

#define SCENE_STATE_DEFAULT (0)
#define SCENE_STATE_NEED_REFRESH (1)

static const char* flipper_app_name[] = {
    [ArchiveFileTypeIButton] = "iButton",
    [ArchiveFileTypeNFC] = "NFC",
    [ArchiveFileTypeSubGhz] = "SubGHz",
    [ArchiveFileTypeLFRFID] = "RFID",
    [ArchiveFileTypeInfrared] = "Infrared",
    [ArchiveFileTypeBadKb] = "Bad KB",
    [ArchiveFileTypeU2f] = "U2F",
    [ArchiveFileTypeApplication] = "Apps",
    [ArchiveFileTypeUpdateManifest] = "UpdaterApp",
    [ArchiveFileTypeFolder] = "Archive",
};

static void archive_loader_callback(const void* message, void* context) {
    furi_assert(message);
    furi_assert(context);
    const LoaderEvent* event = message;
    ArchiveApp* archive = (ArchiveApp*)context;

    if(event->type == LoaderEventTypeApplicationStopped) {
        view_dispatcher_send_custom_event(
            archive->view_dispatcher, ArchiveBrowserEventListRefresh);
    }
}

static void
    archive_run_in_app(ArchiveBrowserView* browser, ArchiveFile_t* selected, bool favorites) {
    UNUSED(browser);
    Loader* loader = furi_record_open(RECORD_LOADER);

    LoaderStatus status;
    if(selected->is_app) {
        char* param = strrchr(furi_string_get_cstr(selected->path), '/');
        if(param != NULL) {
            param++;
        }
        status = loader_start(loader, flipper_app_name[selected->type], param);
    } else {
        const char* str = furi_string_get_cstr(selected->path);
        if(favorites) {
            char arg[strlen(str) + 4];
            snprintf(arg, sizeof(arg), "fav%s", str);
            status = loader_start(loader, flipper_app_name[selected->type], arg);
        } else {
            status = loader_start(loader, flipper_app_name[selected->type], str);
        }
    }

    if(status != LoaderStatusOk) {
        FURI_LOG_E(TAG, "loader_start failed: %d", status);
    }

    furi_record_close(RECORD_LOADER);
}

void archive_scene_browser_callback(ArchiveBrowserEvent event, void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;
    view_dispatcher_send_custom_event(archive->view_dispatcher, event);
}

void archive_scene_browser_on_enter(void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;
    ArchiveBrowserView* browser = archive->browser;
    browser->is_root = true;

    archive_browser_set_callback(browser, archive_scene_browser_callback, archive);
    if(archive_get_tab(browser) == ArchiveTabFavorites && archive_favorites_count() < 1) {
        archive_switch_tab(browser, TAB_LEFT);
    }
    archive_update_focus(browser, archive->text_store);
    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewBrowser);

    Loader* loader = furi_record_open(RECORD_LOADER);
    archive->loader_stop_subscription =
        furi_pubsub_subscribe(loader_get_pubsub(loader), archive_loader_callback, archive);
    furi_record_close(RECORD_LOADER);

    uint32_t state = scene_manager_get_scene_state(archive->scene_manager, ArchiveAppSceneBrowser);

    if(state == SCENE_STATE_NEED_REFRESH) {
        view_dispatcher_send_custom_event(
            archive->view_dispatcher, ArchiveBrowserEventListRefresh);
    }

    scene_manager_set_scene_state(
        archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_DEFAULT);
}

bool archive_scene_browser_on_event(void* context, SceneManagerEvent event) {
    ArchiveApp* archive = (ArchiveApp*)context;
    ArchiveBrowserView* browser = archive->browser;
    ArchiveFile_t* selected = archive_get_current_file(browser);

    bool favorites = archive_get_tab(browser) == ArchiveTabFavorites;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ArchiveBrowserEventFileMenuOpen:
            archive_show_file_menu(browser, true, false);
            consumed = true;
            break;
        case ArchiveBrowserEventManageMenuOpen:
            if(!favorites) {
                archive_show_file_menu(browser, true, true);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuClose:
            archive_show_file_menu(browser, false, false);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuRun:
            if(selected->type == ArchiveFileTypeFolder) {
                archive_switch_tab(browser, TAB_LEFT);
                archive_enter_dir(browser, selected->path);
            } else if(archive_is_known_app(selected->type)) {
                archive_run_in_app(browser, selected, favorites);
            }
            archive_show_file_menu(browser, false, false);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuPin: {
            const char* name = archive_get_name(browser);
            if(favorites) {
                archive_favorites_delete("%s", name);
                archive_file_array_rm_selected(browser);
            } else if(archive_is_known_app(selected->type)) {
                if(archive_is_favorite("%s", name)) {
                    archive_favorites_delete("%s", name);
                } else {
                    archive_file_append(ARCHIVE_FAV_PATH, "%s\n", name);
                }
            }
            archive_show_file_menu(browser, false, false);
            consumed = true;
        } break;
        case ArchiveBrowserEventFileMenuInfo:
            archive_show_file_menu(browser, false, false);
            scene_manager_set_scene_state(
                archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
            scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneInfo);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuShow:
            archive_show_file_menu(browser, false, false);
            scene_manager_set_scene_state(
                archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
            scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneShow);
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuCut:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard == NULL) {
                            model->clipboard = strdup(furi_string_get_cstr(selected->path));
                            model->clipboard_copy = false;
                        }
                    },
                    false);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuCopy:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard == NULL) {
                            model->clipboard = strdup(furi_string_get_cstr(selected->path));
                            model->clipboard_copy = true;
                        }
                    },
                    false);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuPaste:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                FuriString* path_src = NULL;
                FuriString* path_dst = NULL;
                bool copy;
                with_view_model(
                    browser->view,
                    ArchiveBrowserViewModel * model,
                    {
                        if(model->clipboard != NULL) {
                            path_src = furi_string_alloc_set(model->clipboard);
                            path_dst = furi_string_alloc();
                            FuriString* base = furi_string_alloc();
                            path_extract_basename(model->clipboard, base);
                            path_concat(
                                furi_string_get_cstr(browser->path),
                                furi_string_get_cstr(base),
                                path_dst);
                            furi_string_free(base);
                            copy = model->clipboard_copy;
                            free(model->clipboard);
                            model->clipboard = NULL;
                        }
                    },
                    false);
                if(path_src && path_dst) {
                    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewStack);
                    archive_show_loading_popup(archive, true);
                    FS_Error error = archive_copy_rename_file_or_dir(
                        archive->browser,
                        furi_string_get_cstr(path_src),
                        furi_string_get_cstr(path_dst),
                        copy,
                        true);
                    archive_show_loading_popup(archive, false);
                    furi_string_free(path_src);
                    furi_string_free(path_dst);
                    if(error != FSE_OK) {
                        FuriString* dialog_msg;
                        dialog_msg = furi_string_alloc();
                        furi_string_cat_printf(
                            dialog_msg,
                            "Cannot %s:\n%s",
                            copy ? "copy" : "move",
                            storage_error_get_desc(error));
                        dialog_message_show_storage_error(
                            archive->dialogs, furi_string_get_cstr(dialog_msg));
                        furi_string_free(dialog_msg);
                    }
                    view_dispatcher_switch_to_view(archive->view_dispatcher, ArchiveViewBrowser);
                }
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuNewDir:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                scene_manager_set_scene_state(
                    archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
                scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneNewDir);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuRename:
            archive_show_file_menu(browser, false, false);
            if(favorites) {
                browser->callback(ArchiveBrowserEventEnterFavMove, browser->context);
                //} else if((archive_is_known_app(selected->type)) && (selected->is_app == false)) {
            } else {
                // Added ability to rename files and folders
                scene_manager_set_scene_state(
                    archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
                scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneRename);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventFileMenuDelete:
            archive_show_file_menu(browser, false, false);
            if(!favorites) {
                scene_manager_set_scene_state(
                    archive->scene_manager, ArchiveAppSceneBrowser, SCENE_STATE_NEED_REFRESH);
                scene_manager_next_scene(archive->scene_manager, ArchiveAppSceneDelete);
            }
            consumed = true;
            break;
        case ArchiveBrowserEventEnterDir:
            archive_enter_dir(browser, selected->path);
            consumed = true;
            break;
        case ArchiveBrowserEventFavMoveUp:
            archive_file_array_swap(browser, 1);
            consumed = true;
            break;
        case ArchiveBrowserEventFavMoveDown:
            archive_file_array_swap(browser, -1);
            consumed = true;
            break;
        case ArchiveBrowserEventEnterFavMove:
            furi_string_set(archive->fav_move_str, selected->path);
            archive_favorites_move_mode(archive->browser, true);
            consumed = true;
            break;
        case ArchiveBrowserEventExitFavMove:
            archive_update_focus(browser, furi_string_get_cstr(archive->fav_move_str));
            archive_favorites_move_mode(archive->browser, false);
            consumed = true;
            break;
        case ArchiveBrowserEventSaveFavMove:
            archive_favorites_move_mode(archive->browser, false);
            archive_favorites_save(archive->browser);
            consumed = true;
            break;
        case ArchiveBrowserEventLoadPrevItems:
            archive_file_array_load(archive->browser, -1);
            consumed = true;
            break;
        case ArchiveBrowserEventLoadNextItems:
            archive_file_array_load(archive->browser, 1);
            consumed = true;
            break;
        case ArchiveBrowserEventListRefresh:
            if(!favorites) {
                archive_refresh_dir(browser);
            } else {
                archive_favorites_read(browser);
            }
            consumed = true;
            break;

        case ArchiveBrowserEventExit:
            if(!archive_is_home(browser)) {
                archive_leave_dir(browser);
            } else {
                Loader* loader = furi_record_open(RECORD_LOADER);
                furi_pubsub_unsubscribe(
                    loader_get_pubsub(loader), archive->loader_stop_subscription);
                furi_record_close(RECORD_LOADER);

                view_dispatcher_stop(archive->view_dispatcher);
            }
            consumed = true;
            break;

        default:
            break;
        }
    }
    return consumed;
}

void archive_scene_browser_on_exit(void* context) {
    ArchiveApp* archive = (ArchiveApp*)context;

    Loader* loader = furi_record_open(RECORD_LOADER);
    furi_pubsub_unsubscribe(loader_get_pubsub(loader), archive->loader_stop_subscription);
    furi_record_close(RECORD_LOADER);
}
