#include "pin.h"

#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stddef.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

#include "../desktop_i.h"

static const NotificationSequence sequence_pin_fail = {
    &message_display_backlight_on,

    &message_red_255,
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    &message_red_0,

    &message_delay_250,

    &message_red_255,
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    &message_red_0,
    NULL,
};

void desktop_pin_lock_error_notify() {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_pin_fail);
    furi_record_close(RECORD_NOTIFICATION);
}

uint32_t desktop_pin_lock_get_fail_timeout() {
    uint32_t pin_fails = furi_hal_rtc_get_pin_fails();
    if(pin_fails < 3) {
        return 0;
    }
    // Use for loop to avoid including pow() function (4kb of dfu flash)
    uint32_t mult = 1;
    for(size_t i = 0; i < pin_fails - 3; i++) {
        mult *= 2;
    }
    return 30 * mult;
}

bool desktop_pin_compare(const PinCode* pin_code1, const PinCode* pin_code2) {
    furi_assert(pin_code1);
    furi_assert(pin_code2);
    bool result = false;

    if(pin_code1->length == pin_code2->length) {
        result = !memcmp(pin_code1->data, pin_code2->data, pin_code1->length);
    }

    return result;
}

bool desktop_pin_is_valid(const PinCode* pin_code) {
    bool ok = pin_code->length >= MIN_PIN_SIZE && pin_code->length <= MAX_PIN_SIZE;
    for(size_t i = 0; ok && i < pin_code->length; i++) {
        ok = ok && (pin_code->data[i] == InputKeyUp || pin_code->data[i] == InputKeyDown ||
                    pin_code->data[i] == InputKeyRight || pin_code->data[i] == InputKeyLeft);
    }
    return ok;
}
