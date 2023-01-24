#include "xtreme_assets.h"
#include "assets_icons.h"
#include <core/dangerous_defines.h>

XtremeAssets* xtreme_assets = NULL;

XtremeAssets* XTREME_ASSETS() {
    if (xtreme_assets == NULL) {
        XTREME_ASSETS_LOAD();
    }
    return xtreme_assets;
}

void XTREME_ASSETS_LOAD() {
    if (xtreme_assets != NULL) return;

    xtreme_assets = malloc(sizeof(XtremeAssets));
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();

    if (xtreme_settings->nsfw_mode) {
        xtreme_assets->I_BLE_Pairing_128x64               = &I_BLE_Pairing_128x64;
        xtreme_assets->I_DolphinCommon_56x48              = &I_DolphinCommon_56x48;
        xtreme_assets->I_DolphinMafia_115x62              = &I_DolphinMafia_115x62;
        xtreme_assets->I_DolphinNice_96x59                = &I_DolphinNice_96x59;
        xtreme_assets->I_DolphinWait_61x59                = &I_DolphinWait_61x59;
        xtreme_assets->I_iButtonDolphinVerySuccess_108x52 = &I_iButtonDolphinVerySuccess_108x52;
        xtreme_assets->I_DolphinReadingSuccess_59x63      = &I_DolphinReadingSuccess_59x63;
        xtreme_assets->I_NFC_dolphin_emulation_47x61      = &I_NFC_dolphin_emulation_47x61;
        xtreme_assets->I_passport_bad_46x49               = &I_flipper;
        xtreme_assets->I_passport_DB                      = &I_passport_DB;
        xtreme_assets->I_passport_happy_46x49             = &I_flipper;
        xtreme_assets->I_passport_okay_46x49              = &I_flipper;
        xtreme_assets->I_RFIDDolphinReceive_97x61         = &I_RFIDDolphinReceive_97x61;
        xtreme_assets->I_RFIDDolphinSend_97x61            = &I_RFIDDolphinSend_97x61;
        xtreme_assets->I_RFIDDolphinSuccess_108x57        = &I_RFIDDolphinSuccess_108x57;
        xtreme_assets->I_Cry_dolph_55x52                  = &I_Cry_dolph_55x52;
        xtreme_assets->I_Scanning_123x52                  = &I_Scanning_123x52;
        xtreme_assets->I_Auth_62x31                       = &I_Auth_62x31;
        xtreme_assets->I_Connect_me_62x31                 = &I_Connect_me_62x31;
        xtreme_assets->I_Connected_62x31                  = &I_Connected_62x31;
        xtreme_assets->I_Error_62x31                      = &I_Error_62x31;
    } else {
        xtreme_assets->I_BLE_Pairing_128x64               = &I_BLE_Pairing_128x64_sfw;
        xtreme_assets->I_DolphinCommon_56x48              = &I_DolphinCommon_56x48_sfw;
        xtreme_assets->I_DolphinMafia_115x62              = &I_DolphinMafia_115x62_sfw;
        xtreme_assets->I_DolphinNice_96x59                = &I_DolphinNice_96x59_sfw;
        xtreme_assets->I_DolphinWait_61x59                = &I_DolphinWait_61x59_sfw;
        xtreme_assets->I_iButtonDolphinVerySuccess_108x52 = &I_iButtonDolphinVerySuccess_108x52_sfw;
        xtreme_assets->I_DolphinReadingSuccess_59x63      = &I_DolphinReadingSuccess_59x63_sfw;
        xtreme_assets->I_NFC_dolphin_emulation_47x61      = &I_NFC_dolphin_emulation_47x61_sfw;
        xtreme_assets->I_passport_bad_46x49               = &I_passport_bad1_46x49_sfw;
        xtreme_assets->I_passport_DB                      = &I_passport_DB_sfw;
        xtreme_assets->I_passport_happy_46x49             = &I_passport_happy1_46x49_sfw;
        xtreme_assets->I_passport_okay_46x49              = &I_passport_okay1_46x49_sfw;
        xtreme_assets->I_RFIDDolphinReceive_97x61         = &I_RFIDDolphinReceive_97x61_sfw;
        xtreme_assets->I_RFIDDolphinSend_97x61            = &I_RFIDDolphinSend_97x61_sfw;
        xtreme_assets->I_RFIDDolphinSuccess_108x57        = &I_RFIDDolphinSuccess_108x57_sfw;
        xtreme_assets->I_Cry_dolph_55x52                  = &I_Cry_dolph_55x52_sfw;
        xtreme_assets->I_Scanning_123x52                  = &I_Scanning_123x52_sfw;
        xtreme_assets->I_Auth_62x31                       = &I_Auth_62x31_sfw;
        xtreme_assets->I_Connect_me_62x31                 = &I_Connect_me_62x31_sfw;
        xtreme_assets->I_Connected_62x31                  = &I_Connected_62x31_sfw;
        xtreme_assets->I_Error_62x31                      = &I_Error_62x31_sfw;
    }

    if (xtreme_settings->asset_pack[0] == '\0') return;
    FileInfo info;
    FuriString* path = furi_string_alloc();
    const char* pack = xtreme_settings->asset_pack;
    furi_string_printf(path, PACKS_DIR "/%s", pack);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if (storage_common_stat(storage, furi_string_get_cstr(path), &info) == FSE_OK && info.flags & FSF_DIRECTORY) {
        File* file = storage_file_alloc(storage);

        swap_bmx_icon(&xtreme_assets->I_BLE_Pairing_128x64,               pack, "BLE/BLE_Pairing_128x64.bmx",                   path, file);
        swap_bmx_icon(&xtreme_assets->I_DolphinCommon_56x48,              pack, "Dolphin/DolphinCommon_56x48.bmx",              path, file);
        swap_bmx_icon(&xtreme_assets->I_DolphinMafia_115x62,              pack, "iButton/DolphinMafia_115x62.bmx",              path, file);
        swap_bmx_icon(&xtreme_assets->I_DolphinNice_96x59,                pack, "iButton/DolphinNice_96x59.bmx",                path, file);
        swap_bmx_icon(&xtreme_assets->I_DolphinWait_61x59,                pack, "iButton/DolphinWait_61x59.bmx",                path, file);
        swap_bmx_icon(&xtreme_assets->I_iButtonDolphinVerySuccess_108x52, pack, "iButton/iButtonDolphinVerySuccess_108x52.bmx", path, file);
        swap_bmx_icon(&xtreme_assets->I_DolphinReadingSuccess_59x63,      pack, "Infrared/DolphinReadingSuccess_59x63.bmx",     path, file);
        swap_bmx_icon(&xtreme_assets->I_NFC_dolphin_emulation_47x61,      pack, "NFC/NFC_dolphin_emulation_47x61.bmx",          path, file);
        swap_bmx_icon(&xtreme_assets->I_passport_bad_46x49,               pack, "Passport/passport_bad_46x49.bmx",              path, file);
        swap_bmx_icon(&xtreme_assets->I_passport_DB,                      pack, "Passport/passport_DB.bmx",                     path, file);
        swap_bmx_icon(&xtreme_assets->I_passport_happy_46x49,             pack, "Passport/passport_happy_46x49.bmx",            path, file);
        swap_bmx_icon(&xtreme_assets->I_passport_okay_46x49,              pack, "Passport/passport_okay_46x49.bmx",             path, file);
        swap_bmx_icon(&xtreme_assets->I_RFIDDolphinReceive_97x61,         pack, "RFID/RFIDDolphinReceive_97x61.bmx",            path, file);
        swap_bmx_icon(&xtreme_assets->I_RFIDDolphinSend_97x61,            pack, "RFID/RFIDDolphinSend_97x61.bmx",               path, file);
        swap_bmx_icon(&xtreme_assets->I_RFIDDolphinSuccess_108x57,        pack, "RFID/RFIDDolphinSuccess_108x57.bmx",           path, file);
        swap_bmx_icon(&xtreme_assets->I_Cry_dolph_55x52,                  pack, "Settings/Cry_dolph_55x52.bmx",                 path, file);
        swap_bmx_icon(&xtreme_assets->I_Scanning_123x52,                  pack, "SubGhz/Scanning_123x52.bmx",                   path, file);
        swap_bmx_icon(&xtreme_assets->I_Auth_62x31,                       pack, "U2F/Auth_62x31.bmx",                           path, file);
        swap_bmx_icon(&xtreme_assets->I_Connect_me_62x31,                 pack, "U2F/Connect_me_62x31.bmx",                     path, file);
        swap_bmx_icon(&xtreme_assets->I_Connected_62x31,                  pack, "U2F/Connected_62x31.bmx",                      path, file);
        swap_bmx_icon(&xtreme_assets->I_Error_62x31,                      pack, "U2F/Error_62x31.bmx",                          path, file);

        storage_file_free(file);
    }
    furi_record_close(RECORD_STORAGE);
    furi_string_free(path);
}

void swap_bmx_icon(const Icon** replace, const char* pack, const char* name, FuriString* path, File* file) {
    furi_string_printf(path, PACKS_DIR "/%s/Icons/%s", pack, name);
    if (storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
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
