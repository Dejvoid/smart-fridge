
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

#define TFT_CMD_SWRESET		0x01
#define TFT_CMD_DELAY	    0x80
#define TFT_MADCTL	        0x36
#define MADCTL_MX           0x40
#define TFT_RGB_BGR         0x00
#define TFT_CMD_PIXFMT	    0x3A 
#define DISP_COLOR_BITS_24  0x66

#define TFT_CMD_DELAY	    0x80

static const uint8_t ILI9488_init[] = {
    18,                   					        // 18 commands in list
    TFT_CMD_SWRESET, TFT_CMD_DELAY,					//  1: Software reset, no args, w/delay
    200,												//     200 ms delay
    0xE0, 15, 0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F,
    0xE1, 15,	0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F,
    0xC0, 2,   //Power Control 1
	  0x17,    //Vreg1out
	  0x15,    //Verg2out

    0xC1, 1,   //Power Control 2
	  0x41,    //VGH,VGL

    0xC5, 3,   //Power Control 3
	  0x00,
	  0x12,    //Vcom
	  0x80,


    TFT_MADCTL, 1, (MADCTL_MX | TFT_RGB_BGR),			// Memory Access Control (orientation), set to portrait


    // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit;
    TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_24,

    0xB0, 1,   // Interface Mode Control
	  0x00,    // 0x80: SDO NOT USE; 0x00 USE SDO

    0xB1, 1,   //Frame rate
	  0xA0,    //60Hz

    0xB4, 1,   //Display Inversion Control
	  0x02,    //2-dot

    0xB6, 2,   //Display Function Control  RGB/MCU Interface Control
	  0x02,    //MCU
	  0x02,    //Source,Gate scan direction

    0xE9, 1,   // Set Image Function
	  0x00,    // Disable 24 bit data

    0x53, 1,   // Write CTRL Display Value
	  0x28,    // BCTRL && DD on

    0x51, 1,   // Write Display Brightness Value
	  0x7F,    //

    0xF7, 4,   // Adjust Control
	  0xA9,
	  0x51,
	  0x2C,
	  0x02,    // D7 stream, loose


    0x11, TFT_CMD_DELAY,  //Exit Sleep
      120,
    0x29, 0,      //Display on

};



extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    std::cout << "FLASH INITIALIZED" << std::endl;


    WifiDriver::Wifi wifi;
    wifi.init();

    CameraDriver::Camera cam;
    cam.init();

    
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
    
    //lcd.send_command(0x3A); // interface pixel format
    //lcd.send_data(0b01010101); // 16 bit per pixel

    constexpr uint16_t _x1 = 0;
	constexpr uint16_t _x2 = 10;
	constexpr uint16_t _y1 = 0;
	constexpr uint16_t _y2 = 100;

	//lcd.send_command(0x2A);	// set column(x) address
    //lcd.send_data(0);
    //lcd.send_data((uint8_t)_x1);
    //lcd.send_data(0);
    //lcd.send_data((uint8_t)_x2);
////
	//lcd.send_command(0x2B);	// set Page(y) address
    //lcd.send_data(0);
    //lcd.send_data((uint8_t)_y1);
    //lcd.send_data(0);
    //lcd.send_data((uint8_t)_y2);


	lcd.send_command(0x2C);	//	Memory Write
    //vTaskDelay(500 / portTICK_PERIOD_MS);
    constexpr int byte_cnt = 320;// (_x2 - _x1 + 1)*(_y2 - _y1 + 1);
    uint16_t color[byte_cnt];
    for (int i = 0; i < byte_cnt; ++i) {
        color[i] = 0x0ff0;
    }
	for(int i=_x1;i<=_x2;i++){
        vTaskDelay(1 / portTICK_PERIOD_MS);
        lcd.send_data((uint8_t*)color, byte_cnt*2);
	}
    lcd.send_command(0x00);
    std::cout << "END CUNT" << std::endl;
    while (true)
    {
        vTaskDelay(0);
    }

/*
while (1) {
        cam.loop();
    }*/

   
}
