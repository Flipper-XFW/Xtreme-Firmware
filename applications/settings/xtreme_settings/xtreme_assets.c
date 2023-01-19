#include "xtreme_assets.h"
#include "assets_icons.h"
#include <core/dangerous_defines.h>

#define XTREME_ASSETS_COUNT 3
Icon* loaded_icons[XTREME_ASSETS_COUNT];

XtremeAssets* xtreme_assets = NULL;

XtremeAssets* XTREME_ASSETS() {
    if (xtreme_assets == NULL) {
        XTREME_ASSETS_UPDATE();
    }
    return xtreme_assets;
}

void XTREME_ASSETS_UPDATE() {
    if (xtreme_assets == NULL) {
        xtreme_assets = malloc(sizeof(XtremeAssets));
    }
    XtremeSettings* xtreme_settings = XTREME_SETTINGS();

    if (xtreme_settings->sfw_mode) {
        xtreme_assets->passport_happy = &I_passport_happy1_46x49_sfw;
        xtreme_assets->passport_okay  = &I_passport_okay1_46x49_sfw;
        xtreme_assets->passport_angry = &I_passport_bad1_46x49_sfw;
    } else {
        xtreme_assets->passport_happy = &I_flipper;
        xtreme_assets->passport_okay  = &I_flipper;
        xtreme_assets->passport_angry = &I_flipper;
    }

    for (int i = 0; i < XTREME_ASSETS_COUNT; i++) {
        free_bmx_icon(loaded_icons[i]);
    }

    if (xtreme_settings->asset_pack[0] == '\0') return;
    FileInfo info;
    FuriString* path = furi_string_alloc();
    Storage* storage = furi_record_open(RECORD_STORAGE);
    path_concat(PACKS_DIR, xtreme_settings->asset_pack, path);
    const char* pack = furi_string_get_cstr(path);
    if (storage_common_stat(storage, pack, &info) == FSE_OK && info.flags & FSF_DIRECTORY) {
        File* file = storage_file_alloc(storage);
        int i = 0;

        swap_bmx_icon(&xtreme_assets->passport_happy, pack, "passport_happy.bmx", path, file, i++);
        swap_bmx_icon(&xtreme_assets->passport_okay,  pack, "passport_okay.bmx",  path, file, i++);
        swap_bmx_icon(&xtreme_assets->passport_angry, pack, "passport_angry.bmx", path, file, i++);

        storage_file_free(file);
    }
    furi_record_close(RECORD_STORAGE);
    furi_string_free(path);
}

void swap_bmx_icon(const Icon** replace, const char* base, const char* name, FuriString* path, File* file, int i) {
    loaded_icons[i] = NULL;
    path_concat(base, name, path);
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
        loaded_icons[i] = icon;
        *replace = icon;

        storage_file_close(file);
    }
}

void free_bmx_icon(Icon* icon) {
    if (icon != NULL) {
        free((void*)icon->frames[0]);
        free((void*)icon->frames);
        free(icon);
    }
}
