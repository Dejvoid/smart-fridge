/**
 * Datasheet: 
 * https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
 * https://www.displayfuture.com/Display/datasheet/controller/ILI9488.pdf
 * Resolution: 320x480
 * SPI Mode: 4-wire
*/

#ifndef LCD_HPP_
#define LCD_HPP_

#include "font.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include <iostream>
#include <esp_heap_caps.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>


namespace LcdDriver {

enum class ColorMode {
    // Color mode 1-1-1
    RGB_111 = 0b00000001,
    // Color mode 6-6-6
    RGB_666 = 0b00000110,
    // [Not supported on ILI9488 over SPI] Color mode 5-6-5
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
    // Memory Access Control
    MADCTL    = 0x36,
};

constexpr int buff_len = 50*50*3; // We want to be able to draw 50x50 of display at once
static uint8_t buff[buff_len];

struct LcdPins {
    int mosi;        // SPI MOSI pin
    int miso;        // SPI MISO pin (set -1 if not used)
    int sck;         // SPI SCK pin
    int cs;          // SPI CS pin (set -1 if not used)
    gpio_num_t rst;  // Display RST pin 
    gpio_num_t dc;   // Display D/C pin 
    constexpr LcdPins(int mosi, int miso, int sck, int cs, gpio_num_t rst, gpio_num_t dc) : mosi(mosi), miso(miso), sck(sck), cs(cs), rst(rst), dc(dc) {};
};

template <LcdPins PINS>
void lcd_pre_transfer_callback(spi_transaction_t* trans);

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
class Lcd {
    spi_device_handle_t spi_handle;
public:
    Lcd();
    void init();
    void send_command(Command cmd);
    void send_data(const uint8_t* data, int len);
    void send_data(const uint8_t);
    void send_addr_pair(const uint16_t a1, const uint16_t a2);
    void set_area(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y);
    void draw_rect(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint8_t r, const uint8_t g, const uint8_t b);
    void draw_line(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y, const uint8_t r, const uint8_t g, const uint8_t b);
    void draw_char(char c, uint16_t x, uint16_t y,  uint8_t r, uint8_t g, uint8_t b);
    void draw_text(char* str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);

    ~Lcd() {spi_device_release_bus(spi_handle);}
};

template <LcdPins PINS>
void IRAM_ATTR lcd_pre_transfer_callback(spi_transaction_t* t) {
    int dc = (int)t->user;
    gpio_set_level(PINS.dc, dc);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
Lcd<SPI, PINS, W, H>::Lcd() {}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::init() {
    constexpr spi_bus_config_t spi_cfg {
        .mosi_io_num = PINS.mosi,
        .miso_io_num = PINS.miso,
        .sclk_io_num = PINS.sck,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .max_transfer_sz = W * H * 3,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_SCLK,
        .isr_cpu_id = intr_cpu_id_t::INTR_CPU_ID_AUTO,
        .intr_flags = 0
    };
    constexpr spi_device_interface_config_t spi_if_cfg {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .clock_source = spi_clock_source_t::SPI_CLK_SRC_DEFAULT,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_MASTER_FREQ_40M,
        .input_delay_ns = 0,
        .spics_io_num = PINS.cs,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 1,
        .pre_cb = lcd_pre_transfer_callback<PINS>,
        .post_cb = NULL
    };
    constexpr gpio_config_t gpio_cfg {
        .pin_bit_mask = (1 << PINS.dc) | (1 << PINS.rst),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI, &spi_cfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI, &spi_if_cfg, &spi_handle));
    
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    gpio_set_level(PINS.rst, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PINS.rst, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(spi_device_acquire_bus(spi_handle, portMAX_DELAY));

    send_command(Command::SW_RST); // software reset
    vTaskDelay(150 / portTICK_PERIOD_MS);

    send_command(Command::SLP_OUT);  // Sleep Out

    send_command(Command::DISP_ON); // display on

// Color Mode
    send_command(Command::PIX_COL); // interface pixel format
    send_data((uint8_t)ColorMode::RGB_666); // 3 bit per pixel

    set_area(0,0, W, H);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_command(Command cmd) {
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_data[0] = (uint8_t)cmd;             //The data is the cmd itself
    t.user = (void*)0;              //D/C needs to be set to 0
    t.flags = SPI_TRANS_USE_TXDATA;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t));

}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_data(const uint8_t* data, int len) {
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

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_data(const uint8_t data) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.user = (void*)1;              //D/C needs to be set to 1
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle, &t)); //Transmit!
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_addr_pair(const uint16_t a1, const uint16_t a2) {
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

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::set_area(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y) {
    send_command(Command::CA_SET);
    send_addr_pair(from_x, to_x);

	send_command(Command::PA_SET);
    send_addr_pair(from_y, to_y);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_rect(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h, const uint8_t r, const uint8_t g, const uint8_t b) {
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

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_line(const uint16_t from_x, const uint16_t from_y, const uint16_t to_x, const uint16_t to_y, const uint8_t r, const uint8_t g, const uint8_t b) {
    uint16_t dx = to_x - from_x;
    uint16_t dy = to_y - from_y;
    for (auto x = from_x; x < to_x; ++x) {
        auto y = from_y + dy * (x - from_x) / dx;
        draw_rect(x,y,1,1,r,g,b);
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_char(char c, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    auto bmap = font8x8_basic[(int)c];
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (bmap[i] & (1 << j)) {
                draw_rect(x + (7-j),y + i,1,1,r,g,b);
            }
        }
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_text(char* str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    int i = 0;
    while (str[i] != '\0') {
        draw_char(str[i], x - 8*i, y, r, g, b);
        ++i;
    }
}

}
#endif