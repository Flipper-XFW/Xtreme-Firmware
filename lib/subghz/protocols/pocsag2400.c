
//Docs:
//https://www.raveon.com/pdfiles/AN142(POCSAG).pdf  , https://habr.com/en/articles/438906/
// Thanks for @eS^ for the help!

#include "pocsag2400.h"

#include <inttypes.h>
#include <lib/flipper_format/flipper_format_i.h>
#include <furi/core/string.h>

#define TAG "POCSAG2400"

static const SubGhzBlockConst pocsag2400_const = {
    .te_short = 410,
    .te_long = 410,
    .te_delta = 40,
};

// Minimal amount of sync bits (interleaving zeros and ones)
#define POCSAG2400_MIN_SYNC_BITS 32
#define POCSAG2400_CW_BITS 32
#define POCSAG2400_CW_MASK 0xFFFFFFFF
#define POCSAG2400_FRAME_SYNC_CODE 0x7CD215D8
#define POCSAG2400_IDLE_CODE_WORD 0x7A89C197

#define POCSAG2400_FUNC_NUM 0
#define POCSAG2400_FUNC_ALERT1 1
#define POCSAG2400_FUNC_ALERT2 2
#define POCSAG2400_FUNC_ALPHANUM 3

#define POCSAG2400_PRECOUNT 140

static const char* func_msg[] = {"\e#Num:\e# ", "\e#Alert\e#", "\e#Alert:\e# ", "\e#Msg:\e# "};
static const char* bcd_chars = "*U -)(";

struct SubGhzProtocolDecoderPocsag2400 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    PCSGBlockGeneric generic;

    uint8_t codeword_idx;
    uint32_t ric;
    uint8_t func;

    // partially decoded character
    uint8_t char_bits;
    uint8_t char_data;

    // message being decoded
    FuriString* msg;

    // Done messages, ready to be serialized/deserialized
    FuriString* done_msg;
};

struct SubGhzProtocolEncoderPocsag2400 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    PCSGBlockGeneric generic;
};

typedef struct SubGhzProtocolDecoderPocsag2400 SubGhzProtocolDecoderPocsag2400;
typedef struct SubGhzProtocolEncoderPocsag2400 SubGhzProtocolEncoderPocsag2400;

typedef enum {
    Pocsag2400DecoderStepReset = 0,
    Pocsag2400DecoderStepFoundSync,
    Pocsag2400DecoderStepFoundPreamble,
    Pocsag2400DecoderStepMessage,
} Pocsag2400DecoderStep;

void* subghz_protocol_decoder_pocsag2400_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolDecoderPocsag2400* instance = malloc(sizeof(SubGhzProtocolDecoderPocsag2400));
    instance->base.protocol = &subghz_protocol_pocsag2400;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->msg = furi_string_alloc();
    instance->done_msg = furi_string_alloc();
    if(instance->generic.result_msg == NULL) {
        instance->generic.result_msg = furi_string_alloc();
    }
    if(instance->generic.result_ric == NULL) {
        instance->generic.result_ric = furi_string_alloc();
    }
    if(instance->generic.dataHex == NULL) {
        instance->generic.dataHex = furi_string_alloc();
    }
    instance->generic.bits = 0;
    return instance;
}

void subghz_protocol_decoder_pocsag2400_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;
    furi_string_free(instance->msg);
    furi_string_free(instance->done_msg);
    free(instance);
}

void subghz_protocol_decoder_pocsag2400_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;

    instance->decoder.parser_step = Pocsag2400DecoderStepReset;
    instance->decoder.decode_data = 0UL;
    instance->decoder.decode_count_bit = 0;
    instance->codeword_idx = 0;
    instance->char_bits = 0;
    instance->char_data = 0;
    instance->generic.bits = 0;
    furi_string_reset(instance->msg);
    furi_string_reset(instance->done_msg);
    furi_string_reset(instance->generic.dataHex);
    furi_string_reset(instance->generic.result_msg);
    furi_string_reset(instance->generic.result_ric);
}

