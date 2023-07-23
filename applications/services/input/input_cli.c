#include "input_i.h"

#include <furi.h>
#include <cli/cli.h>
#include <toolbox/args.h>

static void input_cli_usage() {
    printf("Usage:\r\n");
    printf("input <cmd> <args>\r\n");
    printf("Cmd list:\r\n");
    printf("\tdump\t\t\t - dump input events\r\n");
    printf("\tsend <key> <type>\t - send input event\r\n");
    printf("\tkeyboard\t - read keyboard input and control flipper with it\r\n");
}

static void input_cli_dump_events_callback(const void* value, void* ctx) {
    furi_assert(value);
    furi_assert(ctx);
    FuriMessageQueue* input_queue = ctx;
    furi_message_queue_put(input_queue, value, FuriWaitForever);
}

static void input_cli_dump(Cli* cli, FuriString* args, Input* input) {
    UNUSED(args);
    FuriMessageQueue* input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(input->event_pubsub, input_cli_dump_events_callback, input_queue);

    InputEvent input_event;
    printf("Press CTRL+C to stop\r\n");
    while(!cli_cmd_interrupt_received(cli)) {
        if(furi_message_queue_get(input_queue, &input_event, 100) == FuriStatusOk) {
            printf(
                "key: %s type: %s\r\n",
                input_get_key_name(input_event.key),
                input_get_type_name(input_event.type));
        }
    }

    furi_pubsub_unsubscribe(input->event_pubsub, input_subscription);
    furi_message_queue_free(input_queue);
}

static void input_cli_keyboard(Cli* cli, FuriString* args, Input* input) {
    UNUSED(args);

    printf("Press CTRL+C to stop\r\n");
    while(cli_is_connected(cli)) {
        char in_chr = cli_getc(cli);
        if(in_chr == CliSymbolAsciiETX) break;

        InputKey send_key = InputKeyMAX;

        switch(in_chr) {
            case CliSymbolAsciiEsc:
                if(!cli_read(cli, (uint8_t*)&in_chr, 1) || in_chr != '[') break;
                if(!cli_read(cli, (uint8_t*)&in_chr, 1)) break;
                if(in_chr >= 'A' && in_chr <= 'D') {
                    send_key = in_chr - 'A';
                }
                break;
            case CliSymbolAsciiBackspace:
                send_key = InputKeyBack;
                break;
            case CliSymbolAsciiCR:
                send_key = InputKeyOk;
                break;
            case CliSymbolAsciiTab:
                break;
            default:
                printf("ignoring key: %u\r\n", in_chr);
                break;
        }

        if(send_key != InputKeyMAX) {
            input_fake_event(input, send_key, InputTypeShort);
        }
    }
}

static void input_cli_send_print_usage() {
    printf("Invalid arguments. Usage:\r\n");
    printf("\tinput send <key> <type>\r\n");
    printf("\t\t <key>\t - one of 'up', 'down', 'left', 'right', 'back', 'ok'\r\n");
    printf("\t\t <type>\t - one of 'press', 'release', 'short', 'long'\r\n");
}

static void input_cli_send(Cli* cli, FuriString* args, Input* input) {
    UNUSED(cli);
    InputKey key;
    InputType type;
    FuriString* key_str;
    key_str = furi_string_alloc();
    bool parsed = false;

    do {
        // Parse Key
        if(!args_read_string_and_trim(args, key_str)) {
            break;
        }
        if(!furi_string_cmp(key_str, "up")) {
            key = InputKeyUp;
        } else if(!furi_string_cmp(key_str, "down")) {
            key = InputKeyDown;
        } else if(!furi_string_cmp(key_str, "left")) {
            key = InputKeyLeft;
        } else if(!furi_string_cmp(key_str, "right")) {
            key = InputKeyRight;
        } else if(!furi_string_cmp(key_str, "ok")) {
            key = InputKeyOk;
        } else if(!furi_string_cmp(key_str, "back")) {
            key = InputKeyBack;
        } else {
            break;
        }
        // Parse Type
        if(!furi_string_cmp(args, "press")) {
            type = InputTypePress;
        } else if(!furi_string_cmp(args, "release")) {
            type = InputTypeRelease;
        } else if(!furi_string_cmp(args, "short")) {
            type = InputTypeShort;
        } else if(!furi_string_cmp(args, "long")) {
            type = InputTypeLong;
        } else {
            break;
        }
        parsed = true;
    } while(false);

    if(parsed) { //-V547
        input_fake_event(input, key, type);
    } else {
        input_cli_send_print_usage();
    }
    furi_string_free(key_str);
}

void input_cli(Cli* cli, FuriString* args, void* context) {
    furi_assert(cli);
    furi_assert(context);
    Input* input = context;
    FuriString* cmd;
    cmd = furi_string_alloc();

    do {
        if(!args_read_string_and_trim(args, cmd)) {
            input_cli_usage();
            break;
        }
        if(furi_string_cmp_str(cmd, "dump") == 0) {
            input_cli_dump(cli, args, input);
            break;
        }
        if(furi_string_cmp_str(cmd, "keyboard") == 0) {
            input_cli_keyboard(cli, args, input);
            break;
        }
        if(furi_string_cmp_str(cmd, "send") == 0) {
            input_cli_send(cli, args, input);
            break;
        }

        input_cli_usage();
    } while(false);

    furi_string_free(cmd);
}
