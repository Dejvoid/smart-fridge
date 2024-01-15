
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

#include <esp_rom_sys.h>

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //WifiDriver::Wifi wifi;
    //wifi.init();
//
    //CameraDriver::Camera cam;
    //cam.init();

    
    LcdDriver::Lcd lcd;
    lcd.init();


    lcd.send_command(0x01); // software reset
    vTaskDelay(150 / portTICK_PERIOD_MS);

    lcd.send_command(0x11);  // Sleep Out
    //vTaskDelay(100 / portTICK_PERIOD_MS);

    //lcd.send_command(0x13);  // normal display mode
    ////vTaskDelay(100 / portTICK_PERIOD_MS);
//
    lcd.send_command(0x29); // display on

//// Gamma settings
//    // Positive
//    constexpr uint8_t data[15] = { 0x00, 0x03, 0x09, 0x08, 0x16,
//                         0x0A, 0x3F, 0x78, 0x4C, 0x09,
//                         0x0A, 0x08, 0x16, 0x1A, 0x0F };
//    lcd.send_command(0xE0);
//    lcd.send_data(data, 15);
//
//    // Negative
//    constexpr uint8_t data2[15] = { 0x00, 0x16, 0x19, 0x03, 0x0F,
//                         0x05, 0x32, 0x45, 0x46, 0x04,
//                         0x0E, 0x0D, 0x35, 0x37, 0x0F};
//    lcd.send_command(0xE1);
//    lcd.send_data(data2, 15);
//// Power settings
//    constexpr uint8_t data3[2] = { 0x17, 0x15 };
//    lcd.send_command(0xC0);
//    lcd.send_data(data3, 2);
//
//    lcd.send_command(0xC1);
//    lcd.send_data(0x41);
//
//    constexpr uint8_t data4[3] = { 0x00, 0x12, 0x80 };
//    lcd.send_command(0xC5);
//    lcd.send_data(data4, 3);
//
//// Memory access control
//
//// Color Mode
    //lcd.send_command(0x3A); // interface pixel format
    //lcd.send_data(0x55); // 16 bit per pixel
//    
    

    constexpr uint16_t _x1 = 0;
	constexpr uint16_t _x2 = 100;
	constexpr uint16_t _y1 = 0;
	constexpr uint16_t _y2 = 100;

	lcd.send_command(0x2A);	// set column(x) address
    lcd.send_data(0);
    lcd.send_data((uint8_t)_x1);
    lcd.send_data(0);
    lcd.send_data((uint8_t)_x2);
////
	lcd.send_command(0x2B);	// set Page(y) address
    lcd.send_data(0);
    lcd.send_data((uint8_t)_y1);
    lcd.send_data(0);
    lcd.send_data((uint8_t)_y2);


	lcd.send_command(0x2C);	//	Memory Write
    for (int i = 0; i < 480*200; ++i) {
        lcd.send_data(0xff); // B
        lcd.send_data(0x00); // G
        lcd.send_data(0xff); // R
    }

    std::cout << "END" << std::endl;
    while (true)
    {
        vTaskDelay(1);
    }
   
}
