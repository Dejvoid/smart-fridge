/**
 * temp_i2c.hpp
 * This file contains definition of the I2C temperature driver. 
 * Datasheet: https://server4.eca.ir/eshop/AHT10/Aosong_AHT10_en_draft_0c.pdf 
 */
#ifndef TEMP_I2C_HPP_
#define TEMP_I2C_HPP_

#include <driver/gpio.h>
#include <driver/i2c.h>

namespace I2cTempDriver {

/**
 * Pins and constants definitions
*/
constexpr gpio_num_t sda_pin = GPIO_NUM_4;
constexpr gpio_num_t sck_pin = GPIO_NUM_15;
constexpr int i2c_chan       = 1;
constexpr uint32_t clk_speed = 400000;
constexpr i2c_port_t i2c_port = i2c_port_t::I2C_NUM_0;
constexpr int i2c_timeout = 1000;

/// @brief 7-bit I2C address of the device
constexpr uint8_t i2c_addr = 0x38;
/// @brief Reset command
constexpr uint8_t rst_cmd = 0b10111010;
/// @brief Lenght of the measurement command in bytes
constexpr int meas_cmd_len = 3;
/// @brief Measurement invocation command
constexpr uint8_t meas_cmd[meas_cmd_len] = { 0xAC, 0b00110011, 0 };
/// @brief Measurement data length in bytes
constexpr int rd_len = 6;

/// @brief Takes care of temperature and humidity sensor.
class Temperature {
    /// @brief Buffer for data read
    uint8_t rd_buf[rd_len];
    float temp_;
    float hum_;
public:
    /// @brief Initialize thermometer to start operating
    void init();
    /// @brief Call this in the loop. Starts meaasurement and updates temperature and humidity values
    void loop();
    /// @brief Get measured temperature
    /// @return Measured temperature in Celsius
    inline float get_temp();
    /// @brief Get measured humidity
    /// @return Measured humidity in percents
    inline float get_hum();
};

}

#endif