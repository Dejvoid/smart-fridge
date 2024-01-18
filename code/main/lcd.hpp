/**
 * Datasheet: https://www.hpinfotech.ro/ILI9488.pdf
 * https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
 * https://www.displayfuture.com/Display/datasheet/controller/ILI9488.pdf
 * Resolution: 320x480
 * SPI Mode: 4-wire
*/

#ifndef LCD_HPP_
#define LCD_HPP_

#include <driver/spi_master.h>
#include <driver/gpio.h>

namespace LcdDriver {

// MOSI pin number
constexpr gpio_num_t MOSI = GPIO_NUM_13;
//constexpr gpio_num_t MISO = GPIO_NUM_12;
// SCLK pin number
constexpr gpio_num_t SCLK = GPIO_NUM_14;
// CS pin number
constexpr gpio_num_t CS   = GPIO_NUM_15;
// D/C pin number
constexpr gpio_num_t DC   = GPIO_NUM_2;
// RST pin number
constexpr gpio_num_t RST  = GPIO_NUM_12;

// Width in pixels
constexpr uint16_t WIDTH = 320;
// Height in pixels
constexpr uint16_t HEIGHT = 480;

enum class ColorMode {
    // Color mode 1-1-1
    RGB_111 = 0b00000001,
    // Color mode 6-6-6
    RGB_666 = 0b00000110,
    // [Not supported on ILI9488] Color mode 5-6-5
    //RGB_565 = 0b00000101 
};

enum class Command {
    // Software reset command
    SW_RST    = 0x01,
    // Sleep out command
    SLP_OUT   = 0x11,
    // Display on command
    DISP_ON   = 0x29,
    // Set pixel color format command
    PIX_COL   = 0x3A,
    // Set column address command
    CA_SET    = 0x2A,
    // Set page address command
    PA_SET    = 0x2B,
    // Memory write command
    MEM_WR    = 0x2C,
};

void lcd_pre_transfer_callback(spi_transaction_t* trans);

class Lcd {
    spi_device_handle_t spi_handle;
    constexpr static spi_host_device_t spi_dev = HSPI_HOST;
    spi_bus_config_t spi_cfg;
    spi_device_interface_config_t spi_if_cfg;
    uint8_t* buff;
public:
    void init();
    void send_command(Command cmd);
    void send_data(const uint8_t* data, int len);
    void send_data(const uint8_t);
    void draw_rect(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint8_t r, const uint8_t g, const uint8_t b);
    ~Lcd() {spi_device_release_bus(spi_handle);}
};
}
#endif