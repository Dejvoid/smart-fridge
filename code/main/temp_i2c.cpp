/**
 * temp_i2c.cpp
 * This file contains implementation of AHT10 I2C driver.
 */
#include "temp_i2c.hpp"

#include <driver/i2c.h>

using namespace I2cTempDriver;

void Temperature::init() {
    constexpr i2c_config_t cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = sck_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = clk_speed},
        .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };

    i2c_param_config(i2c_port, &cfg);
    
    i2c_driver_install(i2c_port, cfg.mode, 0, 0, 0);

    // Soft reset the device
    i2c_master_write_to_device(i2c_port, i2c_addr, &rst_cmd,1, i2c_timeout / portTICK_PERIOD_MS);
    vTaskDelay(20 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(i2c_port, i2c_addr, meas_cmd, meas_cmd_len, i2c_timeout / portTICK_PERIOD_MS);
};

void Temperature::loop() {
    rd_buf[0] = 0;
    i2c_master_read_from_device(i2c_port, i2c_addr, rd_buf, rd_len, i2c_timeout / portTICK_PERIOD_MS);
    if (rd_buf[0]) {
        //printf("RECEIVED SOME DATA\n");
        //printf("State: %d\nHumidity: %d %d \nHumTemp: %d\nTemp: %d %d\n", rd_buf[0], rd_buf[1], rd_buf[2], rd_buf[3], rd_buf[4], rd_buf[5]);
        uint32_t tempRaw = ((uint32_t)rd_buf[3] & 0x0f) << 16 | ((uint32_t)rd_buf[4]) << 8 | (uint32_t)rd_buf[5];
        temp_ = ((float)tempRaw / (1 << 20)) * 200 - 50; 
        uint32_t humRaw = ((uint32_t)rd_buf[1] << 12) | ((uint32_t)rd_buf[2] << 4) | ((uint32_t)rd_buf[3] >> 4);
        hum_ = ((float)humRaw / (1 << 20)) * 100;
        //printf("TempRaw %ld HumRaw %ld\n", tempRaw, humRaw);
        //printf("Temperature: %f \nHumidity: %f%%\n", temp, hum);
        i2c_master_write_to_device(i2c_port, i2c_addr, meas_cmd, meas_cmd_len, i2c_timeout / portTICK_PERIOD_MS);
    }
}

float Temperature::get_hum() {
    return hum_;
}

float Temperature::get_temp() {
    return temp_;
}