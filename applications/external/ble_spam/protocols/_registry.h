#pragma once

#include "continuity.h"

union BleSpamMsg {
    ContinuityMsg continuity;
};

extern const BleSpamProtocol* ble_spam_protocols[];

extern const size_t ble_spam_protocols_count;
