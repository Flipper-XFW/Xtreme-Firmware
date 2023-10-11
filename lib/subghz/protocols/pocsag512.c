
//Docs:
//https://www.raveon.com/pdfiles/AN142(POCSAG).pdf  , https://habr.com/en/articles/438906/
// Thanks for @eS^, @Sil333033 for the help!
// Sender part by @HTotoo, Receiver is based on @Shmuma's work

#include "pocsag512.h"

#include <inttypes.h>
#include <lib/flipper_format/flipper_format_i.h>
#include <furi/core/string.h>

#define TAG "POCSAG512"

static const SubGhzBlockConst pocsag512_const = {
    .te_short = 1950,
    .te_long = 1950,
    .te_delta = 120,
};

// Minimal amount of sync bits (interleaving zeros and ones)
#define POCSAG512_MIN_SYNC_BITS 32
#define POCSAG512_CW_BITS 32
#define POCSAG512_CW_MASK 0xFFFFFFFF
#define POCSAG512_FRAME_SYNC_CODE 0x7CD215D8
#define POCSAG512_IDLE_CODE_WORD 0x7A89C197

#define POCSAG512_FUNC_NUM 0
#define POCSAG512_FUNC_ALERT1 1
#define POCSAG512_FUNC_ALERT2 2
#define POCSAG512_FUNC_ALPHANUM 3

#define POCSAG512_PRECOUNT 140

static const char* func_msg[] = {"\e#Num:\e# ", "\e#Alert\e#", "\e#Alert:\e# ", "\e#Msg:\e# "};
static const char* bcd_chars = "*U -)(";

struct SubGhzProtocolDecoderPocsag512 {
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

struct SubGhzProtocolEncoderPocsag512 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    PCSGBlockGeneric generic;
};

typedef struct SubGhzProtocolDecoderPocsag512 SubGhzProtocolDecoderPocsag512;
typedef struct SubGhzProtocolEncoderPocsag512 SubGhzProtocolEncoderPocsag512;

typedef enum {
    Pocsag512DecoderStepReset = 0,
    Pocsag512DecoderStepFoundSync,
    Pocsag512DecoderStepFoundPreamble,
    Pocsag512DecoderStepMessage,
} Pocsag512DecoderStep;

void* subghz_protocol_decoder_pocsag512_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolDecoderPocsag512* instance = malloc(sizeof(SubGhzProtocolDecoderPocsag512));
    instance->base.protocol = &subghz_protocol_pocsag512;
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

void subghz_protocol_decoder_pocsag512_free(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;
    furi_string_free(instance->msg);
    furi_string_free(instance->done_msg);
    free(instance);
}

