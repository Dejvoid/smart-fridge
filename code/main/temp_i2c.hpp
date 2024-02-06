#ifndef TEMP_I2C_HPP_
#define TEMP_I2C_HPP_

#include <driver/gpio.h>
#include <driver/i2c.h>

namespace I2cTempDriver {

constexpr gpio_num_t sda_pin = GPIO_NUM_4;
constexpr gpio_num_t sck_pin = GPIO_NUM_15;
constexpr int i2c_chan       = 1;
constexpr uint32_t clk_speed = 400000;
constexpr i2c_port_t i2c_port = i2c_port_t::I2C_NUM_0;
constexpr int i2c_timeout = 1000;

constexpr uint8_t i2c_addr = 0x38;
constexpr uint8_t rst_cmd = 0b10111010;
constexpr int meas_cmd_len = 3;
constexpr uint8_t meas_cmd[meas_cmd_len] = { 0xAC, 0b00110011, 0 };
constexpr int rd_len = 6;

class Temperature {
    uint8_t rd_buf[rd_len];
public:
    void init();
    void loop();
};

}

#endif