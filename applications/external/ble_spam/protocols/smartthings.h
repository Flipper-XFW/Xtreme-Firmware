#pragma once
#include "_base.h"

// Hacked together by @Willy-JL and @Spooks4576
// Research by @Spooks4576

typedef enum {
    SmartthingsTypeBuds,
    SmartthingsTypeWatch,
    SmartthingsTypeCOUNT,
} SmartthingsType;

typedef struct {
    SmartthingsType type;
    union {
        struct {
            uint32_t model;
        } buds;
        struct {
            uint8_t model;
        } watch;
    } data;
} SmartthingsCfg;

extern const Protocol protocol_smartthings;