static void
    pocsag2400_decode_address_word(SubGhzProtocolDecoderPocsag2400* instance, uint32_t data) {
    instance->ric = (data >> 13);
    instance->ric = (instance->ric << 3) | (instance->codeword_idx >> 1);
    instance->func = (data >> 11) & 0b11;
}

static bool decode_message_alphanumeric(SubGhzProtocolDecoderPocsag2400* instance, uint32_t data) {
    for(uint8_t i = 0; i < 20; i++) {
        instance->char_data >>= 1;
        if(data & (1 << 30)) {
            instance->char_data |= 1 << 6;
        }
        instance->char_bits++;
        if(instance->char_bits == 7) {
            if(instance->char_data == 0) return false;
            furi_string_push_back(instance->msg, instance->char_data);
            instance->char_data = 0;
            instance->char_bits = 0;
        }
        data <<= 1;
    }
    return true;
}

static void decode_message_numeric(SubGhzProtocolDecoderPocsag2400* instance, uint32_t data) {
    // 5 groups with 4 bits each
    uint8_t val;
    for(uint8_t i = 0; i < 5; i++) {
        val = (data >> (27 - i * 4)) & 0b1111;
        // reverse the order of 4 bits
        val = (val & 0x5) << 1 | (val & 0xA) >> 1;
        val = (val & 0x3) << 2 | (val & 0xC) >> 2;

        if(val <= 9)
            val += '0';
        else
            val = bcd_chars[val - 10];
        furi_string_push_back(instance->msg, val);
    }
}

// decode message word, maintaining instance state for partial decoding. Return true if more data
// might follow or false if end of message reached.
static bool
    pocsag2400_decode_message_word(SubGhzProtocolDecoderPocsag2400* instance, uint32_t data) {
    switch(instance->func) {
    case POCSAG2400_FUNC_ALERT2:
    case POCSAG2400_FUNC_ALPHANUM:
        return decode_message_alphanumeric(instance, data);

    case POCSAG2400_FUNC_NUM:
        decode_message_numeric(instance, data);
        return true;
    }
    return false;
}

// Function called when current message got decoded, but other messages might follow
static void pocsag2400_message_done(SubGhzProtocolDecoderPocsag2400* instance) {
    // append the message to the long-term storage string
    furi_string_printf(instance->generic.result_ric, "\e#RIC: %" PRIu32 "\e# | ", instance->ric);
    furi_string_cat_str(instance->generic.result_ric, func_msg[instance->func]);
    if(instance->func != POCSAG2400_FUNC_ALERT1) {
        furi_string_cat(instance->done_msg, instance->msg);
    }
    furi_string_cat_str(instance->done_msg, " ");

    furi_string_cat(instance->generic.result_msg, instance->done_msg);

    // reset the state
    instance->char_bits = 0;
    instance->char_data = 0;
    furi_string_reset(instance->msg);
}

