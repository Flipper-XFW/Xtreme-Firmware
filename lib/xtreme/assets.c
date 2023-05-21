#include "xtreme.h"
#include "private.h"
#include <furi_hal.h>
#include <assets_icons.h>
#include <storage/storage.h>
#include <core/dangerous_defines.h>

#define TAG "XtremeAssets"

#define ICONS_FMT XTREME_ASSETS_PATH "/%s/Icons/%s"

void swap_icon_animated(const Icon* replace, const char* name, FuriString* path, File* file) {
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
                    storage_file_close(file);
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
    storage_file_close(file);
}

void swap_icon_static(const Icon* replace, const char* name, FuriString* path, File* file) {
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
    }
    storage_file_close(file);
}

void XTREME_ASSETS_LOAD() {
    if(!furi_hal_is_normal_boot()) return;

    const char* pack = XTREME_SETTINGS()->asset_pack;
    if(pack[0] == '\0') return;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* p = furi_string_alloc();
    FileInfo info;
    furi_string_printf(p, XTREME_ASSETS_PATH "/%s", pack);
    if(storage_common_stat(storage, furi_string_get_cstr(p), &info) == FSE_OK &&
       info.flags & FSF_DIRECTORY) {
        File* f = storage_file_alloc(storage);

        for(size_t i = 0; i < ICON_PATHS_COUNT; i++) {
            if(ICON_PATHS[i].animated) {
                swap_icon_animated(ICON_PATHS[i].icon, ICON_PATHS[i].path, p, f);
            } else {
                swap_icon_static(ICON_PATHS[i].icon, ICON_PATHS[i].path, p, f);
            }
        }

        storage_file_free(f);
    }
    furi_string_free(p);
    furi_record_close(RECORD_STORAGE);
}
