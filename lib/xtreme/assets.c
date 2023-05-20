#include "xtreme.h"
#include "private.h"
#include <assets_icons.h>
#include <storage/storage.h>
#include <core/dangerous_defines.h>

#define TAG "XtremeAssets"

#define ICONS_FMT XTREME_ASSETS_PATH "/%s/Icons/%s"

XtremeAssets xtreme_assets = {};

void anim(const Icon* replace, const char* name, FuriString* path, File* file) {
    const char* pack = XTREME_SETTINGS()->asset_pack;
    furi_string_printf(path, ICONS_FMT "/meta", pack, name);
    if(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        int32_t icon_width, icon_height, frame_rate, frame_count;
        bool ok =
            (storage_file_read(file, &icon_width, 4) == 4 &&
             storage_file_read(file, &icon_height, 4) == 4 &&
             storage_file_read(file, &frame_rate, 4) == 4 &&
             storage_file_read(file, &frame_count, 4) == 4);
        storage_file_close(file);

        if(ok) {
            uint8_t** frames = malloc(sizeof(const uint8_t*) * frame_count);
            int i = 0;
            for(; i < frame_count; i++) {
                furi_string_printf(path, ICONS_FMT "/frame_%02d.bm", pack, name, i);
                if(storage_file_open(
                       file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
                    uint64_t size = storage_file_size(file);
                    frames[i] = malloc(size);
                    ok = storage_file_read(file, frames[i], size) == size;
                    storage_file_close(file);
                    if(ok) continue;
                } else {
                    i--;
                }
                break;
            }

            if(i == frame_count) {
                FURI_CONST_ASSIGN(replace->frame_count, frame_count);
                FURI_CONST_ASSIGN(replace->frame_rate, frame_rate);
                FURI_CONST_ASSIGN(replace->width, icon_width);
                FURI_CONST_ASSIGN(replace->height, icon_height);
                FURI_CONST_ASSIGN_PTR(replace->frames, frames);
            } else {
                for(; i >= 0; i--) {
                    free(frames[i]);
                }
                free(frames);
            }
        }
    }
}

void icon(const Icon* replace, const char* name, FuriString* path, File* file) {
    furi_string_printf(path, ICONS_FMT ".bmx", XTREME_SETTINGS()->asset_pack, name);
    if(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint64_t size = storage_file_size(file) - 8;
        uint8_t* frame = malloc(size);
        int32_t icon_width, icon_height;
        if(storage_file_read(file, &icon_width, 4) == 4 &&
           storage_file_read(file, &icon_height, 4) == 4 &&
           storage_file_read(file, frame, size) == size) {
            FURI_CONST_ASSIGN(replace->frame_count, 1);
            FURI_CONST_ASSIGN(replace->frame_rate, 0);
            FURI_CONST_ASSIGN(replace->width, icon_width);
            FURI_CONST_ASSIGN(replace->height, icon_height);
            FURI_CONST_ASSIGN_PTR(replace->frames, malloc(sizeof(const uint8_t*)));
            FURI_CONST_ASSIGN_PTR(replace->frames[0], frame);
        } else {
            free(frame);
        }

        storage_file_close(file);
    }
}

void swap(FuriString* p, File* f) {
    anim(&A_Levelup_128x64, "Animations/Levelup_128x64", p, f);
    icon(&I_BLE_Pairing_128x64, "BLE/BLE_Pairing_128x64", p, f);
    icon(&I_DolphinCommon_56x48, "Dolphin/DolphinCommon_56x48", p, f);
    icon(&I_DolphinMafia_115x62, "iButton/DolphinMafia_115x62", p, f);
    icon(&I_DolphinNice_96x59, "iButton/DolphinNice_96x59", p, f);
    icon(&I_DolphinWait_61x59, "iButton/DolphinWait_61x59", p, f);
    icon(&I_iButtonDolphinVerySuccess_108x52, "iButton/iButtonDolphinVerySuccess_108x52", p, f);
    icon(&I_DolphinReadingSuccess_59x63, "Infrared/DolphinReadingSuccess_59x63", p, f);
    icon(&I_Lockscreen, "Interface/Lockscreen", p, f);
    icon(&I_WarningDolphin_45x42, "Interface/WarningDolphin_45x42", p, f);
    icon(&I_NFC_dolphin_emulation_47x61, "NFC/NFC_dolphin_emulation_47x61", p, f);
    icon(&I_passport_bad_46x49, "Passport/passport_bad_46x49", p, f);
    icon(&I_passport_DB, "Passport/passport_DB", p, f);
    icon(&I_passport_happy_46x49, "Passport/passport_happy_46x49", p, f);
    icon(&I_passport_okay_46x49, "Passport/passport_okay_46x49", p, f);
    icon(&I_RFIDDolphinReceive_97x61, "RFID/RFIDDolphinReceive_97x61", p, f);
    icon(&I_RFIDDolphinSend_97x61, "RFID/RFIDDolphinSend_97x61", p, f);
    icon(&I_RFIDDolphinSuccess_108x57, "RFID/RFIDDolphinSuccess_108x57", p, f);
    icon(&I_Cry_dolph_55x52, "Settings/Cry_dolph_55x52", p, f);
    icon(&I_Background_128x11, "StatusBar/Background_128x11", p, f);
    icon(&I_Fishing_123x52, "SubGhz/Fishing_123x52", p, f);
    icon(&I_Scanning_123x52, "SubGhz/Scanning_123x52", p, f);
    icon(&I_Auth_62x31, "U2F/Auth_62x31", p, f);
    icon(&I_Connect_me_62x31, "U2F/Connect_me_62x31", p, f);
    icon(&I_Connected_62x31, "U2F/Connected_62x31", p, f);
    icon(&I_Error_62x31, "U2F/Error_62x31", p, f);
}

void XTREME_ASSETS_LOAD() {
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();
    if(xtreme_settings->asset_pack[0] == '\0') return;
    xtreme_assets.is_nsfw = strncmp(xtreme_settings->asset_pack, "NSFW", strlen("NSFW")) == 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FileInfo info;
    FuriString* path = furi_string_alloc();
    furi_string_printf(path, XTREME_ASSETS_PATH "/%s", xtreme_settings->asset_pack);
    if(storage_common_stat(storage, furi_string_get_cstr(path), &info) == FSE_OK &&
       info.flags & FSF_DIRECTORY) {
        File* file = storage_file_alloc(storage);
        swap(path, file);
        storage_file_free(file);
    }
    furi_string_free(path);
    furi_record_close(RECORD_STORAGE);
}

XtremeAssets* XTREME_ASSETS() {
    return &xtreme_assets;
}
