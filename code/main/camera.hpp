/**
 * https://github.com/espressif/esp-who/blob/master/examples/code_recognition/main/app_peripherals.c
 * 
 * https://www.dragonwake.com/download/camera/ov2640/OV2640%20Camera%20Module%20Software%20Application%20Notes.pdf
*/
#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <esp_camera.h>
#include <esp_code_scanner.h>

namespace CameraDriver {

constexpr int CAMERA_PIN_PWDN = 32;
constexpr int CAMERA_PIN_RESET = -1;
constexpr int CAMERA_PIN_XCLK = 0;
constexpr int CAMERA_PIN_SIOD = 26;
constexpr int CAMERA_PIN_SIOC = 27;

constexpr int CAMERA_PIN_D7 = 35;
constexpr int CAMERA_PIN_D6 = 34;
constexpr int CAMERA_PIN_D5 = 39;
constexpr int CAMERA_PIN_D4 = 36;
constexpr int CAMERA_PIN_D3 = 21;
constexpr int CAMERA_PIN_D2 = 19;
constexpr int CAMERA_PIN_D1 = 18;
constexpr int CAMERA_PIN_D0 = 5;
constexpr int CAMERA_PIN_VSYNC = 25;
constexpr int CAMERA_PIN_HREF = 23;
constexpr int CAMERA_PIN_PCLK = 22;

constexpr int XCLK_FREQ_HZ = 24000000;
constexpr pixformat_t CAMERA_PIXFORMAT = PIXFORMAT_GRAYSCALE;
constexpr framesize_t CAMERA_FRAME_SIZE = FRAMESIZE_QVGA;
constexpr int CAMERA_FB_COUNT = 1;

/// @brief User settings for the camera
enum class Setting {
    QUALITY,  // Quality setting (0, 63)
    BRIGHTNESS,  // Brightness setting (-2, 2)
    CONTRAST,  // Contrast setting (-2, 2)
    SATURATION,  // Saturation setting (-2, 2)
    SHARPNESS,  // Sharpness setting (-2, 2)
};

class Camera {
    camera_fb_t *fb = NULL;
public:
    void init();
    bool scan_code(esp_code_scanner_symbol_t& res);
    const camera_fb_t* get_frame();
    void ret_frame();
    void change_settings(Setting sett, int value);
};

};

#endif