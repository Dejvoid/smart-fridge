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

/**
 * Lcd constants definitions
*/
constexpr spi_host_device_t LCD_SPI = HSPI_HOST;
constexpr int LCD_MOSI              = GPIO_NUM_13;
constexpr int LCD_MISO              = -1;
constexpr int LCD_SCK               = GPIO_NUM_14;
constexpr int LCD_CS                = -1;
constexpr gpio_num_t LCD_DC         = GPIO_NUM_2;
constexpr gpio_num_t LCD_RST        = GPIO_NUM_12;
constexpr uint16_t LCD_W            = 480;
constexpr uint16_t LCD_H            = 320;

/**
 * Connection constants
*/
constexpr const char* srv_ip = "10.42.0.1";
constexpr uint16_t port = 80;

extern "C" void app_main(void) {   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WifiDriver::Wifi wifi;
    wifi.init();

    InetComm::Connection conn{srv_ip, port};
    conn.open();

    constexpr LcdDriver::LcdPins lcd_pins{LCD_MOSI, LCD_MISO, LCD_SCK, LCD_CS, LCD_RST, LCD_DC};
    LcdDriver::Lcd<LCD_SPI, lcd_pins, LCD_W, LCD_H> lcd;
    lcd.init();

    CameraDriver::Camera cam;
    cam.init();
    cam.change_settings(CameraDriver::Setting::QUALITY, 63);
    cam.change_settings(CameraDriver::Setting::SHARPNESS, 2);
    cam.change_settings(CameraDriver::Setting::SATURATION, -2);

    //I2cTempDriver::Temperature therm;
    //therm.init();

    ConsoleCommander::Commander cmd{&lcd, &conn, &cam};
    lcd.draw_line(0, LCD_H - LcdDriver::font_size - 5, LCD_W, LCD_H - LcdDriver::font_size - 5, 0xff, 0xff, 0xff);

    while (true) {
        cmd.therm_update(22.0, 40.0);
        cmd.loop();
        //therm.loop();
    }
}