void subghz_protocol_decoder_pocsag512_reset(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;

    instance->decoder.parser_step = Pocsag512DecoderStepReset;
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
    pocsag512_decode_address_word(SubGhzProtocolDecoderPocsag512* instance, uint32_t data) {
    instance->ric = (data >> 13);
    instance->ric = (instance->ric << 3) | (instance->codeword_idx >> 1);
    instance->func = (data >> 11) & 0b11;
}

static bool decode_message_alphanumeric(SubGhzProtocolDecoderPocsag512* instance, uint32_t data) {
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

static void decode_message_numeric(SubGhzProtocolDecoderPocsag512* instance, uint32_t data) {
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
    pocsag512_decode_message_word(SubGhzProtocolDecoderPocsag512* instance, uint32_t data) {
    switch(instance->func) {
    case POCSAG512_FUNC_ALERT2:
    case POCSAG512_FUNC_ALPHANUM:
        return decode_message_alphanumeric(instance, data);

    case POCSAG512_FUNC_NUM:
        decode_message_numeric(instance, data);
        return true;
    }
    return false;
}

// Function called when current message got decoded, but other messages might follow
static void pocsag512_message_done(SubGhzProtocolDecoderPocsag512* instance) {
    // append the message to the long-term storage string
    furi_string_printf(instance->generic.result_ric, "\e#RIC: %" PRIu32 "\e# | ", instance->ric);
    furi_string_cat_str(instance->generic.result_ric, func_msg[instance->func]);
    if(instance->func != POCSAG512_FUNC_ALERT1) {
        furi_string_cat(instance->done_msg, instance->msg);
    }
    furi_string_cat_str(instance->done_msg, " ");

    furi_string_cat(instance->generic.result_msg, instance->done_msg);

    // reset the state
    instance->char_bits = 0;
    instance->char_data = 0;
    furi_string_reset(instance->msg);
}

void subghz_protocol_decoder_pocsag512_feed(void* context, bool level, uint32_t duration) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;

    // reset state - waiting for 32 bits of interleaving 1s and 0s
    if(instance->decoder.parser_step == Pocsag512DecoderStepReset) {
        if(DURATION_DIFF(duration, pocsag512_const.te_short) < pocsag512_const.te_delta) {
            // POCSAG 512signals are inverted
            subghz_protocol_blocks_add_bit(&instance->decoder, !level);

            if(instance->decoder.decode_count_bit == POCSAG512_MIN_SYNC_BITS) {
                instance->decoder.parser_step = Pocsag512DecoderStepFoundSync;
            }
        } else if(instance->decoder.decode_count_bit > 0) {
            subghz_protocol_decoder_pocsag512_reset(context);
        }
        return;
    }

    int bits_count = duration / pocsag512_const.te_short;
    uint32_t extra = duration - pocsag512_const.te_short * bits_count;

    if(DURATION_DIFF(extra, pocsag512_const.te_short) < pocsag512_const.te_delta)
        bits_count++;
    else if(extra > pocsag512_const.te_delta) {
        // in non-reset state we faced the error signal - we reached the end of the packet, flush data
        if(furi_string_size(instance->done_msg) > 0) {
            if(instance->base.callback)
                instance->base.callback(&instance->base, instance->base.context);
        }
        subghz_protocol_decoder_pocsag512_reset(context);
        return;
    }

    uint32_t codeword;

    // handle state machine for every incoming bit
    while(bits_count-- > 0) {
        subghz_protocol_blocks_add_bit(&instance->decoder, !level);

        switch(instance->decoder.parser_step) {
        case Pocsag512DecoderStepFoundSync:
            if((instance->decoder.decode_data & POCSAG512_CW_MASK) == POCSAG512_FRAME_SYNC_CODE) {
                instance->decoder.parser_step = Pocsag512DecoderStepFoundPreamble;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
                //add to hex
                furi_string_cat(instance->generic.dataHex, "7CD215D8");
                instance->generic.bits += 32;
            }
            break;
        case Pocsag512DecoderStepFoundPreamble:
            // handle codewords
            if(instance->decoder.decode_count_bit == POCSAG512_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG512_CW_MASK);
                furi_string_cat_printf(instance->generic.dataHex, "%08" PRIX32, codeword);
                instance->generic.bits += 32;
                switch(codeword) {
                case POCSAG512_IDLE_CODE_WORD:
                    instance->codeword_idx++;
                    break;
                case POCSAG512_FRAME_SYNC_CODE:
                    instance->codeword_idx = 0;
                    break;
                default:
                    // Here we expect only address messages
                    if(codeword >> 31 == 0) {
                        pocsag512_decode_address_word(instance, codeword);
                        instance->decoder.parser_step = Pocsag512DecoderStepMessage;
                    }
                    instance->codeword_idx++;
                }
                instance->decoder.decode_count_bit = 0;
                instance->decoder.decode_data = 0UL;
            }
            break;

        case Pocsag512DecoderStepMessage:
            if(instance->decoder.decode_count_bit == POCSAG512_CW_BITS) {
                codeword = (uint32_t)(instance->decoder.decode_data & POCSAG512_CW_MASK);
                furi_string_cat_printf(instance->generic.dataHex, "%08" PRIX32, codeword);
                instance->generic.bits += 32;
                switch(codeword) {
                case POCSAG512_IDLE_CODE_WORD:
                    // Idle during the message stops the message
                    instance->codeword_idx++;
                    instance->decoder.parser_step = Pocsag512DecoderStepFoundPreamble;
                    pocsag512_message_done(instance);
                    break;
                case POCSAG512_FRAME_SYNC_CODE:
                    instance->codeword_idx = 0;
                    break;
                default:
                    // In this state, both address and message words can arrive
                    if(codeword >> 31 == 0) {
                        pocsag512_message_done(instance);
                        pocsag512_decode_address_word(instance, codeword);
                    } else {
                        if(!pocsag512_decode_message_word(instance, codeword)) {
                            instance->decoder.parser_step = Pocsag512DecoderStepFoundPreamble;
                            pocsag512_message_done(instance);
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

uint8_t subghz_protocol_decoder_pocsag512_get_hash_data(void* context) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;
    uint8_t hash = 0;
    for(size_t i = 0; i < furi_string_size(instance->done_msg); i++)
        hash ^= furi_string_get_char(instance->done_msg, i);
    return hash;
}

SubGhzProtocolStatus subghz_protocol_decoder_pocsag512_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;
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
    subghz_protocol_decoder_pocsag512_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;
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

void subhz_protocol_decoder_pocsag512_get_string(void* context, FuriString* output) {
    furi_assert(context);
    SubGhzProtocolDecoderPocsag512* instance = context;
    furi_string_cat_printf(output, "%s\r\n", instance->generic.protocol_name);
    furi_string_cat_printf(output, "Addr: %lu\r\n", instance->ric);
    furi_string_cat(output, instance->done_msg);
}

LevelDuration subghz_protocol_pocsag512_encoder_yield(void* context) {
    SubGhzProtocolEncoderPocsag512* instance = context;
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

void subghz_protocol_pocsag512_encoder_stop(void* context) {
    SubGhzProtocolEncoderPocsag512* instance = context;
    instance->encoder.is_running = false;
}

void* subghz_protocol_pocsag512_encoder_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderPocsag512* instance = malloc(sizeof(SubGhzProtocolEncoderPocsag512));

    instance->base.protocol = &subghz_protocol_pocsag512;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 2;
    instance->encoder.size_upload = POCSAG512_PRECOUNT; //will be recalculated later
    instance->encoder.upload =
        NULL; //malloc(instance->encoder.size_upload * sizeof(LevelDuration)); //will alloc when deserialized, since it is dynamic sized
    instance->encoder.is_running = false;
    return instance;
}

void subghz_protocol_pocsag512_encoder_free(void* context) {
    furi_assert(context);
    SubGhzProtocolEncoderPocsag512* instance = context;
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
void subghz_protocol_pocsag512_upload_deefer(
    SubGhzProtocolEncoderPocsag512* instance,
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
        ((*lastPos) < 0), (uint32_t)pocsag512_const.te_short * abs((int)(*lastPos)));
    if(!finishLast) {
        *lastPos = (updown) ? 1 : -1;
    }
}

bool subghz_protocol_pocsag512_encoder_upload(SubGhzProtocolEncoderPocsag512* instance) {
    furi_assert(instance);
    if(instance->encoder.upload == NULL) {
        return false; //malloc error before
    }
    uint32_t i = 0;
    int16_t lastPos = 0; //to store last position ( + high / - low and the count of it)
    //add pre
    for(uint8_t ii = 0; ii < POCSAG512_PRECOUNT / 2; ++ii) {
        subghz_protocol_pocsag512_upload_deefer(instance, &i, true, &lastPos, false);
        subghz_protocol_pocsag512_upload_deefer(instance, &i, false, &lastPos, false);
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
        subghz_protocol_pocsag512_upload_deefer(
            instance, &i, (((value >> 3) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag512_upload_deefer(
            instance, &i, (((value >> 2) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag512_upload_deefer(
            instance, &i, (((value >> 1) & 1) == 1), &lastPos, false);
        subghz_protocol_pocsag512_upload_deefer(
            instance, &i, (((value >> 0) & 1) == 1), &lastPos, false);
    }
    subghz_protocol_pocsag512_upload_deefer(
        instance, &i, false, &lastPos, true); //finishlast packet!
    return true;
}

SubGhzProtocolStatus
    subghz_protocol_pocsag512_encoder_deserialize(void* context, FlipperFormat* flipper_format) {
    furi_assert(context);
    SubGhzProtocolEncoderPocsag512* instance = context;
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
        instance->encoder.size_upload = instance->generic.bits + POCSAG512_PRECOUNT;
        if(instance->encoder.upload != NULL) {
            free(instance->encoder.upload);
        }
        instance->encoder.upload = malloc(instance->encoder.size_upload * sizeof(LevelDuration));
        //optional parameter
        flipper_format_read_uint32(
            flipper_format, "Repeat", (uint32_t*)&instance->encoder.repeat, 2);

        if(!subghz_protocol_pocsag512_encoder_upload(instance)) {
            ret = SubGhzProtocolStatusErrorEncoderGetUpload;
            break;
        }
        instance->encoder.is_running = true;
    } while(false);

    return ret;
}

const SubGhzProtocolDecoder subghz_protocol_pocsag512_decoder = {
    .alloc = subghz_protocol_decoder_pocsag512_alloc,
    .free = subghz_protocol_decoder_pocsag512_free,
    .reset = subghz_protocol_decoder_pocsag512_reset,
    .feed = subghz_protocol_decoder_pocsag512_feed,
    .get_hash_data = subghz_protocol_decoder_pocsag512_get_hash_data,
    .serialize = subghz_protocol_decoder_pocsag512_serialize,
    .deserialize = subghz_protocol_decoder_pocsag512_deserialize,
    .get_string = subhz_protocol_decoder_pocsag512_get_string,
};

const SubGhzProtocolEncoder subghz_protocol_pocsag512_encoder = {
    .alloc = subghz_protocol_pocsag512_encoder_alloc,
    .free = subghz_protocol_pocsag512_encoder_free,
    .deserialize = subghz_protocol_pocsag512_encoder_deserialize,
    .stop = subghz_protocol_pocsag512_encoder_stop,
    .yield = subghz_protocol_pocsag512_encoder_yield,
};

const SubGhzProtocol subghz_protocol_pocsag512 = {
    .name = SUBGHZ_PROTOCOL_POCSAG512_NAME,
    .type = SubGhzProtocolTypeStatic,
    .flag = SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Save |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Send,

    .decoder = &subghz_protocol_pocsag512_decoder,
    .encoder = &subghz_protocol_pocsag512_encoder,
};
