/**
 * camera.hpp
 * This file contains definition of the camera controls.
 * 
 * Basic example from Espressif: https://github.com/espressif/esp-who/blob/master/examples/code_recognition/main/app_peripherals.c
 * 
 * OV2640 Camera datasheet: https://www.dragonwake.com/download/camera/ov2640/OV2640%20Camera%20Module%20Software%20Application%20Notes.pdf
*/
#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <esp_camera.h>
#include <esp_code_scanner.h>
#include <string>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>

namespace CameraDriver {
/// @brief User settings for the camera
enum class Setting {
    QUALITY,  // Quality setting (0, 63)
    BRIGHTNESS,  // Brightness setting (-2, 2)
    CONTRAST,  // Contrast setting (-2, 2)
    SATURATION,  // Saturation setting (-2, 2)
    SHARPNESS,  // Sharpness setting (-2, 2)
};

class CameraBase {
public:
    virtual void init() = 0;
    virtual bool scan_code(std::string& res) = 0;
    virtual const camera_fb_t* get_frame() = 0;
    virtual void ret_frame() = 0;
    virtual void change_settings(Setting sett, int val) = 0;
};

/// @brief Class for camera handling. Supports getting the frame buffer and scanning QR and barcodes.
template<camera_config_t CFG>
class Camera : public CameraBase {
    camera_fb_t *fb = NULL;
    esp_image_scanner_t *esp_scn;
public:
    /// @brief Initialize the camera for usage
    void init() override {
        // camera initialization
        esp_err_t err = esp_camera_init(&CFG);
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
    };
    /// @brief Try finding code in camera buffer
    /// @param res - Found code (if any)
    /// @return - returns true if code was found
    bool scan_code(std::string& res) override {
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
                // Copy to our known variable
                auto tmp = esp_code_scanner_result(esp_scn);
                res = std::string{tmp.data};
                ESP_LOGI("Camera", "Scanned %s", res.c_str());
                // Throw away old scanner since it bugs
                esp_code_scanner_destroy(esp_scn);
                esp_scn = esp_code_scanner_create();
                ret = true;
            }
        }
    return ret;
    };
    /// @brief Provides access to the camera buffer. Frame should be returned after use: See ret_frame
    /// @return Camera buffer
    const camera_fb_t* get_frame() override {
        fb = esp_camera_fb_get();
        return fb;
    };
    /// @brief Returns buffer ownership back to the camera
    void ret_frame() override {
        esp_camera_fb_return(fb);
        fb = NULL;
    };
    /// @brief Provides API for changing some camera settings
    /// @param sett - Setting type
    /// @param value - Setting value
    void change_settings(Setting sett, int value) override {
        sensor_t* s = esp_camera_sensor_get();
        switch (sett) {
        case Setting::QUALITY:
            s->set_quality(s, value);
            break;
        case Setting::SHARPNESS:
            s->set_sharpness(s, value);
            break;
        case Setting::SATURATION:
            s->set_saturation(s, value);
            break;
        default:
            break;
        }
    };
    ~Camera() {
        esp_code_scanner_destroy(esp_scn);
    };
};

};

#endif