#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_subghz_i.h>
#include <flipper_format/flipper_format_i.h>

void subghz_extended_freq() {
    bool is_extended_i = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    if(flipper_format_file_open_existing(file, "/ext/subghz/assets/extend_range.txt")) {
        flipper_format_read_bool(file, "use_ext_range_at_own_risk", &is_extended_i, 1);
    }

    furi_hal_subghz_set_extended_frequency(is_extended_i);

    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}
