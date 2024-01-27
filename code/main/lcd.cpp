#include "lcd.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include <iostream>
#include <esp_heap_caps.h>

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
    spi_cfg.max_transfer_sz = 320*480*3;
    ESP_ERROR_CHECK(spi_bus_initialize(spi_dev, &spi_cfg, SPI_DMA_CH_AUTO));

    spi_if_cfg.address_bits = 0;
    spi_if_cfg.clock_source = spi_clock_source_t::SPI_CLK_SRC_DEFAULT;
    spi_if_cfg.clock_speed_hz = SPI_MASTER_FREQ_40M;
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
    spi_if_cfg.queue_size = 1;
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
    send_data((uint8_t)ColorMode::RGB_666); // 3 bit per pixel

    set_area(0,0, WIDTH, HEIGHT);
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
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.user = (void*)1;              //D/C needs to be set to 1
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t)); //Transmit!
}

void Lcd::send_addr_pair(const uint16_t a1, const uint16_t a2) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.user = (void*)1;              //D/C needs to be set to 1
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8 * (sizeof(a1) + sizeof(a2));
    t.tx_data[0] = a1 >> 8;
    t.tx_data[1] = a1 & 0xFF;
    t.tx_data[2] = a2 >> 8;
    t.tx_data[3] = a2 & 0xFF;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t)); //Transmit!
}

void Lcd::set_area(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y) {
    send_command(Command::CA_SET);
    send_addr_pair(from_x, to_x);

	send_command(Command::PA_SET);
    send_addr_pair(from_y, to_y);
}

void Lcd::draw_rect(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint8_t r, const uint8_t g, const uint8_t b) {
    set_area(x,y,(x + w - 1), (y + h - 1));

    send_command(Command::MEM_WR);
    auto len = w*h*3;
    int j = 0;
    int ctr = 0;
    while (j < len) {
        for (int i = 0; i < buff_len && j < len; i+=3, j+=3) {
            buff[i]   = b;
            buff[i+1] = g;
            buff[i+2] = r;
            ctr+=3;
        }
        if (j < len || j % buff_len == 0) {
            send_data(buff, buff_len);
        }
        else {
            send_data(buff, (len % buff_len));
        }
    }
}

void Lcd::draw_line(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y, const uint8_t r, const uint8_t g, const uint8_t b) {
    uint16_t dx = to_x - from_x;
    uint16_t dy = to_y - from_y;
    for (auto x = from_x; x < to_x; ++x) {
        auto y = from_y + dy * (x - from_x) / dx;
        draw_rect(x,y,1,1,r,g,b);
    }
}

void Lcd::draw_char(char c, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    auto bmap = font8x8_basic[(int)c];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (bmap[i] & (1 << j)) {
                draw_rect(x + (7-j),y + i,1,1,r,g,b);
            }
        }
    }
}
    
void Lcd::draw_text(char* str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    int i = 0;
    while (str[i] != '\0') {
        draw_char(str[i], x - 8*i, y, r, g, b);
        ++i;
    }
}