#pragma once

#include "continuity.h"
#include "fastpair.h"
#include "easysetup.h"
#include "swiftpair.h"

typedef enum {
    ProtocolModeRandom,
    ProtocolModeValue,
} ProtocolMode;

struct ProtocolCfg {
    ProtocolMode mode;
    union {
        ContinuityCfg continuity;
        FastpairCfg fastpair;
        EasysetupCfg easysetup;
        SwiftpairCfg swiftpair;
    } specific;
};

extern const Protocol* protocols[];

extern const size_t protocols_count;

typedef struct {
    bool random_mac;
    ProtocolCfg cfg;
} Payload;

struct Attack {
    const char* title;
    const char* text;
    const Protocol* protocol;
    Payload payload;
};
