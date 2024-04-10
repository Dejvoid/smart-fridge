/**
 * camera.hpp
 * This file contains implementation of camera controls.
 */
#include "camera.hpp"

#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>

using namespace CameraDriver;

void Camera::init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = CAMERA_PIN_D0;
    config.pin_d1 = CAMERA_PIN_D1;
    config.pin_d2 = CAMERA_PIN_D2;
    config.pin_d3 = CAMERA_PIN_D3;
    config.pin_d4 = CAMERA_PIN_D4;
    config.pin_d5 = CAMERA_PIN_D5;
    config.pin_d6 = CAMERA_PIN_D6;
    config.pin_d7 = CAMERA_PIN_D7;
    config.pin_xclk = CAMERA_PIN_XCLK;
    config.pin_pclk = CAMERA_PIN_PCLK;
    config.pin_vsync = CAMERA_PIN_VSYNC;
    config.pin_href = CAMERA_PIN_HREF;
    config.pin_sccb_sda = CAMERA_PIN_SIOD;
    config.pin_sccb_scl = CAMERA_PIN_SIOC;
    config.pin_pwdn = CAMERA_PIN_PWDN;
    config.pin_reset = CAMERA_PIN_RESET;
    config.xclk_freq_hz = XCLK_FREQ_HZ;
    config.pixel_format = CAMERA_PIXFORMAT;
    config.frame_size = CAMERA_FRAME_SIZE;
    config.jpeg_quality = 12;
    config.fb_count = CAMERA_FB_COUNT;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    // camera initialization
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        ESP_LOGE("camera.cpp","Camera init failed with error 0x%x", err);
        return;
    }

    // Acquire the sensor from the esp_camera underlying driver and perform transformations to make the picture nice to work with
    sensor_t *s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);

    // Initialize underlying scanner for codes
    esp_scn = esp_code_scanner_create();
}

const camera_fb_t* Camera::get_frame() {
    fb = esp_camera_fb_get();
    return fb;
}

void Camera::ret_frame() {
    esp_camera_fb_return(fb);
    fb = NULL;
}

bool Camera::scan_code(esp_code_scanner_symbol_t* res) {
    bool ret = false;
    if (fb != NULL) {
        
        esp_code_scanner_config_t config = {
            .mode = ESP_CODE_SCANNER_MODE_FAST, 
            .fmt = ESP_CODE_SCANNER_IMAGE_GRAY,
            .width = fb->width,
            .height = fb->height
        };
        ESP_ERROR_CHECK(esp_code_scanner_set_config(esp_scn, config));
        int decoded_num = esp_code_scanner_scan_image(esp_scn, fb->buf);
        if(decoded_num) {
            *res = esp_code_scanner_result(esp_scn);
            ESP_LOGI("Camera", "Scanned %s", res->data);
            // Throw away old scanner since it bugs
            esp_code_scanner_destroy(esp_scn);
            esp_scn = esp_code_scanner_create();
            ret = true;
        }
    }
    return ret;
}

void Camera::change_settings(Setting sett, int val) {
    sensor_t* s = esp_camera_sensor_get();
    switch (sett) {
    case Setting::QUALITY:
        s->set_quality(s, val);
        break;
    case Setting::SHARPNESS:
        s->set_sharpness(s, val);
        break;
    case Setting::SATURATION:
        s->set_saturation(s, val);
        break;
    default:
        break;
    }
}

Camera::~Camera() {
    esp_code_scanner_destroy(esp_scn);
}