#pragma once

#include <gui/icon_i.h>
#include "xtreme_settings.h"
#include <toolbox/path.h>

#define PACKS_DIR EXT_PATH("dolphin_custom")

typedef struct {
    const Icon* passport_happy;
    const Icon* passport_okay;
    const Icon* passport_angry;
} XtremeAssets;

XtremeAssets* XTREME_ASSETS();

void XTREME_ASSETS_UPDATE();

void swap_bmx_icon(const Icon** replace, const char* base, const char* name, FuriString* path, File* file, int i);

void free_bmx_icon(Icon* icon);
