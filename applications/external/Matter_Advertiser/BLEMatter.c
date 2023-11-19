//
// Created by Ryan on 11/19/2023.
//

#include <stdint.h>
#include <string.h>
#include <furi_hal_bt.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

// Matter BLE advertisement data structure
typedef struct {
    uint8_t length_flags;
    uint8_t type_flags;
    uint8_t flags;
    uint8_t length_service_data;
    uint8_t type_service_data;
    uint8_t matter_uuid[2];
    uint8_t opcode;
    uint8_t discriminator[2];
    uint8_t vendor_id[2];
    uint8_t product_id[2];
    uint8_t additional_data_flags;
} MatterAdvData;

typedef struct {
    bool advertising;
    View* main_view;
    FuriThread* thread;
} State;
// Function to create the Matter BLE advertisement payload
MatterAdvData create_matter_ble_adv_payload() {
    MatterAdvData adv_data;

    // Set up flags
    adv_data.length_flags = 0x02; // Length of Flags
    adv_data.type_flags = 0x01;   // Type of Flags
    adv_data.flags = 0x06;        // Flags (General Discoverability, BR/EDR Not Supported)

    // Set up service data
    adv_data.length_service_data = 0x0B; // Length of Service Data
    adv_data.type_service_data = 0x16;   // Type of Service Data (Service Data - 16-bit UUID)
    adv_data.matter_uuid[0] = 0xF6;      // Matter UUID
    adv_data.matter_uuid[1] = 0xFF;

    adv_data.opcode = 0x00; // Matter BLE OpCode (Commissionable)

    // 12-bit discriminator (0x3AB), packed into 2 bytes with Advertisement version
    adv_data.discriminator[0] = 0xAB;
    adv_data.discriminator[1] = 0x03;

    // Vendor ID
    adv_data.vendor_id[0] = 0xF1;
    adv_data.vendor_id[1] = 0xFF;

    // Product ID
    adv_data.product_id[0] = 0x00;
    adv_data.product_id[1] = 0x80;

    // Additional Data Flags
    adv_data.additional_data_flags = 0x01; // GATT-based Additional Data present

    return adv_data;
}

// Function to start custom Matter advertising
bool start_matter_advertising() {
    MatterAdvData adv_data = create_matter_ble_adv_payload();
    uint8_t adv_payload[sizeof(MatterAdvData)];
    memcpy(adv_payload, &adv_data, sizeof(MatterAdvData));

    // Custom advertising parameters
    uint8_t mac_type = 0x01; // Static random
    uint8_t power_amp_level = 0x1F; // Max power level
    uint8_t mac_addr[GAP_MAC_ADDR_SIZE] = FURI_HAL_BT_DEFAULT_MAC_ADDR;

    // Stop any existing advertising
    furi_hal_bt_custom_adv_stop();

    // Set the custom advertisement data
    if(!furi_hal_bt_custom_adv_set(adv_payload, sizeof(adv_payload))) {
        return false;
    }

    // Start custom advertisement
    if(!furi_hal_bt_custom_adv_start(6400, 6400, mac_type, mac_addr, power_amp_level)) {
        return false;
    }

    return true;
}

// Function to stop custom Matter advertising
void stop_matter_advertising() {
    furi_hal_bt_custom_adv_stop();
}

// Main application function
int main() {
    // Initialize BLE
    // furi_hal_bt_init();

    // Setup GUI
    Gui* gui = furi_record_open("gui");
    ViewPort* view_port = view_port_alloc();
    //view_port_draw_callback_set(view_port, draw_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Setup input handling
    InputQueue* input_queue = furi_record_open("input");
    InputEvent event;
    bool advertising = false;

    while(furi_input_queue_read(input_queue, &event, FuriWaitForever)) {
        if(event.type == InputTypeShort && event.key == InputKeyOk) {
            if(advertising) {
                stop_matter_advertising();
            } else {
                start_matter_advertising();
            }
            advertising = !advertising;
        } else if(event.type == InputTypeShort && event.key == InputKeyBack) {
            break; // Exit the application
        }
    }

    // Cleanup
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");
    furi_record_close("input");

    return 0;
}

// Draw callback function for GUI
/*void draw_callback(Canvas* canvas, void* context) {
    // Drawing code here
    // Example: canvas_draw_str(canvas, 10, 10, "Matter Advertiser");
}*/

