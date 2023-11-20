#include "transport.h"
#include <furi_hal_rtc.h>
#define TAG "Transport parser"

void timestamp_to_datetime(uint32_t timestamp, FuriHalRtcDateTime* datetime) { //Rewrite need
    datetime->year = timestamp / (60 * 60 * 24 * 366) + FURI_HAL_RTC_EPOCH_START_YEAR;
    uint16_t extra_days = (datetime->year - FURI_HAL_RTC_EPOCH_START_YEAR - 1) / 4;
    uint16_t days_since_epoch = timestamp / FURI_HAL_RTC_SECONDS_PER_DAY;
    uint16_t days_since_epoch_without_extra_days = days_since_epoch - extra_days;
    uint16_t days_in_this_year =
        days_since_epoch_without_extra_days % furi_hal_rtc_days_per_year[0] + 1;
    while(days_in_this_year > furi_hal_rtc_get_days_per_month(0, datetime->month + 1)) {
        days_in_this_year -= furi_hal_rtc_get_days_per_month(0, datetime->month + 1);
        datetime->month++;
    }
    datetime->month++;
    if(FURI_HAL_RTC_IS_LEAP_YEAR(datetime->year)) {
        datetime->day = days_in_this_year - 1;
    } else {
        datetime->day = days_in_this_year;
    }
    // uint16_t seconds_in_this_day =
    // timestamp - (days_since_epoch * FURI_HAL_RTC_SECONDS_PER_DAY);
    // datetime->hour = seconds_in_this_day / FURI_HAL_RTC_SECONDS_PER_HOUR;
    // uint16_t minutes_in_this_day =
    // seconds_in_this_day - (datetime->hour * FURI_HAL_RTC_SECONDS_PER_HOUR);
    // datetime->minute = minutes_in_this_day / FURI_HAL_RTC_SECONDS_PER_MINUTE;
    // datetime->second = minutes_in_this_day - (datetime->minute * FURI_HAL_RTC_SECONDS_PER_MINUTE);
    datetime->second = timestamp % 60;
    datetime->minute = timestamp / 60 % 60;
    datetime->hour = timestamp / (60 * 60) % 24;
}

void from_days_to_datetime(uint16_t days, FuriHalRtcDateTime* datetime, uint16_t start_year) {
    uint32_t timestamp = days * 24 * 60 * 60;
    FuriHalRtcDateTime start_datetime = {0};
    start_datetime.year = start_year - 1;
    start_datetime.month = 12;
    start_datetime.day = 31;
    timestamp += furi_hal_rtc_datetime_to_timestamp(&start_datetime);
    timestamp_to_datetime(timestamp, datetime);
}

void from_minutes_to_datetime(uint32_t minutes, FuriHalRtcDateTime* datetime, uint16_t start_year) {
    uint32_t timestamp = minutes * 60;
    FuriHalRtcDateTime start_datetime = {0};
    start_datetime.year = start_year - 1;
    start_datetime.month = 12;
    start_datetime.day = 31;
    timestamp += furi_hal_rtc_datetime_to_timestamp(&start_datetime);
    timestamp_to_datetime(timestamp, datetime);
}

