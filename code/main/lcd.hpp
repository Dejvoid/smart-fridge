/**
 * Datasheet: https://www.hpinfotech.ro/ILI9488.pdf
 * https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
 * https://www.displayfuture.com/Display/datasheet/controller/ILI9488.pdf
*/
// 320 (RGB) (H) x 480 (V) x 18 bits
#ifndef LCD_HPP_
#define LCD_HPP_

#include <driver/spi_master.h>
#include <driver/gpio.h>

namespace LcdDriver {

constexpr gpio_num_t MOSI = GPIO_NUM_13;
constexpr gpio_num_t MISO = GPIO_NUM_12;
constexpr gpio_num_t SCLK = GPIO_NUM_14;
constexpr gpio_num_t CS   = GPIO_NUM_15;
constexpr gpio_num_t DC   = GPIO_NUM_2;

void lcd_pre_transfer_callback(spi_transaction_t* trans);

class Lcd {
    spi_device_handle_t spi_handle;
    constexpr static spi_host_device_t spi_dev = HSPI_HOST;
    spi_bus_config_t spi_cfg;
    spi_device_interface_config_t spi_if_cfg;
public:
    void init();
    void send_command(uint8_t cmd);
    void send_data(const uint8_t* data, int len);
    void send_data(const uint8_t);
    ~Lcd() {spi_device_release_bus(spi_handle);}
};
}
#endif