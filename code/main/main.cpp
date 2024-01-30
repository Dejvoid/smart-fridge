
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include "one_wire.hpp"
#include "lcd.hpp"
#include "wifi.hpp"
#include "camera.hpp"
#include <iostream>
#include <cstring>
#include <driver/spi_master.h>
#include <iostream>
#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>
#include <esp_http_client.h>
#include <esp_code_scanner.h>
#include <driver/gpio.h>

#include <esp_rom_sys.h>

uint8_t buffer[240*240*3 / 2];

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
constexpr uint16_t LCD_W            = 320;
constexpr uint16_t LCD_H            = 480;

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
    
    //vTaskDelay(10 / portTICK_PERIOD_MS);
    //OneWireDriver::OneWire ow;
    //ow.init();

    lcd.draw_rect(0,0,320,480,0x00,0x00,0x03);
    lcd.draw_line(0,0,320,480,0xff,0xff,0xff);
    lcd.draw_char('A', 270, 430, 0xff, 0xff, 0xff);
    lcd.draw_text("!@#$%^&*()-_=+[]{};:'\"\\|,<.>/?`~", 270, 440, 0xff, 0x00, 0x00);

    lcd.draw_rect(50, 50, 240, 240, 0xf0, 0x00, 0x00);
    
    while (true) {
        auto fb = esp_camera_fb_get();
        if (fb == NULL) {
            std::cout << "Error getting framebuffer" << std::endl;
        }
        else {
            int j = 0;
            for (int i = 0; i < 240 * 240 * 2; i+=2) {
                auto r = (fb->buf[i] & 0xf8);
                auto g = ((fb->buf[i] & 0x07) << (3+2)) | ((fb->buf[i+1] & 0xe0) >> 3);
                auto b = (fb->buf[i+1] & 0x1f) << 3;
                
                buffer[j++] = b; 
                buffer[j++] = g;
                buffer[j++] = r;
                if (j >= 240*240*3 / 2) {
                    j = 0;
                    lcd.send_data(buffer, 240*240*3 / 2);
                }
            }
        }
        esp_camera_fb_return(fb);
        cam.loop();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

   
}
