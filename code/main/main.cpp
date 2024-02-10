
#include "lcd.hpp"
#include "wifi.hpp"
#include "camera.hpp"
#include "commander.hpp"

#include <driver/gpio.h>
#include <esp_http_client.h>
#include <nvs_flash.h>

constexpr size_t bufflen = 320 * 50 * 3;
uint8_t buffer[bufflen];

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

extern "C" void app_main(void)
{   
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    WifiDriver::Wifi wifi;
    wifi.init();

    constexpr LcdDriver::LcdPins lcd_pins{LCD_MOSI, LCD_MISO, LCD_SCK, LCD_CS, LCD_RST, LCD_DC};
    LcdDriver::Lcd<LCD_SPI, lcd_pins, LCD_W, LCD_H> lcd;
    lcd.init();

    CameraDriver::Camera cam;
    cam.init();
    
    //I2cTempDriver::Temperature therm;
    //therm.init();

    //vTaskDelay(10 / portTICK_PERIOD_MS);
    //OneWireDriver::OneWire ow;
    //ow.init();

    ConsoleCommander::Commander cmd{&lcd};

    while (true) {
        cmd.loop();
        //therm.loop();
        auto fb = esp_camera_fb_get();
        if (fb == NULL) {
            std::cout << "Error getting framebuffer" << std::endl;
        }
        else {
            //lcd.draw_grayscale(fb->buf, 0, 0, fb->height, fb->width); // for some reason we have to exchange height with width
            lcd.draw_565buff(fb->buf, 0, 0, fb->height, fb->width); // for some reason we have to exchange height with width
        }
        esp_camera_fb_return(fb);
        //vTaskDelay(100 / portTICK_PERIOD_MS);
        //cam.loop();
    }

   
}
