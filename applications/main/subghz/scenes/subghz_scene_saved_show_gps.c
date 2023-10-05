#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"

void subghz_scene_saved_show_gps_draw_satellites(void* context) {
    SubGhz* subghz = context;

    FuriString* text_str = furi_string_alloc();
    FuriString* lat_str = furi_string_alloc();
    FuriString* lon_str = furi_string_alloc();

    subghz_txrx_get_latitude_and_longitude(subghz->txrx, lat_str, lon_str);

    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    if((datetime.second - subghz->gps->fix_second) > 15) {
        subghz->gps->latitude = NAN;
        subghz->gps->longitude = NAN;
        subghz->gps->satellites = 0;
        subghz->gps->fix_hour = 0;
        subghz->gps->fix_minute = 0;
        subghz->gps->fix_second = 0;
    }

    double latitude = atof(furi_string_get_cstr(lat_str));
    double longitude = atof(furi_string_get_cstr(lon_str));

    double distance = subghz_gps_calc_distance(
        latitude, longitude, (double)subghz->gps->latitude, (double)subghz->gps->longitude);

    float angle = subghz_gps_calc_angle(
        latitude, longitude, (double)subghz->gps->latitude, (double)subghz->gps->longitude);

    char* angle_str = "N";
    if(angle > 22.5 && angle <= 67.5) {
        angle_str = "NE";
    } else if(angle > 67.5 && angle <= 112.5) {
        angle_str = "E";
    } else if(angle > 112.5 && angle <= 157.5) {
        angle_str = "SE";
    } else if(angle > 157.5 && angle <= 202.5) {
        angle_str = "S";
    } else if(angle > 202.5 && angle <= 247.5) {
        angle_str = "SW";
    } else if(angle > 247.5 && angle <= 292.5) {
        angle_str = "W";
    } else if(angle > 292.5 && angle <= 337.5) {
        angle_str = "NW";
    }

    furi_string_printf(
        text_str,
        "Captured at: %f,\r\n%f\r\n\r\nRealtime:  Sats: %d\r\nDistance: %.2f%s Dir: %s\r\nGPS time: %d:%d:%d UTC",
        latitude,
        longitude,
        subghz->gps->satellites,
        subghz->gps->satellites > 0 ? distance > 1 ? distance : distance * 1000 : 0,
        distance > 1 ? "km" : "m",
        angle_str,
        subghz->gps->fix_hour,
        subghz->gps->fix_minute,
        subghz->gps->fix_second);

    widget_add_text_scroll_element(subghz->widget, 0, 0, 128, 64, furi_string_get_cstr(text_str));

    furi_string_free(text_str);
    furi_string_free(lat_str);
    furi_string_free(lon_str);
}

void subghz_scene_saved_show_gps_refresh_screen(void* context) {
    SubGhz* subghz = context;
    widget_reset(subghz->widget);
    subghz_scene_saved_show_gps_draw_satellites(subghz);
}

void subghz_scene_saved_show_gps_on_enter(void* context) {
    SubGhz* subghz = context;

    if(subghz->last_settings->gps_enabled) {
        furi_thread_start(subghz->gps->thread);
    }

    subghz_scene_saved_show_gps_draw_satellites(subghz);

    subghz->gps->timer = furi_timer_alloc(
        subghz_scene_saved_show_gps_refresh_screen, FuriTimerTypePeriodic, subghz);
    furi_timer_start(subghz->gps->timer, 1000);

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdWidget);
}

bool subghz_scene_saved_show_gps_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void subghz_scene_saved_show_gps_on_exit(void* context) {
    SubGhz* subghz = context;

    if(subghz->last_settings->gps_enabled) {
        furi_thread_flags_set(furi_thread_get_id(subghz->gps->thread), WorkerEvtStop);
        furi_thread_join(subghz->gps->thread);
    }

    furi_timer_stop(subghz->gps->timer);
    furi_timer_free(subghz->gps->timer);

    widget_reset(subghz->widget);
}