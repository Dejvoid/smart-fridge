/**
 * @mainpage Smart Fridge Documentation
 *
 * This is documentation for Smart Fridge Project
 */


/**
 * main.cpp 
 * 
 * This file contains main function of the whole firmware of the board.
 * Firmware on startup initializes all the components. Then continues to main loop.
 */
#include "lcd.hpp"
#include "wifi.hpp"
#include "camera.hpp"
#include "commander.hpp"
#include "inet_comm.hpp"

#include <driver/gpio.h>
#include <esp_http_client.h>
#include <nvs_flash.h>
#include <esp_timer.h>
#include "mqtt_comm.hpp"

/**
 * Camera constants definitions
 * See: https://github.com/espressif/esp-who/blob/master/examples/code_recognition/main/app_peripherals.c
 */
constexpr camera_config_t cam_cfg = {
    .pin_pwdn       = 32,
    .pin_reset      = -1,
    .pin_xclk       = 0,
    .pin_sccb_sda   = 26,
    .pin_sccb_scl   = 27,
    .pin_d7         = 35,
    .pin_d6         = 34,
    .pin_d5         = 39,
    .pin_d4         = 36,
    .pin_d3         = 21,
    .pin_d2         = 19,
    .pin_d1         = 18,
    .pin_d0         = 5,
    .pin_vsync      = 25,
    .pin_href       = 23,
    .pin_pclk       = 22,
    .xclk_freq_hz   = 24000000,
    .ledc_timer     = LEDC_TIMER_0,
    .ledc_channel   = LEDC_CHANNEL_0,
    .pixel_format   = PIXFORMAT_GRAYSCALE,
    .frame_size     = FRAMESIZE_QVGA,
    .jpeg_quality   = 12,
    .fb_count       = 1,
    .fb_location    = CAMERA_FB_IN_PSRAM,
    .grab_mode      = CAMERA_GRAB_WHEN_EMPTY
};

/**
 * Lcd constants definitions
*/
constexpr spi_host_device_t LCD_SPI = HSPI_HOST;
constexpr int LCD_MOSI              = GPIO_NUM_13;
constexpr int LCD_MISO              = -1;
constexpr int LCD_SCK               = GPIO_NUM_14;
constexpr int LCD_CS                = GPIO_NUM_15;
constexpr gpio_num_t LCD_DC         = GPIO_NUM_2;
constexpr gpio_num_t LCD_RST        = GPIO_NUM_12;
constexpr uint16_t LCD_W            = 480;
constexpr uint16_t LCD_H            = 320;

/**
 * Connection constants
*/
constexpr std::string_view WIFI_SSID = ""; // Change to your own WiFi SSID
constexpr std::string_view WIFI_PASSWORD = ""; // Change to your own WiFi password
constexpr std::string_view mqtt_uri = "mqtts://<server-address>:8883"; // Change to your MQTT URI

static void temp_update(void* arg) {
    ((ConsoleCommander::Commander*)arg)->therm_update(22.0, 40.0); // Dummy values for now since we can't connect thermometer
}

extern "C" void app_main(void) {   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WifiDriver::Wifi wifi;
    wifi.init(WIFI_SSID.data(), WIFI_PASSWORD.data());

    //InetComm::Connection conn{srv_ip, port};
    //conn.open();

    constexpr LcdDriver::LcdPins lcd_pins{LCD_MOSI, LCD_MISO, LCD_SCK, LCD_CS, LCD_RST, LCD_DC};
    LcdDriver::Lcd<LCD_SPI, lcd_pins, LCD_W, LCD_H> lcd;
    lcd.init();

    CameraDriver::Camera<cam_cfg> cam;
    cam.init();
    // These settings offer some decent performance. The numbers were derived based on trial and error
    cam.change_settings(CameraDriver::Setting::QUALITY, 63);
    cam.change_settings(CameraDriver::Setting::SHARPNESS, 2);
    cam.change_settings(CameraDriver::Setting::SATURATION, -2);

    // Temperature driver initialization. Currently not supported due to lack of pins for the communication
    //I2cTempDriver::Temperature therm;
    //therm.init();

    MqttComm mqtt{mqtt_uri.data()};
    ConsoleCommander::Commander cmd{&lcd, &mqtt, &cam};
    mqtt.connect(); // we have to connect after the queue for receiving messages is initialized in cmd
    //lcd.draw_line(0, LCD_H - LcdDriver::font_size - 5, LCD_W, LCD_H - LcdDriver::font_size - 5, 0xff, 0xff, 0xff);
    lcd.draw_line(0,240,320,240, 0xff,0xff,0xff);
    lcd.draw_line(320, 0, 320, 240, 0xff, 0xff, 0xff);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &temp_update,
        .arg = &cmd
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10'000'000));

    while (true) {
        cmd.loop();
        //therm.loop();
    }
}
