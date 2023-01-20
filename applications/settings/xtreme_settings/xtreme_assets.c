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

    if (xtreme_settings->sfw_mode) {
        xtreme_assets->authenticate        = &I_Auth_62x31_sfw;
        xtreme_assets->bt_pairing          = &I_BLE_Pairing_128x64_sfw;
        xtreme_assets->connect_me          = &I_Connect_me_62x31_sfw;
        xtreme_assets->connected           = &I_Connected_62x31_sfw;
        xtreme_assets->dolphin_common      = &I_DolphinCommon_56x48_sfw;
        xtreme_assets->dolphin_cry         = &I_Cry_dolph_55x52_sfw;
        xtreme_assets->dolphin_mafia       = &I_DolphinMafia_115x62_sfw;
        xtreme_assets->dolphin_nice        = &I_DolphinNice_96x59_sfw;
        xtreme_assets->dolphin_wait        = &I_DolphinWait_61x59_sfw;
        xtreme_assets->error               = &I_Error_62x31_sfw;
        xtreme_assets->ibutton_success     = &I_iButtonDolphinVerySuccess_108x52_sfw;
        xtreme_assets->ir_success          = &I_DolphinReadingSuccess_59x63_sfw;
        xtreme_assets->nfc_emulation       = &I_NFC_dolphin_emulation_47x61_sfw;
        xtreme_assets->rfid_receive        = &I_RFIDDolphinReceive_97x61_sfw;
        xtreme_assets->rfid_send           = &I_RFIDDolphinSend_97x61_sfw;
        xtreme_assets->rfid_success        = &I_RFIDDolphinSuccess_108x57_sfw;
        xtreme_assets->subghz_scanning     = &I_Scanning_123x52_sfw;

        xtreme_assets->passport_angry      = &I_passport_bad1_46x49_sfw;
        xtreme_assets->passport_background = &I_passport_DB_sfw;
        xtreme_assets->passport_happy      = &I_passport_happy1_46x49_sfw;
        xtreme_assets->passport_okay       = &I_passport_okay1_46x49_sfw;
    } else {
        xtreme_assets->authenticate        = &I_Auth_62x31;
        xtreme_assets->bt_pairing          = &I_BLE_Pairing_128x64;
        xtreme_assets->connect_me          = &I_Connect_me_62x31;
        xtreme_assets->connected           = &I_Connected_62x31;
        xtreme_assets->dolphin_common      = &I_DolphinCommon_56x48;
        xtreme_assets->dolphin_cry         = &I_Cry_dolph_55x52;
        xtreme_assets->dolphin_mafia       = &I_DolphinMafia_115x62;
        xtreme_assets->dolphin_nice        = &I_DolphinNice_96x59;
        xtreme_assets->dolphin_wait        = &I_DolphinWait_61x59;
        xtreme_assets->error               = &I_Error_62x31;
        xtreme_assets->ibutton_success     = &I_iButtonDolphinVerySuccess_108x52;
        xtreme_assets->ir_success          = &I_DolphinReadingSuccess_59x63;
        xtreme_assets->nfc_emulation       = &I_NFC_dolphin_emulation_47x61;
        xtreme_assets->rfid_receive        = &I_RFIDDolphinReceive_97x61;
        xtreme_assets->rfid_send           = &I_RFIDDolphinSend_97x61;
        xtreme_assets->rfid_success        = &I_RFIDDolphinSuccess_108x57;
        xtreme_assets->subghz_scanning     = &I_Scanning_123x52;

        xtreme_assets->passport_angry      = &I_flipper;
        xtreme_assets->passport_background = &I_passport_DB;
        xtreme_assets->passport_happy      = &I_flipper;
        xtreme_assets->passport_okay       = &I_flipper;
    }

    if (xtreme_settings->asset_pack[0] == '\0') return;
    FileInfo info;
    FuriString* path = furi_string_alloc();
    const char* pack = xtreme_settings->asset_pack;
    furi_string_printf(path, PACKS_DIR "/%s", pack);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if (storage_common_stat(storage, furi_string_get_cstr(path), &info) == FSE_OK && info.flags & FSF_DIRECTORY) {
        File* file = storage_file_alloc(storage);

        swap_bmx_icon(&xtreme_assets->authenticate,        pack, "Icons/authenticate.bmx",    path, file);
        swap_bmx_icon(&xtreme_assets->bt_pairing,          pack, "Icons/bt_pairing.bmx",      path, file);
        swap_bmx_icon(&xtreme_assets->connect_me,          pack, "Icons/connect_me.bmx",      path, file);
        swap_bmx_icon(&xtreme_assets->connected,           pack, "Icons/connected.bmx",       path, file);
        swap_bmx_icon(&xtreme_assets->dolphin_common,      pack, "Icons/dolphin_common.bmx",  path, file);
        swap_bmx_icon(&xtreme_assets->dolphin_cry,         pack, "Icons/dolphin_cry.bmx",     path, file);
        swap_bmx_icon(&xtreme_assets->dolphin_mafia,       pack, "Icons/dolphin_mafia.bmx",   path, file);
        swap_bmx_icon(&xtreme_assets->dolphin_nice,        pack, "Icons/dolphin_nice.bmx",    path, file);
        swap_bmx_icon(&xtreme_assets->dolphin_wait,        pack, "Icons/dolphin_wait.bmx",    path, file);
        swap_bmx_icon(&xtreme_assets->error,               pack, "Icons/error.bmx",           path, file);
        swap_bmx_icon(&xtreme_assets->ibutton_success,     pack, "Icons/ibutton_success.bmx", path, file);
        swap_bmx_icon(&xtreme_assets->ir_success,          pack, "Icons/ir_success.bmx",      path, file);
        swap_bmx_icon(&xtreme_assets->nfc_emulation,       pack, "Icons/nfc_emulation.bmx",   path, file);
        swap_bmx_icon(&xtreme_assets->rfid_receive,        pack, "Icons/rfid_receive.bmx",    path, file);
        swap_bmx_icon(&xtreme_assets->rfid_send,           pack, "Icons/rfid_send.bmx",       path, file);
        swap_bmx_icon(&xtreme_assets->rfid_success,        pack, "Icons/rfid_success.bmx",    path, file);
        swap_bmx_icon(&xtreme_assets->subghz_scanning,     pack, "Icons/subghz_scanning.bmx", path, file);

        swap_bmx_icon(&xtreme_assets->passport_angry,      pack, "Passport/angry.bmx",        path, file);
        swap_bmx_icon(&xtreme_assets->passport_background, pack, "Passport/background.bmx",   path, file);
        swap_bmx_icon(&xtreme_assets->passport_happy,      pack, "Passport/happy.bmx",        path, file);
        swap_bmx_icon(&xtreme_assets->passport_okay,       pack, "Passport/okay.bmx",         path, file);

        storage_file_free(file);
    }
    furi_record_close(RECORD_STORAGE);
    furi_string_free(path);
}

void swap_bmx_icon(const Icon** replace, const char* pack, const char* name, FuriString* path, File* file) {
    furi_string_printf(path, PACKS_DIR "/%s/%s", pack, name);
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
