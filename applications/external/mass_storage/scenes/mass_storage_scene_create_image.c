#include "../mass_storage_app_i.h"
#include <lib/toolbox/value_index.h>

enum VarItemListIndex {
    VarItemListIndexImageSize,
    VarItemListIndexImageName,
    VarItemListIndexCreateImage,
};

void mass_storage_scene_create_image_var_item_list_callback(void* context, uint32_t index) {
    MassStorageApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static const struct {
    char* name;
    uint64_t value;
} image_sizes[] = {
    {"1MB", 1LL * 1024 * 1024},
    {"2MB", 2LL * 1024 * 1024},
    {"4MB", 4LL * 1024 * 1024},
    {"8MB", 8LL * 1024 * 1024},
    {"16MB", 16LL * 1024 * 1024},
    {"32MB", 32LL * 1024 * 1024},
    {"64MB", 64LL * 1024 * 1024},
    {"128MB", 128LL * 1024 * 1024},
    {"256MB", 256LL * 1024 * 1024},
    {"512MB", 512LL * 1024 * 1024},
    {"1GB", 1LL * 1024 * 1024 * 1024},
    {"2GB", 2LL * 1024 * 1024 * 1024},
    {"3GB", 3LL * 1024 * 1024 * 1024},
    {"4GB", 4LL * 1024 * 1024 * 1024 - 1},
};
static void mass_storage_scene_create_image_image_size_changed(VariableItem* item) {
    MassStorageApp* app = variable_item_get_context(item);
    app->create_image_size = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, image_sizes[app->create_image_size].name);
}

void mass_storage_scene_create_image_on_enter(void* context) {
    MassStorageApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Image Size",
        COUNT_OF(image_sizes),
        mass_storage_scene_create_image_image_size_changed,
        app);
    variable_item_set_current_value_index(item, app->create_image_size);
    variable_item_set_current_value_text(item, image_sizes[app->create_image_size].name);

    item = variable_item_list_add(var_item_list, "Image Name", 0, NULL, app);
    variable_item_set_current_value_text(item, app->create_image_name);

    variable_item_list_add(var_item_list, "Create Image", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, mass_storage_scene_create_image_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, MassStorageSceneCreateImage));

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewVarItemList);
}

static void popup_callback_ok(void* context) {
    MassStorageApp* app = context;
    scene_manager_set_scene_state(
        app->scene_manager, MassStorageSceneStart, MassStorageSceneFileSelect);
    scene_manager_previous_scene(app->scene_manager);
    scene_manager_next_scene(app->scene_manager, MassStorageSceneFileSelect);
}

static void popup_callback_error(void* context) {
    MassStorageApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewVarItemList);
}

bool mass_storage_scene_create_image_on_event(void* context, SceneManagerEvent event) {
    MassStorageApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, MassStorageSceneCreateImage, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexImageName:
            scene_manager_next_scene(app->scene_manager, MassStorageSceneCreateImageName);
            break;
        case VarItemListIndexCreateImage: {
            mass_storage_app_show_loading_popup(app, true);
            const char* name = strnlen(app->create_image_name, sizeof(app->create_image_name)) ?
                                   app->create_image_name :
                                   image_sizes[app->create_image_size].name;
            furi_string_printf(
                app->file_path,
                "%s/%s%s",
                MASS_STORAGE_APP_PATH_FOLDER,
                name,
                MASS_STORAGE_APP_EXTENSION);

            app->file = storage_file_alloc(app->fs_api);
            const char* error = NULL;
            if(storage_file_open(
                   app->file, furi_string_get_cstr(app->file_path), FSAM_WRITE, FSOM_CREATE_NEW)) {
                if(!storage_file_expand(app->file, image_sizes[app->create_image_size].value)) {
                    error = storage_file_get_error_desc(app->file);
                    storage_file_close(app->file);
                    storage_common_remove(app->fs_api, furi_string_get_cstr(app->file_path));
                }
            } else {
                error = storage_file_get_error_desc(app->file);
            }
            storage_file_free(app->file);
            mass_storage_app_show_loading_popup(app, false);

            if(error) {
                popup_set_header(
                    app->popup, "Error Creating Image!", 64, 26, AlignCenter, AlignCenter);
                popup_set_text(app->popup, error, 64, 40, AlignCenter, AlignCenter);
                popup_set_callback(app->popup, popup_callback_error);
            } else {
                popup_set_header(app->popup, "Image Created!", 64, 32, AlignCenter, AlignCenter);
                popup_set_text(app->popup, "", 0, 0, AlignLeft, AlignBottom);
                popup_set_callback(app->popup, popup_callback_ok);
            }
            popup_set_context(app->popup, app);
            popup_set_timeout(app->popup, 0);
            popup_disable_timeout(app->popup);
            view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewPopup);
            break;
        }
        default:
            break;
        }
    }

    return consumed;
}

void mass_storage_scene_create_image_on_exit(void* context) {
    MassStorageApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