void subghz_protocol_decoder_pocsag2400_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;

    // reset state - waiting for 32 bits of interleaving 1s and 0s
    if(instance->decoder.parser_step == Pocsag2400DecoderStepReset) {
        if(DURATION_DIFF(duration, pocsag2400_const.te_short) < pocsag2400_const.te_delta) {
            // POCSAG 2400signals are inverted
            subghz_protocol_blocks_add_bit(&instance->decoder, !level);

            if(instance->decoder.decode_count_bit == POCSAG2400_MIN_SYNC_BITS) {
                instance->decoder.parser_step = Pocsag2400DecoderStepFoundSync;
            }
        } else if(instance->decoder.decode_count_bit > 0) {
            subghz_protocol_decoder_pocsag2400_reset(context);
        }
        return;
    }

    int bits_count = duration / pocsag2400_const.te_short;
    uint32_t extra = duration - pocsag2400_const.te_short * bits_count;

    if(DURATION_DIFF(extra, pocsag2400_const.te_short) < pocsag2400_const.te_delta)
        bits_count++;
    else if(extra > pocsag2400_const.te_delta) {
        // in non-reset state we faced the error signal - we reached the end of the packet, flush data
        if(furi_string_size(instance->done_msg) > 0) {
            if(instance->base.callback)
                instance->base.callback(&instance->base, instance->base.context);
        }
        subghz_protocol_decoder_pocsag2400_reset(context);
        return;
    }

    uint32_t codeword;

    // handle state machine for every incoming bit
    while(bits_count-- > 0) {
        subghz_protocol_blocks_add_bit(&instance->decoder, !level);

        switch(instance->decoder.parser_step) {
        case Pocsag2400DecoderStepFoundSync:
            if((instance->decoder.decode_data & POCSAG2400_CW_MASK) ==
               POCSAG2400_FRAME_SYNC_CODE) {
                instance->decoder.parser_step = Pocsag2400DecoderStepFoundPreamble;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
                //add to hex
                furi_string_cat(instance->generic.dataHex, "7CD215D8");
                instance->generic.bits += 32;
            }
            break;
        case Pocsag2400DecoderStepFoundPreamble:
            // handle codewords
            if(instance->decoder.decode_count_bit == POCSAG2400_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG2400_CW_MASK);
                furi_string_cat_printf(instance->generic.dataHex, "%08" PRIX32, codeword);
                instance->generic.bits += 32;
                switch(codeword) {
                case POCSAG2400_IDLE_CODE_WORD:
                    instance->codeword_idx++;
                    break;
                case POCSAG2400_FRAME_SYNC_CODE:
                    instance->codeword_idx = 0;
                    break;
                default:
                    // Here we expect only address messages
                    if(codeword >> 31 == 0) {
                        pocsag2400_decode_address_word(instance, codeword);
                        instance->decoder.parser_step = Pocsag2400DecoderStepMessage;
                    }
                    instance->codeword_idx++;
                }
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;

        case Pocsag2400DecoderStepMessage:
            if(instance->decoder.decode_count_bit == POCSAG2400_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG2400_CW_MASK);
                furi_string_cat_printf(instance->generic.dataHex, "%08" PRIX32, codeword);
                instance->generic.bits += 32;
                switch(codeword) {
                case POCSAG2400_IDLE_CODE_WORD:
                    // Idle during the message stops the message
                    instance->codeword_idx++;
                    instance->decoder.parser_step = Pocsag2400DecoderStepFoundPreamble;
                    pocsag2400_message_done(instance);
                    break;
                case POCSAG2400_FRAME_SYNC_CODE:
                    instance->codeword_idx = 0;
                    break;
                default:
                    // In this state, both address and message words can arrive
                    if(codeword >> 31 == 0) {
                        pocsag2400_message_done(instance);
                        pocsag2400_decode_address_word(instance, codeword);
                    } else {
                        if(!pocsag2400_decode_message_word(instance, codeword)) {
                            instance->decoder.parser_step = Pocsag2400DecoderStepFoundPreamble;
                            pocsag2400_message_done(instance);
                        }
                    }
                    instance->codeword_idx++;
                }
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;
        }
    }
}

uint8_t subghz_protocol_decoder_pocsag2400_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;
    uint8_t hash = 0;
    for(size_t i = 0; i < furi_string_size(instance->done_msg); i++)
        hash ^= furi_string_get_char(instance->done_msg, i);
    return hash;
}

SubGhzProtocolStatus subghz_protocol_decoder_pocsag2400_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;
    uint32_t msg_len;

    if(SubGhzProtocolStatusOk !=
       pcsg_block_generic_serialize(&instance->generic, flipper_format, preset))
        return SubGhzProtocolStatusError;

    msg_len = furi_string_size(instance->done_msg);
    if(!flipper_format_write_uint32(flipper_format, "MsgLen", &msg_len, 1)) {
        FURI_LOG_E(TAG, "Error adding MsgLen");
        return SubGhzProtocolStatusError;
    }

    uint8_t* s = (uint8_t*)furi_string_get_cstr(instance->done_msg);
    if(!flipper_format_write_hex(flipper_format, "Msg", s, msg_len)) {
        FURI_LOG_E(TAG, "Error adding Msg");
        return SubGhzProtocolStatusError;
    }
    return SubGhzProtocolStatusOk;
}

