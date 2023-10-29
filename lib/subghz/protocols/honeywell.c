#include "honeywell.h"
#include <lib/toolbox/manchester_decoder.h>

#define TAG "SubGhzProtocolHoneywell"

uint16_t subghz_protocol_honeywell_crc16(
    uint8_t const message[],
    unsigned nBytes,
    uint16_t polynomial,
    uint16_t init) {
    uint16_t remainder = init;
    unsigned byte, bit;

    for(byte = 0; byte < nBytes; ++byte) {
        remainder ^= message[byte] << 8;
        for(bit = 0; bit < 8; ++bit) {
            if(remainder & 0x8000) {
                remainder = (remainder << 1) ^ polynomial;
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    return remainder;
}

void subghz_protocol_decoder_honeywell_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;
    free(instance);
}

void subghz_protocol_decoder_honeywell_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;
    instance->decoder.decode_data = 0;
    instance->decoder.decode_count_bit = 0;
}

void subghz_protocol_decoder_honeywell_addbit(void* context, bool data) {
    SubGhzProtocolDecoderHoneywell* instance = context;
    instance->decoder.decode_data = (instance->decoder.decode_data << 1) | data;
    instance->decoder.decode_count_bit++;

    uint16_t preamble = (instance->decoder.decode_data >> 48) & 0xFFFF;
    //if (preamble == 0x7fff) instance->decoder.decode_data = (instance->decoder.decode_data << 1); //shifted by 1 bit. flipper fault...

    preamble = (instance->decoder.decode_data >> 48) & 0xFFFF; //recalc it
    if(preamble == 0b0011111111111110 || preamble == 0b0111111111111110 ||
       preamble == 0b1111111111111110) {
        uint8_t datatocrc[4];
        datatocrc[0] = (instance->decoder.decode_data >> 40) & 0xFFFF;
        datatocrc[1] = (instance->decoder.decode_data >> 32) & 0xFFFF;
        datatocrc[2] = (instance->decoder.decode_data >> 24) & 0xFFFF;
        datatocrc[3] = (instance->decoder.decode_data >> 16) & 0xFFFF;

        uint8_t channel = (instance->decoder.decode_data >> 44) & 0xF;
        uint16_t crc_calc = 0;

        if(channel == 0x2 || channel == 0x4 || channel == 0xA) {
            // 2GIG brand
            crc_calc = subghz_protocol_honeywell_crc16(datatocrc, 4, 0x8050, 0);
        } else { // channel == 0x8
            crc_calc = subghz_protocol_honeywell_crc16(datatocrc, 4, 0x8005, 0);
        }
        uint16_t crc = instance->decoder.decode_data & 0xFFFF;
        if(crc == crc_calc) {
            //the data is good. process it.
            instance->generic.data = instance->decoder.decode_data;
            instance->generic.data_count_bit =
                instance->decoder
                    .decode_count_bit; //maybe set it to 64, and hack the first 2 bits to 1! will see if replay needs it
            instance->generic.serial = (instance->decoder.decode_data >> 24) & 0xFFFFF;
            instance->generic.btn = (instance->decoder.decode_data >> 16) &
                                    0xFF; //not exactly button, but can contain btn data too.
            if(instance->base.callback)
                instance->base.callback(&instance->base, instance->base.context);
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
        } else {
            return;
        }
    }
}

void subghz_protocol_decoder_honeywell_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;

    ManchesterEvent event = ManchesterEventReset;
    if(!level) {
        if(DURATION_DIFF(duration, subghz_protocol_honeywell_const.te_short) <
           subghz_protocol_honeywell_const.te_delta) {
            event = ManchesterEventShortLow;
        } else if(
            DURATION_DIFF(duration, subghz_protocol_honeywell_const.te_long) <
            subghz_protocol_honeywell_const.te_delta * 2) {
            event = ManchesterEventLongLow;
        }
    } else {
        if(DURATION_DIFF(duration, subghz_protocol_honeywell_const.te_short) <
           subghz_protocol_honeywell_const.te_delta) {
            event = ManchesterEventShortHigh;
        } else if(
            DURATION_DIFF(duration, subghz_protocol_honeywell_const.te_long) <
            subghz_protocol_honeywell_const.te_delta * 2) {
            event = ManchesterEventLongHigh;
        }
    }
    if(event != ManchesterEventReset) {
        bool data;
        bool data_ok = manchester_advance(
            instance->manchester_saved_state, event, &instance->manchester_saved_state, &data);
        if(data_ok) {
            subghz_protocol_decoder_honeywell_addbit(instance, data);
        }
    } else {
        instance->decoder.decode_data = 0;
        instance->decoder.decode_count_bit = 0;
    }
}

uint8_t subghz_protocol_decoder_honeywell_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;
    return subghz_protocol_blocks_get_hash_data(
        &instance->decoder, (instance->decoder.decode_count_bit / 8) + 1);
}

SubGhzProtocolStatus subghz_protocol_decoder_honeywell_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;
    return subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    subghz_protocol_decoder_honeywell_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        subghz_protocol_honeywell_const.min_count_bit_for_found);
}

void subghz_protocol_decoder_honeywell_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderHoneywell* instance = context;

    uint8_t channel = (instance->generic.data >> 44) & 0xF;
    uint8_t contact = (instance->generic.btn & 0x80) >> 7;
    uint8_t tamper = (instance->generic.btn & 0x40) >> 6;
    uint8_t reed = (instance->generic.btn & 0x20) >> 5;
    uint8_t alarm = (instance->generic.btn & 0x10) >> 4;
    uint8_t battery_low = (instance->generic.btn & 0x08) >> 3;
    uint8_t heartbeat = (instance->generic.btn & 0x04) >> 2;

    furi_string_cat_printf(
        output,
        "%s\r\n%dbit\r\n"
        "Sn:%07lu Ch:%u  Bat:%d  Hb: %d\r\n"
        "L1: %u, L2: %u, L3: %u, L4: %u\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        instance->generic.serial,
        channel,
        battery_low,
        heartbeat,
        contact,
        reed,
        alarm,
        tamper);
}

void* subghz_protocol_decoder_honeywell_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderHoneywell* instance = malloc(sizeof(SubGhzProtocolDecoderHoneywell));
    instance->base.protocol = &subghz_protocol_honeywell;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

const SubGhzProtocolDecoder subghz_protocol_honeywell_decoder = {
    .alloc = subghz_protocol_decoder_honeywell_alloc,
    .free = subghz_protocol_decoder_honeywell_free,
    .feed = subghz_protocol_decoder_honeywell_feed,
    .reset = subghz_protocol_decoder_honeywell_reset,
    .get_hash_data = subghz_protocol_decoder_honeywell_get_hash_data,
    .serialize = subghz_protocol_decoder_honeywell_serialize,
    .deserialize = subghz_protocol_decoder_honeywell_deserialize,
    .get_string = subghz_protocol_decoder_honeywell_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_honeywell_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};

const SubGhzProtocol subghz_protocol_honeywell = {
    .name = SUBGHZ_PROTOCOL_HONEYWELL_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_315 | SubGhzProtocolFlag_868 |
            SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load |
            SubGhzProtocolFlag_Save,
    .encoder = &subghz_protocol_honeywell_encoder,
    .decoder = &subghz_protocol_honeywell_decoder,

};
