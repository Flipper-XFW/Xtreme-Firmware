#pragma once

#include <furi.h>
#include <input/input.h>

typedef struct Desktop Desktop;

#define RECORD_DESKTOP "desktop"

bool desktop_api_is_locked(Desktop* instance);

void desktop_api_unlock(Desktop* instance);

typedef struct {
    bool locked;
} DesktopStatus;

FuriPubSub* desktop_api_get_status_pubsub(Desktop* instance);

void desktop_run_keybind(Desktop* instance, InputType _type, InputKey _key);
