#pragma once

#include <gui/icon_i.h>
#include "xtreme_settings.h"
#include <toolbox/path.h>

#define PACKS_DIR EXT_PATH("dolphin_custom")

typedef struct {
    const Icon* authenticate;
    const Icon* bt_pairing;
    const Icon* connect_me;
    const Icon* connected;
    const Icon* dolphin_common;
    const Icon* dolphin_cry;
    const Icon* dolphin_mafia;
    const Icon* dolphin_nice;
    const Icon* dolphin_wait;
    const Icon* error;
    const Icon* ibutton_success;
    const Icon* ir_success;
    const Icon* nfc_emulation;
    const Icon* rfid_receive;
    const Icon* rfid_send;
    const Icon* rfid_success;
    const Icon* subghz_scanning;

    const Icon* passport_angry;
    const Icon* passport_background;
    const Icon* passport_happy;
    const Icon* passport_okay;
} XtremeAssets;

XtremeAssets* XTREME_ASSETS();

void XTREME_ASSETS_UPDATE();

void swap_bmx_icon(const Icon** replace, const char* base, const char* name, FuriString* path, File* file, int i);

void free_bmx_icon(Icon* icon);
