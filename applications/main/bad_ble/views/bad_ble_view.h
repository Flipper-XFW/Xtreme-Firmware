#pragma once

#include <gui/view.h>
#include "../bad_ble_script.h"

typedef struct BadBle BadBle;
typedef void (*BadBleButtonCallback)(InputKey key, void* context);

BadBle* bad_ble_alloc();

void bad_ble_free(BadBle* bad_ble);

View* bad_ble_get_view(BadBle* bad_ble);

void bad_ble_set_button_callback(BadBle* bad_ble, BadBleButtonCallback callback, void* context);

void bad_ble_set_file_name(BadBle* bad_ble, const char* name);

void bad_ble_set_layout(BadBle* bad_ble, const char* layout);

void bad_ble_set_state(BadBle* bad_ble, BadBleState* st);