SubGhzProtocolStatus
    subghz_protocol_decoder_pocsag2400_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    uint32_t msg_len;
    uint8_t* buf;

    do {
        if(SubGhzProtocolStatusOk !=
           pcsg_block_generic_deserialize(&instance->generic, flipper_format)) {
            break;
        }

        if(!flipper_format_read_uint32(flipper_format, "MsgLen", &msg_len, 1)) {
            FURI_LOG_E(TAG, "Missing MsgLen");
            break;
        }

        buf = malloc(msg_len);
        if(!flipper_format_read_hex(flipper_format, "Msg", buf, msg_len)) {
            FURI_LOG_E(TAG, "Missing Msg");
            free(buf);
            break;
        }
        furi_string_set_strn(instance->done_msg, (const char*)buf, msg_len);
        free(buf);

        ret = SubGhzProtocolStatusOk;
    } while(false);
    return ret;
}

void subhz_protocol_decoder_pocsag2400_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag2400* instance = context;
    furi_string_cat_printf(output, "%s\r\n", instance->generic.protocol_name);
    furi_string_cat_printf(output, "Addr: %lu\r\n", instance->ric);
    furi_string_cat(output, instance->done_msg);
}

LevelDuration subghz_protocol_pocsag2400_encoder_yield(void* context) {
    SubGhzProtocolEncoderPocsag2400* instance = context;
    if(instance->encoder.repeat == 0 || !instance->encoder.is_running) {
        instance->encoder.is_running = false;
        return level_duration_reset();
    }
    LevelDuration ret = instance->encoder.upload[instance->encoder.front];
    if(++instance->encoder.front == instance->encoder.size_upload) {
        instance->encoder.repeat--;
        instance->encoder.front = 0;
    }
    return ret;
}

void subghz_protocol_pocsag2400_encoder_stop(void* context) {
    SubGhzProtocolEncoderPocsag2400* instance = context;
    instance->encoder.is_running = false;
}

void* subghz_protocol_pocsag2400_encoder_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderPocsag2400* instance = malloc(sizeof(SubGhzProtocolEncoderPocsag2400));

    instance->base.protocol = &subghz_protocol_pocsag2400;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 2;
    instance->encoder.size_upload = POCSAG2400_PRECOUNT; //will be recalculated later
    instance->encoder.upload =
        NULL; //malloc(instance->encoder.size_upload * sizeof(LevelDuration)); //will alloc when deserialized, since it is dynamic sized
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_pocsag2400_encoder_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderPocsag2400* instance = context;
    if(instance->encoder.upload != NULL) {
        free(instance->encoder.upload);
    }
    free(instance);
}

/*
    Params: instance, the proto's instance
    i: the Level counter
    updown - the signal to send
    lastPos - the count of the ups / downs. negative: downs count, positive ups count.
    finishLast - true if it is AFTER the last bit, so send the collected ones
*/
void subghz_protocol_pocsag2400_upload_deefer(
    SubGhzProtocolEncoderPocsag2400* instance,
    uint32_t* i,
    bool updown,
    int16_t* lastPos,
    bool finishLast) {
    bool changed = false;
    if((*lastPos > 0 && !updown) || (*lastPos < 0 && updown)) changed = true;

    if(!finishLast && !changed) {
        if(*lastPos >= 0 && updown) ++(*lastPos);
        if(*lastPos <= 0 && !updown) --(*lastPos);
        return;
    }
    //here maybe changed OR finishlast
    //add packet
    instance->encoder.upload[(*i)++] = level_duration_make(
        ((*lastPos) < 0), (uint32_t)pocsag2400_const.te_short * abs((int)(*lastPos)));
    if(!finishLast) {
        *lastPos = (updown) ? 1 : -1;
    }
}

