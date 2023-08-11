#include "../esp_flasher_app_i.h"

enum QuickState {
    QuickStart,
    QuickBlackmagic,
    QuickBlackmagicWifidev,
    QuickEvilportal,
    QuickEvilportalWifidevS2,
    QuickEvilportalDevproWroom,
    QuickMarauder,
    QuickMarauderWifidevS2,
    QuickMarauderDevproWroom,
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
        submenu_set_header(submenu, "Flash Firmware:");
        submenu_add_item(
            submenu, "Black Magic", QuickBlackmagic, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu, "Evil Portal", QuickEvilportal, esp_flasher_scene_quick_submenu_callback, app);
        submenu_add_item(
            submenu, "Marauder", QuickMarauder, esp_flasher_scene_quick_submenu_callback, app);
        break;
    case QuickBlackmagic:
        submenu_set_header(submenu, "Flash Black Magic To:");
        submenu_add_item(
            submenu,
            "Flipper WiFi Dev Board",
            QuickBlackmagicWifidev,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickEvilportal:
        submenu_set_header(submenu, "Flash Evil Portal To:");
        submenu_add_item(
            submenu,
            "WiFi Dev Board / S2",
            QuickEvilportalWifidevS2,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Dev Board Pro / WROOM",
            QuickEvilportalDevproWroom,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    case QuickMarauder:
        submenu_set_header(submenu, "Flash Marauder To:");
        submenu_add_item(
            submenu,
            "WiFi Dev Board / S2",
            QuickMarauderWifidevS2,
            esp_flasher_scene_quick_submenu_callback,
            app);
        submenu_add_item(
            submenu,
            "Dev Board Pro / WROOM",
            QuickMarauderDevproWroom,
            esp_flasher_scene_quick_submenu_callback,
            app);
        break;
    default:
        break;
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewSubmenu);
}

bool esp_flasher_scene_quick_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    EspFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;

        bool flash = true;
        const char* boot = NULL; // 0x1000
        const char* part = NULL; // 0x8000
        const char* nvs = NULL; // 0x9000
        const char* app0 = APP_DATA_PATH("assets/boot_app0.bin"); // 0xE000
        const char* firm = NULL; // 0x10000

        switch(event.event) {
        case QuickBlackmagic:
        case QuickEvilportal:
        case QuickMarauder:
            scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, event.event);
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneQuick);
            flash = false;
            break;
        case QuickBlackmagicWifidev:
            boot = APP_DATA_PATH("assets/blackmagic/wifidev/bootloader.bin");
            part = APP_DATA_PATH("assets/blackmagic/wifidev/partition-table.bin");
            nvs = APP_DATA_PATH("assets/blackmagic/wifidev/nvs.bin");
            firm = APP_DATA_PATH("assets/blackmagic/wifidev/blackmagic.bin");
            break;
        case QuickEvilportalWifidevS2:
            boot = APP_DATA_PATH("assets/evilportal/wifidev-s2/EvilPortal.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/evilportal/EvilPortal.ino.partitions.bin");
            firm = APP_DATA_PATH("assets/evilportal/wifidev-s2/EvilPortal.ino.bin");
            break;
        case QuickEvilportalDevproWroom:
            boot = APP_DATA_PATH("assets/evilportal/devpro-wroom/EvilPortal.ino.bootloader.bin");
            part = APP_DATA_PATH("assets/evilportal/EvilPortal.ino.partitions.bin");
            firm = APP_DATA_PATH("assets/evilportal/devpro-wroom/EvilPortal.ino.bin");
            break;
        case QuickMarauderWifidevS2:
            boot = APP_DATA_PATH("assets/marauder/wifidev-s2/bootloader.bin");
            part = APP_DATA_PATH("assets/marauder/partitions.bin");
            firm = APP_DATA_PATH("assets/marauder/wifidev-s2/flipper_sd_serial.bin");
            break;
        case QuickMarauderDevproWroom:
            boot = APP_DATA_PATH("assets/marauder/devpro-wroom/bootloader.bin");
            part = APP_DATA_PATH("assets/marauder/partitions.bin");
            firm = APP_DATA_PATH("assets/marauder/devpro-wroom/marauder_dev_board_pro.bin");
            break;
        default:
            flash = false;
            consumed = false;
            break;
        }

        if(flash) {
            memset(app->selected_flash_options, 0, sizeof(app->selected_flash_options));
            app->bin_file_path_boot[0] = '\0';
            app->bin_file_path_part[0] = '\0';
            app->bin_file_path_nvs[0] = '\0';
            app->bin_file_path_boot_app0[0] = '\0';
            app->bin_file_path_app[0] = '\0';
            app->bin_file_path_custom[0] = '\0';

            if(boot) {
                app->selected_flash_options[SelectedFlashBoot] = true;
                strncpy(app->bin_file_path_boot, boot, sizeof(app->bin_file_path_boot));
            }
            if(part) {
                app->selected_flash_options[SelectedFlashPart] = true;
                strncpy(app->bin_file_path_part, part, sizeof(app->bin_file_path_part));
            }
            if(nvs) {
                app->selected_flash_options[SelectedFlashNvs] = true;
                strncpy(app->bin_file_path_nvs, nvs, sizeof(app->bin_file_path_nvs));
            }
            if(app0) {
                app->selected_flash_options[SelectedFlashBootApp0] = true;
                strncpy(app->bin_file_path_boot_app0, app0, sizeof(app->bin_file_path_boot_app0));
            }
            if(firm) {
                app->selected_flash_options[SelectedFlashApp] = true;
                strncpy(app->bin_file_path_app, firm, sizeof(app->bin_file_path_app));
            }

            app->boot = true;
            app->reset = false;
            app->quickflash = true;
            scene_manager_next_scene(app->scene_manager, EspFlasherSceneConsoleOutput);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneQuick, QuickStart);
    }

    return consumed;
}

void esp_flasher_scene_quick_on_exit(void* context) {
    furi_assert(context);

    EspFlasherApp* app = context;
    submenu_reset(app->submenu);
}
