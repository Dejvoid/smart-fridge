/**
 * https://github.com/espressif/esp-who/blob/master/examples/code_recognition/main/app_peripherals.c
*/
#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <esp_camera.h>

namespace CameraDriver {

#define CAMERA_PIN_PWDN 32
#define CAMERA_PIN_RESET -1
#define CAMERA_PIN_XCLK 0
#define CAMERA_PIN_SIOD 26
#define CAMERA_PIN_SIOC 27

#define CAMERA_PIN_D7 35
#define CAMERA_PIN_D6 34
#define CAMERA_PIN_D5 39
#define CAMERA_PIN_D4 36
#define CAMERA_PIN_D3 21
#define CAMERA_PIN_D2 19
#define CAMERA_PIN_D1 18
#define CAMERA_PIN_D0 5
#define CAMERA_PIN_VSYNC 25
#define CAMERA_PIN_HREF 23
#define CAMERA_PIN_PCLK 22

#define XCLK_FREQ_HZ 40000000
#define CAMERA_PIXFORMAT PIXFORMAT_GRAYSCALE
#define CAMERA_FRAME_SIZE FRAMESIZE_QVGA
#define CAMERA_FB_COUNT 2

enum class Setting {
    QUALITY,  // Quality setting (0, 63)
    BRIGHTNESS,  // Brightness setting (-2, 2)
    CONTRAST,  // Contrast setting (-2, 2)
    SATURATION,  // Saturation setting (-2, 2)
    SHARPNESS,  // Sharpness setting (-2, 2)
};

class Camera {
    camera_fb_t *fb = NULL;
    int64_t time1, time2;
public:
    void init();
    void loop();
    const camera_fb_t* get_frame();
    void change_settings(Setting sett, int value);
};

};

#endif