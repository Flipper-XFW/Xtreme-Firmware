#pragma once

#include <stdint.h>
#include <stdlib.h>

// Hacked together by @Willy-JL
// Structures docs and Nearby Action IDs from https://github.com/furiousMAC/continuity/
// Proximity Pairing IDs from https://github.com/ECTO-1A/AppleJuice/
// Custom adv logic and Airtag ID from https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

typedef enum {
    ContinuityTypeNearbyAction,
    ContinuityTypeProximityPairing,
    ContinuityTypeCount
} ContinuityType;

typedef union {
    struct {
        uint8_t action_type;
    } nearby_action;
    struct {
        uint16_t device_model;
    } proximity_pairing;
} ContinuityData;

typedef struct {
    ContinuityType type;
    ContinuityData data;
} ContinuityMessage;

const char* continuity_get_type_name(ContinuityType type);

size_t continuity_get_packet_size(ContinuityType type);

void continuity_generate_packet(const ContinuityMessage* msg, uint8_t* packet);
