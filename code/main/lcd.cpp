#include "lcd.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include <iostream>

using namespace LcdDriver;

void IRAM_ATTR LcdDriver::lcd_pre_transfer_callback(spi_transaction_t* t) {
    int dc = (int)t->user;
    gpio_set_level(DC, dc);
}

void Lcd::init() {
    spi_cfg.data4_io_num = -1;
    spi_cfg.data5_io_num = -1;
    spi_cfg.data6_io_num = -1;
    spi_cfg.data7_io_num = -1;
    spi_cfg.mosi_io_num = MOSI;
    spi_cfg.miso_io_num = -1;
    spi_cfg.sclk_io_num = SCLK;
    spi_cfg.quadhd_io_num = -1;
    spi_cfg.quadwp_io_num = -1;
    spi_cfg.flags = SPICOMMON_BUSFLAG_MASTER /*| SPICOMMON_BUSFLAG_IOMUX_PINS | SPICOMMON_BUSFLAG_MISO*/ | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_SCLK;
    spi_cfg.isr_cpu_id = intr_cpu_id_t::INTR_CPU_ID_AUTO;
    spi_cfg.intr_flags = 0;
    spi_cfg.max_transfer_sz = 4092;
    ESP_ERROR_CHECK(spi_bus_initialize(spi_dev, &spi_cfg, SPI_DMA_CH_AUTO));

    spi_if_cfg.address_bits = 0;
    spi_if_cfg.clock_source = spi_clock_source_t::SPI_CLK_SRC_DEFAULT;
    spi_if_cfg.clock_speed_hz = SPI_MASTER_FREQ_8M;
    spi_if_cfg.command_bits = 0;
    spi_if_cfg.cs_ena_posttrans = 0;
    spi_if_cfg.cs_ena_pretrans = 0;
    spi_if_cfg.dummy_bits = 0;
    spi_if_cfg.duty_cycle_pos = 0;
    spi_if_cfg.flags = SPI_DEVICE_HALFDUPLEX;
    spi_if_cfg.input_delay_ns = 0;
    spi_if_cfg.mode = 0;
    spi_if_cfg.post_cb = NULL;
    spi_if_cfg.pre_cb = lcd_pre_transfer_callback;
    spi_if_cfg.queue_size = 7;
    spi_if_cfg.spics_io_num = CS;

    ESP_ERROR_CHECK(spi_bus_add_device(spi_dev, &spi_if_cfg, &spi_handle));

    gpio_config_t io_conf; //   DC/RS       RST        backlight
    io_conf.pin_bit_mask = (1 << DC) | (1 << RST) ;//| (1 << )
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    
    ESP_ERROR_CHECK(gpio_config(&io_conf));


    gpio_set_level(RST, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(RST, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(spi_device_acquire_bus(spi_handle, portMAX_DELAY));

    send_command(Command::SW_RST); // software reset
    vTaskDelay(150 / portTICK_PERIOD_MS);

    send_command(Command::SLP_OUT);  // Sleep Out

    send_command(Command::DISP_ON); // display on

// Color Mode
    send_command(Command::PIX_COL); // interface pixel format
    send_data((uint8_t)ColorMode::RGB_111); // 3 bit per pixel

    send_command(Command::CA_SET);
    send_data(0);
    send_data(0);
    send_data(WIDTH >> 8);
    send_data(WIDTH & 0xFF);

	send_command(Command::PA_SET);
    send_data(0);
    send_data(0);
    send_data(HEIGHT >> 8);
    send_data(HEIGHT & 0xFF);
}

void Lcd::send_command(Command cmd) {
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_data[0] = (uint8_t)cmd;             //The data is the cmd itself
    t.user = (void*)0;              //D/C needs to be set to 0
    t.flags = SPI_TRANS_USE_TXDATA;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t));

}

void Lcd::send_data(const uint8_t* data, int len) {
     if (len == 0) {
        return;
    }
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = len * 8;             //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;             //Data
    t.user = (void*)1;              //D/C needs to be set to 1
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t)); //Transmit!
}

void Lcd::send_data(const uint8_t data) {
    send_data(&data, 1);
}

void Lcd::draw_rect(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint8_t r, const uint8_t g, const uint8_t b) {
    send_command(Command::CA_SET);
    send_data(x >> 8);
    send_data(x & 0xFF);
    send_data((x + w) >> 8);
    send_data((x + w) & 0xFF);

	send_command(Command::PA_SET);
    send_data(y >> 8);
    send_data(y & 0xFF);
    send_data((y + h) >> 8);
    send_data((y + h) & 0xFF);

    
    send_command(Command::MEM_WR);

    //for (int i = 0; i < w*h ; ++i) {
    //    send_data(0b11111111);
    //    
    //}
    //if (w*h*3 < BUFF_SIZE) {
    //    for (int i = 0; i < w*h*3; i+=3) {
    //        lcd_buff[i] = b;
    //        lcd_buff[i + 1] = g;
    //        lcd_buff[i + 2] = r;
    //    }
    //    send_data(lcd_buff, w*h*3);
    //}
    //else 
        for (int i = 0; i < w*h / 6; i++) {
            //send_data(b & 0xfc);
                send_data(0b00100100);//g); // Blue
                //send_data(0b00010010);      // Green
                //send_data(0b00001001);      // Red
            //send_data(0);//r);
        }
        for (int i = 0; i < w*h / 6; i++) {
            //send_data(b & 0xfc);
                //send_data(0b00100100);//g); // Blue
                send_data(0b00010010);      // Green
                //send_data(0b00001001);      // Red
            //send_data(0);//r);
        }
        for (int i = 0; i < w*h / 6; i++) {
            //send_data(b & 0xfc);
                //send_data(0b00100100);//g); // Blue
                //send_data(0b00010010);      // Green
                send_data(0b00001001);      // Red
            //send_data(0);//r);
        }
}