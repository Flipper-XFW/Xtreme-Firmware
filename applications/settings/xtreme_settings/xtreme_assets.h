#pragma once

#include "xtreme_settings.h"
#include "assets_icons.h"

typedef struct {
    const Icon* passport_happy;
    const Icon* passport_okay;
    const Icon* passport_angry;
} XtremeAssets;

XtremeAssets* XTREME_ASSETS();

void XTREME_ASSETS_UPDATE();
