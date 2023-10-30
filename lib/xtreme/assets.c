#include "xtreme.h"
#include <furi_hal.h>
#include <gui/icon_i.h>
#include <assets_icons.h>
#include <storage/storage.h>
#include <core/dangerous_defines.h>

#define TAG "XtremeAssets"

#define ICONS_FMT XTREME_ASSETS_PATH "/%s/Icons/%s"

void load_icon_animated(const Icon* replace, const char* name, FuriString* path, File* file) {
    const char* pack = xtreme_settings.asset_pack;
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
                Icon* original = malloc(sizeof(Icon));
                memcpy(original, replace, sizeof(Icon));
                FURI_CONST_ASSIGN_PTR(replace->original, original);
                FURI_CONST_ASSIGN(replace->width, icon_width);
                FURI_CONST_ASSIGN(replace->height, icon_height);
                FURI_CONST_ASSIGN(replace->frame_rate, frame_rate);
                FURI_CONST_ASSIGN(replace->frame_count, frame_count);
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

void load_icon_static(const Icon* replace, const char* name, FuriString* path, File* file) {
    furi_string_printf(path, ICONS_FMT ".bmx", xtreme_settings.asset_pack, name);
    if(storage_file_open(file, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint64_t size = storage_file_size(file) - 8;
        uint8_t* frame = malloc(size);
        int32_t icon_width, icon_height;

        if(storage_file_read(file, &icon_width, 4) == 4 &&
           storage_file_read(file, &icon_height, 4) == 4 &&
           storage_file_read(file, frame, size) == size) {
            Icon* original = malloc(sizeof(Icon));
            memcpy(original, replace, sizeof(Icon));
            FURI_CONST_ASSIGN_PTR(replace->original, original);
            uint8_t** frames = malloc(sizeof(const uint8_t*));
            frames[0] = frame;
            FURI_CONST_ASSIGN(replace->frame_rate, 0);
            FURI_CONST_ASSIGN(replace->frame_count, 1);
            FURI_CONST_ASSIGN(replace->width, icon_width);
            FURI_CONST_ASSIGN(replace->height, icon_height);
            FURI_CONST_ASSIGN_PTR(replace->frames, frames);
        } else {
            free(frame);
        }
    }
    storage_file_close(file);
}

void free_icon(const Icon* icon) {
    uint8_t** frames = (void*)icon->frames;
    int32_t frame_count = icon->frame_count;

    Icon* original = icon->original;
    memcpy((void*)icon, original, sizeof(Icon));

    free(original);
    for(int32_t i = 0; i < frame_count; i++) {
        free(frames[i]);
    }
    free(frames);
}

void XTREME_ASSETS_LOAD() {
    const char* pack = xtreme_settings.asset_pack;
    xtreme_settings.is_nsfw = !strncmp(pack, "NSFW", strlen("NSFW"));
    if(pack[0] == '\0') return;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* p = furi_string_alloc();
    FileInfo info;
    furi_string_printf(p, XTREME_ASSETS_PATH "/%s", pack);
    if(storage_common_stat(storage, furi_string_get_cstr(p), &info) == FSE_OK &&
       info.flags & FSF_DIRECTORY) {
        File* f = storage_file_alloc(storage);

        for(size_t i = 0; i < ICON_PATHS_COUNT; i++) {
            if(ICON_PATHS[i].icon->original == NULL) {
                if(ICON_PATHS[i].animated) {
                    load_icon_animated(ICON_PATHS[i].icon, ICON_PATHS[i].path, p, f);
                } else {
                    load_icon_static(ICON_PATHS[i].icon, ICON_PATHS[i].path, p, f);
                }
            }
        }

        storage_file_free(f);
    }
    furi_string_free(p);
    furi_record_close(RECORD_STORAGE);
}

void XTREME_ASSETS_FREE() {
    for(size_t i = 0; i < ICON_PATHS_COUNT; i++) {
        if(ICON_PATHS[i].icon->original != NULL) {
            free_icon(ICON_PATHS[i].icon);
        }
    }
}