bool subghz_protocol_pocsag2400_encoder_upload(SubGhzProtocolEncoderPocsag2400* instance) {
    furi_assert(instance);
    if(instance->encoder.upload == NULL) {
        return false; //malloc error before
    }
    uint32_t i = 0;
    int16_t lastPos = 0; //to store last position ( + high / - low and the count of it)
    //add pre
    for(uint8_t ii = 0; ii < POCSAG2400_PRECOUNT / 2; ++ii) {
        subghz_protocol_pocsag2400_upload_deefer(instance, &i, true, &lastPos, false);
        subghz_protocol_pocsag2400_upload_deefer(instance, &i, false, &lastPos, false);
    }
    size_t dhl = furi_string_size(instance->generic.dataHex);

    for(size_t ii = 0; ii < dhl; ii++) {
        char ch = furi_string_get_char(instance->generic.dataHex, ii);

        uint8_t value = 0;
        if(ch >= 'A' && ch <= 'F') {
            value = ch - 'A' + 10;
        } else if(ch >= '0' && ch <= '9') {
            value = ch - '0';
        }
        subghz_protocol_pocsag2400_upload_deefer(
            instance, &i, (((value >> 3) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag2400_upload_deefer(
            instance, &i, (((value >> 2) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag2400_upload_deefer(
            instance, &i, (((value >> 1) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag2400_upload_deefer(
            instance, &i, (((value >> 0) & 1) == 1), &lastPos, false);
    }
    subghz_protocol_pocsag2400_upload_deefer(
        instance, &i, false, &lastPos, true); //finishlast packet!
    return true;
}

SubGhzProtocolStatus
    subghz_protocol_pocsag2400_encoder_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderPocsag2400* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;
    do {
        ret = pcsg_block_generic_deserialize(&instance->generic, flipper_format);
        if(ret != SubGhzProtocolStatusOk) {
            break;
        }
        //check bit count
        if((instance->generic.bits <= 32)) {
            FURI_LOG_E(TAG, "Wrong number of bits in key");
            ret = SubGhzProtocolStatusErrorValueBitCount;
            break;
        }
        instance->encoder.size_upload = instance->generic.bits + POCSAG2400_PRECOUNT;
        if(instance->encoder.upload != NULL) {
            free(instance->encoder.upload);
        }
        instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
        //optional parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 2);

        if(!subghz_protocol_pocsag2400_encoder_upload(instance)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

const SubGhzProtocolDecoder subghz_protocol_pocsag2400_decoder = {
    .alloc = subghz_protocol_decoder_pocsag2400_alloc,
    .free = subghz_protocol_decoder_pocsag2400_free,
    .reset = subghz_protocol_decoder_pocsag2400_reset,
    .feed = subghz_protocol_decoder_pocsag2400_feed,
    .get_hash_data = subghz_protocol_decoder_pocsag2400_get_hash_data,
    .serialize = subghz_protocol_decoder_pocsag2400_serialize,
    .deserialize = subghz_protocol_decoder_pocsag2400_deserialize,
    .get_string = subhz_protocol_decoder_pocsag2400_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_pocsag2400_encoder = {
    .alloc = subghz_protocol_pocsag2400_encoder_alloc,
    .free = subghz_protocol_pocsag2400_encoder_free,
    .deserialize = subghz_protocol_pocsag2400_encoder_deserialize,
    .stop = subghz_protocol_pocsag2400_encoder_stop,
    .yield = subghz_protocol_pocsag2400_encoder_yield,
};

const SubGhzProtocol subghz_protocol_pocsag2400 = {
    .name = SUBGHZ_PROTOCOL_POCSAG2400_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Save |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_pocsag2400_decoder,
    .encoder = &subghz_protocol_pocsag2400_encoder,
};
