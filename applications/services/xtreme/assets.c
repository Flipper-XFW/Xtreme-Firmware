#include "assets.h"
#include <assets_icons.h>
#include <core/dangerous_defines.h>

#define TAG "XtremeAssets"

#define ICONS_FMT PACKS_DIR "/%s/Icons/%s"

XtremeAssets* xtreme_assets = NULL;

void anim(const Icon** replace, const char* name, FuriString* path, File* file) {
    do {
        furi_string_printf(path, ICONS_FMT "/meta", XTREME_SETTINGS()->asset_pack, name);
        if(!storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING))
            break;
        int32_t width, height, frame_rate, frame_count;
        storage_file_read(file, &width, 4);
        storage_file_read(file, &height, 4);
        storage_file_read(file, &frame_rate, 4);
        storage_file_read(file, &frame_count, 4);
        storage_file_close(file);

        Icon* icon = malloc(sizeof(Icon));
        FURI_CONST_ASSIGN(icon->width, width);
        FURI_CONST_ASSIGN(icon->height, height);
        FURI_CONST_ASSIGN(icon->frame_rate, frame_rate);
        FURI_CONST_ASSIGN(icon->frame_count, frame_count);
        icon->frames = malloc(sizeof(const uint8_t*) * icon->frame_count);
        const char* pack = XTREME_SETTINGS()->asset_pack;

        bool ok = true;
        for(int i = 0; i < icon->frame_count; ++i) {
            FURI_CONST_ASSIGN_PTR(icon->frames[i], 0);
            if(ok) {
                ok = false;
                furi_string_printf(path, ICONS_FMT "/frame_%02d.bm", pack, name, i);
                do {
                    if(!storage_file_open(
                           file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING))
                        break;

                    uint64_t size = storage_file_size(file);
                    FURI_CONST_ASSIGN_PTR(icon->frames[i], malloc(size));
                    if(storage_file_read(file, (void*)icon->frames[i], size) == size) ok = true;
                    storage_file_close(file);
                } while(0);
            }
        }
        if(!ok) {
            for(int i = 0; i < icon->frame_count; ++i) {
                if(icon->frames[i]) {
                    free((void*)icon->frames[i]);
                }
            }
            free((void*)icon->frames);
            free(icon);

            break;
        }

        *replace = icon;
    } while(false);
}

void icon(const Icon** replace, const char* name, FuriString* path, File* file) {
    furi_string_printf(path, ICONS_FMT ".bmx", XTREME_SETTINGS()->asset_pack, name);
    if(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint64_t size = storage_file_size(file) - 8;
        int32_t width, height;
        storage_file_read(file, &width, 4);
        storage_file_read(file, &height, 4);

        Icon* icon = malloc(sizeof(Icon));
        FURI_CONST_ASSIGN(icon->frame_count, 1);
        FURI_CONST_ASSIGN(icon->frame_rate, 0);
        FURI_CONST_ASSIGN(icon->width, width);
        FURI_CONST_ASSIGN(icon->height, height);
        icon->frames = malloc(sizeof(const uint8_t*));
        FURI_CONST_ASSIGN_PTR(icon->frames[0], malloc(size));
        storage_file_read(file, (void*)icon->frames[0], size);
        *replace = icon;

        storage_file_close(file);
    }
}

void swap(XtremeAssets* x, FuriString* p, File* f) {
    anim(&x->A_Levelup_128x64, "Animations/Levelup_128x64", p, f);
    icon(&x->I_BLE_Pairing_128x64, "BLE/BLE_Pairing_128x64", p, f);
    icon(&x->I_DolphinCommon_56x48, "Dolphin/DolphinCommon_56x48", p, f);
    icon(&x->I_DolphinMafia_115x62, "iButton/DolphinMafia_115x62", p, f);
    icon(&x->I_DolphinNice_96x59, "iButton/DolphinNice_96x59", p, f);
    icon(&x->I_DolphinWait_61x59, "iButton/DolphinWait_61x59", p, f);
    icon(&x->I_iButtonDolphinVerySuccess_108x52, "iButton/iButtonDolphinVerySuccess_108x52", p, f);
    icon(&x->I_DolphinReadingSuccess_59x63, "Infrared/DolphinReadingSuccess_59x63", p, f);
    icon(&x->I_Lockscreen, "Interface/Lockscreen", p, f);
    icon(&x->I_WarningDolphin_45x42, "Interface/WarningDolphin_45x42", p, f);
    icon(&x->I_NFC_dolphin_emulation_47x61, "NFC/NFC_dolphin_emulation_47x61", p, f);
    icon(&x->I_passport_bad_46x49, "Passport/passport_bad_46x49", p, f);
    icon(&x->I_passport_DB, "Passport/passport_DB", p, f);
    icon(&x->I_passport_happy_46x49, "Passport/passport_happy_46x49", p, f);
    icon(&x->I_passport_okay_46x49, "Passport/passport_okay_46x49", p, f);
    icon(&x->I_RFIDDolphinReceive_97x61, "RFID/RFIDDolphinReceive_97x61", p, f);
    icon(&x->I_RFIDDolphinSend_97x61, "RFID/RFIDDolphinSend_97x61", p, f);
    icon(&x->I_RFIDDolphinSuccess_108x57, "RFID/RFIDDolphinSuccess_108x57", p, f);
    icon(&x->I_Cry_dolph_55x52, "Settings/Cry_dolph_55x52", p, f);
    icon(&x->I_Fishing_123x52, "SubGhz/Fishing_123x52", p, f);
    icon(&x->I_Scanning_123x52, "SubGhz/Scanning_123x52", p, f);
    icon(&x->I_Auth_62x31, "U2F/Auth_62x31", p, f);
    icon(&x->I_Connect_me_62x31, "U2F/Connect_me_62x31", p, f);
    icon(&x->I_Connected_62x31, "U2F/Connected_62x31", p, f);
    icon(&x->I_Error_62x31, "U2F/Error_62x31", p, f);
}

