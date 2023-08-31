#include "../esp_flasher_app_i.h"

enum QuickState {
    QuickStart,
    QuickBlackmagic,
    QuickBlackmagicWifidev,
    QuickDualboot,
    QuickDualbootWifidevS2,
    QuickDualbootDevproWroom,
};

void esp_flasher_scene_quick_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    EspFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void esp_flasher_scene_quick_on_enter(void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;
    Submenu* submenu = app->submenu;
    uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);

    switch(state) {
    case QuickStart:
    case QuickBlackmagic:
    case QuickDualboot:
        submenu_set_header(submenu, "Flash Firmware:");
        submenu_add_item(
            submenu, "Black Magic", QuickBlackmagic, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu,
            "EvilPortal + Marauder",
            QuickDualboot,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickBlackmagicWifidev:
        submenu_set_header(submenu, "Flash Black Magic To:");
        submenu_add_item(
            submenu,
            "Flipper WiFi Dev Board",
            QuickBlackmagicWifidev,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickDualbootWifidevS2:
    case QuickDualbootDevproWroom:
        submenu_set_header(submenu, "Flash EP+MR To:");
        submenu_add_item(
            submenu,
            "WiFi Dev (ESP32-S2)",
            QuickDualbootWifidevS2,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Dev Pro (ESP32-WROOM)",
            QuickDualbootDevproWroom,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    default:
        break;
    }

    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_quick_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;

        bool flash = true;
        bool enter_bootloader = false;
        const char* boot = NULL; // 0x1000
        const char* part = NULL; // 0x8000
        const char* firm = NULL; // 0x10000

        switch(event.event) {
        case QuickBlackmagic:
        case QuickDualboot:
            scene_manager_set_scene_state(
                app->scene_manager, EspFlasherSceneQuick, event.event + 1);
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneQuick);
            flash = false;
            break;
        case QuickBlackmagicWifidev:
            boot = APP_DATA_PATH("assets/blackmagic/bootloader.bin");
            part = APP_DATA_PATH("assets/blackmagic/partition-table.bin");
            firm = APP_DATA_PATH("assets/blackmagic/blackmagic.bin");
            enter_bootloader = true;
            break;
        case QuickDualbootWifidevS2:
            boot = APP_DATA_PATH("assets/dualboot/wifidev-s2.bin");
            enter_bootloader = true;
            break;
        case QuickDualbootDevproWroom:
            boot = APP_DATA_PATH("assets/dualboot/devpro-wroom.bin");
            break;
        default:
            flash = false;
            consumed = false;
            break;
        }

        if(flash) {
            scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, event.event);
            memset(app->selected_flash_options, 0, sizeof(app->selected_flash_options));
            app->bin_file_path_boot[0] = '\0';
            app->bin_file_path_part[0] = '\0';
            app->bin_file_path_nvs[0] = '\0';
            app->bin_file_path_boot_app0[0] = '\0';
            app->bin_file_path_app_a[0] = '\0';
            app->bin_file_path_app_b[0] = '\0';
            app->bin_file_path_custom[0] = '\0';

            if(boot) {
                app->selected_flash_options[SelectedFlashBoot] = true;
                strncpy(app->bin_file_path_boot, boot, sizeof(app->bin_file_path_boot));
            }
            if(part) {
                app->selected_flash_options[SelectedFlashPart] = true;
                strncpy(app->bin_file_path_part, part, sizeof(app->bin_file_path_part));
            }
            if(firm) {
                app->selected_flash_options[SelectedFlashAppA] = true;
                strncpy(app->bin_file_path_app_a, firm, sizeof(app->bin_file_path_app_a));
            }

            app->reset = false;
            app->quickflash = true;
            app->boot = enter_bootloader;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        uint32_t state = scene_manager_get_scene_state(app->scene_manager, EspFlasherSceneQuick);
        if(state > QuickDualboot)
            state = QuickDualboot;
        else if(state > QuickBlackmagic)
            state = QuickBlackmagic;
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, state);
    }

    return consumed;
}

void esp_flasher_scene_quick_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}
