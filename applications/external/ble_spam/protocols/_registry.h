#pragma once

#include "continuity.h"
#include "fastpair.h"
#include "swiftpair.h"

union ProtocolCfg {
    ContinuityCfg continuity;
    FastpairCfg fastpair;
    SwiftpairCfg swiftpair;
};

extern const Protocol* protocols[];

extern const size_t protocols_count;

typedef struct {
    bool random_mac;
    ProtocolCfg cfg;
} Payload;