void XTREME_ASSETS_LOAD() {
    if(xtreme_assets != NULL) return;

    xtreme_assets = malloc(sizeof(XtremeAssets));

    xtreme_assets->A_Levelup_128x64 = &A_Levelup_128x64;
    xtreme_assets->I_BLE_Pairing_128x64 = &I_BLE_Pairing_128x64;
    xtreme_assets->I_DolphinCommon_56x48 = &I_DolphinCommon_56x48;
    xtreme_assets->I_DolphinMafia_115x62 = &I_DolphinMafia_115x62;
    xtreme_assets->I_DolphinNice_96x59 = &I_DolphinNice_96x59;
    xtreme_assets->I_DolphinWait_61x59 = &I_DolphinWait_61x59;
    xtreme_assets->I_iButtonDolphinVerySuccess_108x52 = &I_iButtonDolphinVerySuccess_108x52;
    xtreme_assets->I_DolphinReadingSuccess_59x63 = &I_DolphinReadingSuccess_59x63;
    xtreme_assets->I_Lockscreen = &I_Lockscreen;
    xtreme_assets->I_WarningDolphin_45x42 = &I_WarningDolphin_45x42;
    xtreme_assets->I_NFC_dolphin_emulation_47x61 = &I_NFC_dolphin_emulation_47x61;
    xtreme_assets->I_passport_bad_46x49 = &I_passport_bad_46x49;
    xtreme_assets->I_passport_DB = &I_passport_DB;
    xtreme_assets->I_passport_happy_46x49 = &I_passport_happy_46x49;
    xtreme_assets->I_passport_okay_46x49 = &I_passport_okay_46x49;
    xtreme_assets->I_RFIDDolphinReceive_97x61 = &I_RFIDDolphinReceive_97x61;
    xtreme_assets->I_RFIDDolphinSend_97x61 = &I_RFIDDolphinSend_97x61;
    xtreme_assets->I_RFIDDolphinSuccess_108x57 = &I_RFIDDolphinSuccess_108x57;
    xtreme_assets->I_Cry_dolph_55x52 = &I_Cry_dolph_55x52;
    xtreme_assets->I_Fishing_123x52 = &I_Fishing_123x52;
    xtreme_assets->I_Scanning_123x52 = &I_Scanning_123x52;
    xtreme_assets->I_Auth_62x31 = &I_Auth_62x31;
    xtreme_assets->I_Connect_me_62x31 = &I_Connect_me_62x31;
    xtreme_assets->I_Connected_62x31 = &I_Connected_62x31;
    xtreme_assets->I_Error_62x31 = &I_Error_62x31;

    if(furi_hal_rtc_get_boot_mode() != FuriHalRtcBootModeNormal) {
        FURI_LOG_W(TAG, "Load skipped. Device is in special startup mode.");
        return;
    }

    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    if(xtreme_settings->asset_pack[0] == '\0') return;
    xtreme_assets->is_nsfw = strncmp(xtreme_settings->asset_pack, "NSFW", strlen("NSFW")) == 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    int32_t timeout = 5000;
    while(timeout > 0) {
        if(storage_sd_status(storage) == FSE_OK) break;
        furi_delay_ms(250);
        timeout -= 250;
    }

    FileInfo info;
    FuriString* path = furi_string_alloc();
    furi_string_printf(path, PACKS_DIR "/%s", xtreme_settings->asset_pack);
    if(storage_common_stat(storage, furi_string_get_cstr(path), &info) == FSE_OK &&
       info.flags & FSF_DIRECTORY) {
        File* file = storage_file_alloc(storage);
        swap(xtreme_assets, path, file);
        storage_file_free(file);
    }
    furi_string_free(path);
    furi_record_close(RECORD_STORAGE);
}

XtremeAssets* XTREME_ASSETS() {
    if(xtreme_assets == NULL) {
        XTREME_ASSETS_LOAD();
    }
    return xtreme_assets;
}
