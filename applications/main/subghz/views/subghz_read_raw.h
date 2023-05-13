#pragma once

#include <gui/view.h>
#include "../helpers/subghz_custom_event.h"

#define SUBGHZ_RAW_THRESHOLD_MIN -90.0f

typedef void (*SubGhzReadRAWCallback)(SubGhzCustomEvent event, void* context);

typedef struct {
    View* view;
    SubGhzReadRAWCallback callback;
    void* context;
} SubGhzReadRAW;

typedef enum {
    SubGhzReadRAWStatusStart,
    SubGhzReadRAWStatusIDLE,
    SubGhzReadRAWStatusREC,
    SubGhzReadRAWStatusTX,
    SubGhzReadRAWStatusTXRepeat,

    SubGhzReadRAWStatusLoadKeyIDLE,
    SubGhzReadRAWStatusLoadKeyTX,
    SubGhzReadRAWStatusLoadKeyTXRepeat,
    SubGhzReadRAWStatusSaveKey,
} SubGhzReadRAWStatus;

typedef struct {
    FuriString* frequency_str;
    FuriString* preset_str;
    FuriString* sample_write;
    FuriString* file_name;
    uint8_t* rssi_history;
    uint8_t rssi_current;
    bool rssi_history_end;
    uint8_t ind_write;
    uint8_t ind_sin;
    SubGhzReadRAWStatus status;
    bool raw_send_only;
    float raw_threshold_rssi;
    bool not_showing_samples;
} SubGhzReadRAWModel;

void subghz_read_raw_set_callback(
    SubGhzReadRAW* subghz_read_raw,
    SubGhzReadRAWCallback callback,
    void* context);

SubGhzReadRAW* subghz_read_raw_alloc(bool raw_send_only);

void subghz_read_raw_free(SubGhzReadRAW* subghz_static);

void subghz_read_raw_add_data_statusbar(
    SubGhzReadRAW* instance,
    const char* frequency_str,
    const char* preset_str);

void subghz_read_raw_update_sample_write(SubGhzReadRAW* instance, size_t sample);

void subghz_read_raw_stop_send(SubGhzReadRAW* instance);

void subghz_read_raw_update_sin(SubGhzReadRAW* instance);

void subghz_read_raw_add_data_rssi(SubGhzReadRAW* instance, float rssi, bool trace);

void subghz_read_raw_set_status(
    SubGhzReadRAW* instance,
    SubGhzReadRAWStatus status,
    const char* file_name,
    float raw_threshold_rssi);

View* subghz_read_raw_get_view(SubGhzReadRAW* subghz_static);