bool parse_transport_block(const MfClassicBlock* block, FuriString* result) {
    uint16_t transport_departament = bit_lib_get_bits_16(block->data, 0, 10);

    FURI_LOG_D(TAG, "Transport departament: %x", transport_departament);

    uint16_t layout_type = bit_lib_get_bits_16(block->data, 52, 4);
    if(layout_type == 0xE) {
        layout_type = bit_lib_get_bits_16(block->data, 52, 9);
    } else if(layout_type == 0xF) {
        layout_type = bit_lib_get_bits_16(block->data, 52, 14);
    }

    FURI_LOG_D(TAG, "Layout type %x", layout_type);

    uint16_t card_view = 0;
    uint16_t card_type = 0;
    uint32_t card_number = 0;
    uint8_t card_layout = 0;
    uint8_t card_layout2 = 0;
    uint16_t card_use_before_date = 0;
    uint16_t card_blank_type = 0;
    uint32_t card_start_trip_minutes = 0;
    uint8_t card_minutes_pass = 0;
    uint32_t card_remaining_funds = 0;
    uint16_t card_validator = 0;
    uint8_t card_blocked = 0;
    uint32_t card_hash = 0;

    switch(layout_type) {
    case 0x02: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        uint8_t card_benefit_code = bit_lib_get_bits(block->data, 72, 8); //124
        uint32_t card_rfu1 = bit_lib_get_bits_32(block->data, 80, 32); //rfu1
        uint16_t card_crc16 = bit_lib_get_bits_16(block->data, 112, 16); //501.1
        card_blocked = bit_lib_get_bits(block->data, 128, 1); //303
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 177, 12); //403
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 189, 16); //402
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 157, 16); //311
        uint16_t card_valid_by_date = bit_lib_get_bits_16(block->data, 173, 16); //312
        uint8_t card_start_trip_seconds = bit_lib_get_bits(block->data, 189, 6); //406
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 180, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 182, 2); //421.2
        uint8_t card_transport_type3 = bit_lib_get_bits(block->data, 184, 2); //421.3
        uint8_t card_transport_type4 = bit_lib_get_bits(block->data, 186, 2); //421.4
        uint16_t card_use_with_date = bit_lib_get_bits_16(block->data, 189, 16); //205
        uint8_t card_route = bit_lib_get_bits(block->data, 205, 1); //424
        uint16_t card_validator1 = bit_lib_get_bits_16(block->data, 206, 15); //422.1
        card_validator = bit_lib_get_bits_16(block->data, 205, 16); //422
        uint16_t card_total_trips = bit_lib_get_bits_16(block->data, 221, 16); //331
        uint8_t card_write_enabled = bit_lib_get_bits(block->data, 237, 1); //write_enabled
        uint8_t card_rfu2 = bit_lib_get_bits(block->data, 238, 2); //rfu2
        uint16_t card_crc16_2 = bit_lib_get_bits_16(block->data, 240, 16); //501.2

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_benefit_code,
            card_rfu1,
            card_crc16,
            card_blocked,
            card_start_trip_time,
            card_start_trip_date,
            card_valid_from_date,
            card_valid_by_date,
            card_start_trip_seconds,
            card_transport_type1,
            card_transport_type2,
            card_transport_type3,
            card_transport_type4,
            card_use_with_date,
            card_route,
            card_validator1,
            card_validator,
            card_total_trips,
            card_write_enabled,
            card_rfu2,
            card_crc16_2);
        break;
    }
    case 0x06: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        uint8_t card_geozone_a = bit_lib_get_bits(block->data, 72, 4); //GeoZoneA
        uint8_t card_geozone_b = bit_lib_get_bits(block->data, 76, 4); //GeoZoneB
        card_blank_type = bit_lib_get_bits_16(block->data, 80, 10); //121.
        uint16_t card_type_of_extended = bit_lib_get_bits_16(block->data, 90, 10); //122
        uint32_t card_rfu1 = bit_lib_get_bits_16(block->data, 100, 12); //rfu1
        uint16_t card_crc16 = bit_lib_get_bits_16(block->data, 112, 16); //501.1
        card_blocked = bit_lib_get_bits(block->data, 128, 1); //303
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 129, 12); //403
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 141, 16); //402
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 157, 16); //311
        uint16_t card_valid_by_date = bit_lib_get_bits_16(block->data, 173, 16); //312
        uint16_t card_company = bit_lib_get_bits(block->data, 189, 4); //Company
        uint8_t card_validator1 = bit_lib_get_bits(block->data, 193, 4); //422.1
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 197, 10); //321
        uint8_t card_units = bit_lib_get_bits(block->data, 207, 6); //Units
        uint16_t card_validator2 = bit_lib_get_bits_16(block->data, 213, 10); //422.2
        uint16_t card_total_trips = bit_lib_get_bits_16(block->data, 223, 16); //331
        uint8_t card_extended = bit_lib_get_bits(block->data, 239, 1); //123
        uint16_t card_crc16_2 = bit_lib_get_bits_16(block->data, 240, 16); //501.2

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_geozone_a,
            card_geozone_b,
            card_blank_type,
            card_type_of_extended,
            card_rfu1,
            card_crc16,
            card_blocked,
            card_start_trip_time,
            card_start_trip_date,
            card_valid_from_date,
            card_valid_by_date,
            card_company,
            card_validator1,
            card_remaining_trips,
            card_units,
            card_validator2,
            card_total_trips,
            card_extended,
            card_crc16_2);
        card_validator = card_validator1 * 1024 + card_validator2;
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_valid_by_date - 1, &card_use_before_date_s, 1992);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_start_trip_date - 1) * 24 * 60 + card_start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrips left: %d of %d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_remaining_trips,
            card_total_trips,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x08: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        uint64_t card_rfu1 = bit_lib_get_bits_64(block->data, 72, 56); //rfu1
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint8_t card_rfu2 = bit_lib_get_bits(block->data, 153, 7); //rfu2
        uint8_t card_remaining_trips1 = bit_lib_get_bits(block->data, 160, 8); //321.1
        uint8_t card_remaining_trips = bit_lib_get_bits(block->data, 168, 8); //321
        uint8_t card_validator1 = bit_lib_get_bits(block->data, 193, 2); //422.1
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 177, 15); //422
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint32_t card_rfu3 = bit_lib_get_bits_32(block->data, 224, 32); //rfu3

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %llx %x %x %x %x %x %x %x %x %lx %x %lx",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_rfu1,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu2,
            card_remaining_trips1,
            card_remaining_trips,
            card_validator1,
            card_validator,
            card_hash,
            card_valid_from_date,
            card_rfu3);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 1992);

        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0A: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 64, 12); //311
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 76, 19); //314
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 95, 1); //301
        card_start_trip_minutes = bit_lib_get_bits_32(block->data, 96, 19); //405
        card_minutes_pass = bit_lib_get_bits(block->data, 119, 7); //412
        uint8_t card_transport_type_flag = bit_lib_get_bits(block->data, 126, 2); //421.0
        uint8_t card_remaining_trips = bit_lib_get_bits(block->data, 128, 8); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 136, 16); //422
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 152, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 154, 2); //421.2
        uint8_t card_transport_type3 = bit_lib_get_bits(block->data, 156, 2); //421.3
        uint8_t card_transport_type4 = bit_lib_get_bits(block->data, 158, 2); //421.4
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %lx %x %lx %x %x %x %x %x %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_valid_from_date,
            card_valid_for_minutes,
            card_requires_activation,
            card_start_trip_minutes,
            card_minutes_pass,
            card_transport_type_flag,
            card_remaining_trips,
            card_validator,
            card_transport_type1,
            card_transport_type2,
            card_transport_type3,
            card_transport_type4,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 2016);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            card_start_trip_minutes - (2 * 24 * 60), &card_start_trip_minutes_s, 2016);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0C: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_use_before_date = bit_lib_get_bits_16(block->data, 56, 16); //202
        uint64_t card_rfu1 = bit_lib_get_bits_64(block->data, 72, 56); //rfu1
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint16_t card_rfu2 = bit_lib_get_bits_16(block->data, 153, 13); //rfu2
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 166, 10); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 176, 16); //422
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 224, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 240, 11); //403
        uint8_t card_transport_type = bit_lib_get_bits(block->data, 251, 2); //421
        uint8_t card_rfu3 = bit_lib_get_bits(block->data, 253, 2); //rfu3
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 255, 1); //432

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %llx %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_use_before_date,
            card_rfu1,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu2,
            card_remaining_trips,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type,
            card_rfu3,
            card_transfer_in_metro);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 1992);
        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_start_trip_date - 1) * 24 * 60 + card_start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x0D: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        uint8_t card_rfu1 = bit_lib_get_bits(block->data, 56, 8); //rfu1
        card_use_before_date = bit_lib_get_bits_16(block->data, 64, 16); //202
        uint16_t card_valid_for_time = bit_lib_get_bits_16(block->data, 80, 11); //316
        uint8_t card_rfu2 = bit_lib_get_bits(block->data, 91, 5); //rfu2
        uint16_t card_use_before_date2 = bit_lib_get_bits_16(block->data, 96, 16); //202.2
        uint16_t card_valid_for_time2 = bit_lib_get_bits_16(block->data, 123, 11); //316.2
        uint8_t card_rfu3 = bit_lib_get_bits(block->data, 123, 5); //rfu3
        uint16_t card_valid_from_date = bit_lib_get_bits_16(block->data, 128, 16); //311
        uint8_t card_valid_for_days = bit_lib_get_bits(block->data, 144, 8); //313
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 152, 1); //301
        uint8_t card_rfu4 = bit_lib_get_bits(block->data, 153, 2); //rfu4
        uint8_t card_passage_5_minutes = bit_lib_get_bits(block->data, 155, 5); //413
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 160, 2); //421.1
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 162, 1); //431
        uint8_t card_passages_ground_transport = bit_lib_get_bits(block->data, 163, 3); //433
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 166, 10); //321
        uint16_t card_validator = bit_lib_get_bits_16(block->data, 176, 16); //422
        card_hash = bit_lib_get_bits_32(block->data, 192, 32); //502
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 224, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 240, 11); //403
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 251, 2); //421.2
        uint8_t card_rfu5 = bit_lib_get_bits(block->data, 253, 2); //rfu5
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 255, 1); //432

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_rfu1,
            card_use_before_date,
            card_valid_for_time,
            card_rfu2,
            card_use_before_date2,
            card_valid_for_time2,
            card_rfu3,
            card_valid_from_date,
            card_valid_for_days,
            card_requires_activation,
            card_rfu4,
            card_passage_5_minutes,
            card_transport_type1,
            card_passage_in_metro,
            card_passages_ground_transport,
            card_remaining_trips,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type2,
            card_rfu5,
            card_transfer_in_metro);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 1992);
        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_start_trip_date - 1) * 24 * 60 + card_start_trip_time,
            &card_start_trip_minutes_s,
            1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nTrips left: %d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_remaining_trips,
            card_validator);
        break;
    }
    case 0x1C1: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        card_use_before_date = bit_lib_get_bits_16(block->data, 61, 16); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 77, 10); //121.
        card_validator = bit_lib_get_bits_16(block->data, 128, 16); //422
        uint16_t card_start_trip_date = bit_lib_get_bits_16(block->data, 144, 16); //402
        uint16_t card_start_trip_time = bit_lib_get_bits_16(block->data, 160, 11); //403
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 171, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 173, 2); //421.2
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 177, 1); //432
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 178, 1); //431
        uint8_t card_passages_ground_transport = bit_lib_get_bits(block->data, 179, 3); //433
        card_minutes_pass = bit_lib_get_bits(block->data, 185, 8); //412.
        card_remaining_funds = bit_lib_get_bits_32(block->data, 196, 19) / 100; //322
        uint8_t card_fare_trip = bit_lib_get_bits(block->data, 215, 2); //441
        card_blocked = bit_lib_get_bits(block->data, 202, 1); //303
        uint8_t card_zoo = bit_lib_get_bits(block->data, 218, 1); //zoo
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %lx %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_use_before_date,
            card_blank_type,
            card_validator,
            card_start_trip_date,
            card_start_trip_time,
            card_transport_type1,
            card_transport_type2,
            card_transfer_in_metro,
            card_passage_in_metro,
            card_passages_ground_transport,
            card_minutes_pass,
            card_remaining_funds,
            card_fare_trip,
            card_blocked,
            card_zoo,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 1992);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            card_start_trip_minutes - (2 * 24 * 60), &card_start_trip_minutes_s, 1992);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x1C2: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        uint16_t card_type_of_extended = bit_lib_get_bits_16(block->data, 61, 10); //122
        card_use_before_date = bit_lib_get_bits_16(block->data, 71, 16); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 87, 10); //121.
        uint16_t card_valid_to_date = bit_lib_get_bits_16(block->data, 97, 16); //311
        uint16_t card_activate_during = bit_lib_get_bits_16(block->data, 113, 9); //302
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 131, 20); //314
        card_minutes_pass = bit_lib_get_bits(block->data, 154, 8); //412.
        uint8_t card_transport_type = bit_lib_get_bits(block->data, 163, 2); //421
        uint8_t card_passage_in_metro = bit_lib_get_bits(block->data, 165, 1); //431
        uint8_t card_transfer_in_metro = bit_lib_get_bits(block->data, 166, 1); //432
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 167, 10); //321
        card_validator = bit_lib_get_bits_16(block->data, 177, 16); //422
        uint32_t card_start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 196, 20); //404
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 216, 1); //301
        card_blocked = bit_lib_get_bits(block->data, 217, 1); //303
        uint8_t card_extended = bit_lib_get_bits(block->data, 218, 1); //123
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x %lx %x %x %x %x %x %x %lx %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_type_of_extended,
            card_use_before_date,
            card_blank_type,
            card_valid_to_date,
            card_activate_during,
            card_valid_for_minutes,
            card_minutes_pass,
            card_transport_type,
            card_passage_in_metro,
            card_transfer_in_metro,
            card_remaining_trips,
            card_validator,
            card_start_trip_neg_minutes,
            card_requires_activation,
            card_blocked,
            card_extended,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 2016);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_valid_to_date - 1) * 24 * 60 + card_valid_for_minutes -
                card_start_trip_neg_minutes,
            &card_start_trip_minutes_s,
            2016); //-time
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x1C3: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        card_use_before_date = bit_lib_get_bits_16(block->data, 61, 16); //202
        card_blank_type = bit_lib_get_bits_16(block->data, 77, 10); //121
        card_remaining_funds = bit_lib_get_bits_32(block->data, 188, 22) / 100; //322
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502
        card_validator = bit_lib_get_bits_16(block->data, 128, 16); //422
        card_start_trip_minutes = bit_lib_get_bits_32(block->data, 144, 23); //405
        uint8_t card_fare_trip = bit_lib_get_bits(block->data, 210, 2); //441
        card_minutes_pass = bit_lib_get_bits(block->data, 171, 7); //412
        uint8_t card_transport_type_flag = bit_lib_get_bits(block->data, 178, 2); //421.0
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 180, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 182, 2); //421.2
        uint8_t card_transport_type3 = bit_lib_get_bits(block->data, 184, 2); //421.3
        uint8_t card_transport_type4 = bit_lib_get_bits(block->data, 186, 2); //421.4
        card_blocked = bit_lib_get_bits(block->data, 212, 1); //303
        FURI_LOG_D(
            TAG,
            "Card view: %x, type: %x, number: %lx, layout: %x, layout2: %x, use before date: %x, blank type: %x, remaining funds: %lx, hash: %lx, validator: %x, start trip minutes: %lx, fare trip: %x, minutes pass: %x, transport type flag: %x, transport type1: %x, transport type2: %x, transport type3: %x, transport type4: %x, blocked: %x",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_use_before_date,
            card_blank_type,
            card_remaining_funds,
            card_hash,
            card_validator,
            card_start_trip_minutes,
            card_fare_trip,
            card_minutes_pass,
            card_transport_type_flag,
            card_transport_type1,
            card_transport_type2,
            card_transport_type3,
            card_transport_type4,
            card_blocked);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 1992);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(card_start_trip_minutes, &card_start_trip_minutes_s, 2016);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nBalance: %ld rub\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_remaining_funds,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x1C4: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        uint16_t card_type_of_extended = bit_lib_get_bits_16(block->data, 61, 10); //122
        card_use_before_date = bit_lib_get_bits_16(block->data, 71, 13); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 84, 10); //121.
        uint16_t card_valid_to_date = bit_lib_get_bits_16(block->data, 94, 13); //311
        uint16_t card_activate_during = bit_lib_get_bits_16(block->data, 107, 9); //302
        uint16_t card_extension_counter = bit_lib_get_bits_16(block->data, 116, 10); //304
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 128, 20); //314
        card_minutes_pass = bit_lib_get_bits(block->data, 158, 7); //412.
        uint8_t card_transport_type_flag = bit_lib_get_bits(block->data, 178, 2); //421.0
        uint8_t card_transport_type1 = bit_lib_get_bits(block->data, 180, 2); //421.1
        uint8_t card_transport_type2 = bit_lib_get_bits(block->data, 182, 2); //421.2
        uint8_t card_transport_type3 = bit_lib_get_bits(block->data, 184, 2); //421.3
        uint8_t card_transport_type4 = bit_lib_get_bits(block->data, 186, 2); //421.4
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 169, 10); //321
        card_validator = bit_lib_get_bits_16(block->data, 179, 16); //422
        uint32_t card_start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 195, 20); //404
        uint8_t card_requires_activation = bit_lib_get_bits(block->data, 215, 1); //301
        card_blocked = bit_lib_get_bits(block->data, 216, 1); //303
        uint8_t card_extended = bit_lib_get_bits(block->data, 217, 1); //123
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x %x %lx %x %x %x %x %x %x %x %x %lx %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_type_of_extended,
            card_use_before_date,
            card_blank_type,
            card_valid_to_date,
            card_activate_during,
            card_extension_counter,
            card_valid_for_minutes,
            card_minutes_pass,
            card_transport_type_flag,
            card_transport_type1,
            card_transport_type2,
            card_transport_type3,
            card_transport_type4,
            card_remaining_trips,
            card_validator,
            card_start_trip_neg_minutes,
            card_requires_activation,
            card_blocked,
            card_extended,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 2016);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            (card_use_before_date - 1) * 24 * 60 + card_valid_for_minutes -
                card_start_trip_neg_minutes,
            &card_start_trip_minutes_s,
            2016); //-time
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x1C5: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        card_use_before_date = bit_lib_get_bits_16(block->data, 61, 13); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 74, 10); //121.
        uint32_t card_valid_to_time = bit_lib_get_bits_32(block->data, 84, 23); //317
        uint16_t card_extension_counter = bit_lib_get_bits_16(block->data, 107, 10); //304
        card_start_trip_minutes = bit_lib_get_bits_32(block->data, 128, 23); //405
        uint8_t card_metro_ride_with = bit_lib_get_bits(block->data, 151, 7); //414
        card_minutes_pass = bit_lib_get_bits(block->data, 158, 7); //412.
        card_remaining_funds = bit_lib_get_bits_32(block->data, 167, 19) / 100; //322
        card_validator = bit_lib_get_bits_16(block->data, 186, 16); //422
        card_blocked = bit_lib_get_bits(block->data, 202, 1); //303
        uint16_t card_route = bit_lib_get_bits_16(block->data, 204, 12); //424
        uint8_t card_passages_ground_transport = bit_lib_get_bits(block->data, 216, 7); //433
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %lx %x %lx %x %x %lx %x %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_use_before_date,
            card_blank_type,
            card_valid_to_time,
            card_extension_counter,
            card_start_trip_minutes,
            card_metro_ride_with,
            card_minutes_pass,
            card_remaining_funds,
            card_validator,
            card_blocked,
            card_route,
            card_passages_ground_transport,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};

        from_days_to_datetime(card_use_before_date, &card_use_before_date_s, 2019);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            card_start_trip_minutes - (24 * 60), &card_start_trip_minutes_s, 2019);
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nBalance: %ld rub\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_remaining_funds,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x1C6: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        card_layout2 = bit_lib_get_bits(block->data, 56, 5); //112
        uint16_t card_type_of_extended = bit_lib_get_bits_16(block->data, 61, 10); //122
        card_use_before_date = bit_lib_get_bits_16(block->data, 71, 13); //202.
        card_blank_type = bit_lib_get_bits_16(block->data, 84, 10); //121.
        uint32_t card_valid_from_date = bit_lib_get_bits_32(block->data, 94, 23); //311
        uint16_t card_extension_counter = bit_lib_get_bits_16(block->data, 117, 10); //304
        uint32_t card_valid_for_minutes = bit_lib_get_bits_32(block->data, 128, 20); //314
        uint32_t card_start_trip_neg_minutes = bit_lib_get_bits_32(block->data, 148, 20); //404
        uint8_t card_metro_ride_with = bit_lib_get_bits(block->data, 168, 7); //414
        card_minutes_pass = bit_lib_get_bits(block->data, 175, 7); //412.
        uint16_t card_remaining_trips = bit_lib_get_bits_16(block->data, 182, 7); //321
        card_validator = bit_lib_get_bits_16(block->data, 189, 16); //422
        card_blocked = bit_lib_get_bits(block->data, 205, 1); //303
        uint8_t card_extended = bit_lib_get_bits(block->data, 206, 1); //123
        uint16_t card_route = bit_lib_get_bits_16(block->data, 212, 12); //424
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %lx %x %lx %lx %x %x %x %x %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_layout2,
            card_type_of_extended,
            card_use_before_date,
            card_blank_type,
            card_valid_from_date,
            card_extension_counter,
            card_valid_for_minutes,
            card_start_trip_neg_minutes,
            card_metro_ride_with,
            card_minutes_pass,
            card_remaining_trips,
            card_validator,
            card_blocked,
            card_extended,
            card_route,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_use_before_date - 1, &card_use_before_date_s, 2019);

        FuriHalRtcDateTime card_start_trip_minutes_s = {0};
        from_minutes_to_datetime(
            card_valid_from_date + card_valid_for_minutes - card_start_trip_neg_minutes - 24 * 60,
            &card_start_trip_minutes_s,
            2019); //-time
        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nTrip from: %02d.%02d.%04d %02d:%02d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_start_trip_minutes_s.day,
            card_start_trip_minutes_s.month,
            card_start_trip_minutes_s.year,
            card_start_trip_minutes_s.hour,
            card_start_trip_minutes_s.minute,
            card_validator);
        break;
    }
    case 0x3CCB: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        uint16_t card_tech_code = bit_lib_get_bits_32(block->data, 56, 10); //tech_code
        uint16_t card_valid_to_minutes = bit_lib_get_bits_16(block->data, 66, 16); //311
        uint16_t card_valid_by_date = bit_lib_get_bits_16(block->data, 82, 16); //312
        uint8_t card_interval = bit_lib_get_bits(block->data, 98, 4); //interval
        uint16_t card_app_code1 = bit_lib_get_bits_16(block->data, 102, 16); //app_code1
        uint16_t card_hash1 = bit_lib_get_bits_16(block->data, 112, 16); //502.1
        uint16_t card_type1 = bit_lib_get_bits_16(block->data, 128, 10); //type1
        uint16_t card_app_code2 = bit_lib_get_bits_16(block->data, 138, 10); //app_code2
        uint16_t card_type2 = bit_lib_get_bits_16(block->data, 148, 10); //type2
        uint16_t card_app_code3 = bit_lib_get_bits_16(block->data, 158, 10); //app_code3
        uint16_t card_type3 = bit_lib_get_bits_16(block->data, 148, 10); //type3
        uint16_t card_app_code4 = bit_lib_get_bits_16(block->data, 168, 10); //app_code4
        uint16_t card_type4 = bit_lib_get_bits_16(block->data, 178, 10); //type4
        card_hash = bit_lib_get_bits_32(block->data, 224, 32); //502.2

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %lx",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_tech_code,
            card_use_before_date,
            card_blank_type,
            card_valid_to_minutes,
            card_valid_by_date,
            card_interval,
            card_app_code1,
            card_hash1,
            card_type1,
            card_app_code2,
            card_type2,
            card_app_code3,
            card_type3,
            card_app_code4,
            card_type4,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_valid_by_date - 1, &card_use_before_date_s, 1992);

        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_validator);
        break;
    }
    case 0x3C0B: {
        card_view = bit_lib_get_bits_16(block->data, 0, 10); //101
        card_type = bit_lib_get_bits_16(block->data, 10, 10); //102
        card_number = bit_lib_get_bits_32(block->data, 20, 32); //201
        card_layout = bit_lib_get_bits(block->data, 52, 4); //111
        uint16_t card_tech_code = bit_lib_get_bits_32(block->data, 56, 10); //tech_code
        uint16_t card_valid_to_minutes = bit_lib_get_bits_16(block->data, 66, 16); //311
        uint16_t card_valid_by_date = bit_lib_get_bits_16(block->data, 82, 16); //312
        uint16_t card_hash = bit_lib_get_bits_16(block->data, 112, 16); //502.1

        FURI_LOG_D(
            TAG,
            "%x %x %lx %x %x %x %x %x %x %x",
            card_view,
            card_type,
            card_number,
            card_layout,
            card_tech_code,
            card_use_before_date,
            card_blank_type,
            card_valid_to_minutes,
            card_valid_by_date,
            card_hash);
        FuriHalRtcDateTime card_use_before_date_s = {0};
        from_days_to_datetime(card_valid_by_date - 1, &card_use_before_date_s, 1992);

        furi_string_printf(
            result,
            "Number: %010lu\nValid for: %02d.%02d.%04d\nValidator: %05d",
            card_number,
            card_use_before_date_s.day,
            card_use_before_date_s.month,
            card_use_before_date_s.year,
            card_validator);
        break;
    }
    default:
        return false;
    }
    return true;
}