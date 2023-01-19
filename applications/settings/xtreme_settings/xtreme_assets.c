#include "xtreme_assets.h"

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
        xtreme_assets->passport_okay = &I_passport_okay1_46x49_sfw;
        xtreme_assets->passport_angry = &I_passport_bad1_46x49_sfw;
    } else {
        xtreme_assets->passport_happy = &I_flipper;
        xtreme_assets->passport_okay = &I_flipper;
        xtreme_assets->passport_angry = &I_flipper;
    }
}
