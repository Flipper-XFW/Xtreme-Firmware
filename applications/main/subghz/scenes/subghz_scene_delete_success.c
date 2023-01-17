#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"
#include "../../../settings/desktop_settings/desktop_settings_app.h"

void subghz_scene_delete_success_popup_callback(void* context) {
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(
        subghz->view_dispatcher, SubGhzCustomEventSceneDeleteSuccess);
}

void subghz_scene_delete_success_on_enter(void* context) {
    SubGhz* subghz = context;
    DesktopSettings* settings = malloc(sizeof(DesktopSettings));
    DESKTOP_SETTINGS_LOAD(settings);

    // Setup view
    Popup* popup = subghz->popup;
    if(settings->sfw_mode) {
        popup_set_icon(popup, 0, 2, &I_DolphinMafia_115x62_sfw);
    } else {
        popup_set_icon(popup, 0, 2, &I_DolphinMafia_115x62);
    }
    popup_set_header(popup, "Deleted", 83, 19, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, subghz);
    popup_set_callback(popup, subghz_scene_delete_success_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdPopup);
    free(settings);
}

bool subghz_scene_delete_success_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubGhzCustomEventSceneDeleteSuccess) {
            if(scene_manager_search_and_switch_to_previous_scene(
                   subghz->scene_manager, SubGhzSceneReadRAW)) {
                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReadRAW);
            } else if(scene_manager_search_and_switch_to_previous_scene(
                          subghz->scene_manager, SubGhzSceneSaved)) {
                //scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSaved);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    subghz->scene_manager, SubGhzSceneStart);
            }
            return true;
        }
    }
    return false;
}

void subghz_scene_delete_success_on_exit(void* context) {
    SubGhz* subghz = context;
    Popup* popup = subghz->popup;

    popup_reset(popup);
}
