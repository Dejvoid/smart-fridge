/**
 * Datasheet: 
 * https://www.displayfuture.com/Display/datasheet/controller/ILI9488.pdf
 * Resolution: 480x320
 * SPI Mode: 4-wire
*/

#ifndef LCD_HPP_
#define LCD_HPP_

#include "font.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstring>
#include <iostream>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_heap_caps.h>


namespace LcdDriver {

/// @brief Color modes of the LCD
enum class ColorMode {
    /// @brief Color mode 1-1-1
    RGB_111 = 0b00000001,
    /// @brief Color mode 6-6-6
    RGB_666 = 0b00000110,
    // [Not supported on ILI9488 over SPI] Color mode 5-6-5
    //RGB_565 = 0b00000101 
};

/// @brief Possible commands to control LCD
enum class Command {
    /// @brief Software reset command
    SW_RST    = 0x01,
    /// @brief Sleep out command
    SLP_OUT   = 0x11,
    /// @brief Display on command
    DISP_ON   = 0x29,
    /// @brief Set pixel color format command
    PIX_COL   = 0x3A,
    /// @brief Set column address command
    CA_SET    = 0x2A,
    /// @brief Set page address command
    PA_SET    = 0x2B,
    /// @brief Memory write command
    MEM_WR    = 0x2C,
    /// @brief Memory Access Control
    MADCTL    = 0x36,
    /// @brief Frame rate control
    FRMCTR    = 0xB1,
};

/// @brief Struct for defining pins for LCD
struct LcdPins {
    /// @brief SPI MOSI pin
    int mosi;
    /// @brief SPI MISO pin (set -1 if not used)
    int miso;
    /// @brief SPI SCK pin
    int sck;
    /// @brief SPI CS pin (set -1 if not used)
    int cs;
    /// @brief LCD RST pin
    gpio_num_t rst;
    /// @brief LCD D/C pin (Data/Command)
    gpio_num_t dc;
    constexpr LcdPins(int mosi, int miso, int sck, int cs, gpio_num_t rst, gpio_num_t dc) : mosi(mosi), miso(miso), sck(sck), cs(cs), rst(rst), dc(dc) {};
};

/**
 * Interface for the Lcd
*/
class LcdBase {
public:
    /// @brief Gets width of the display
    /// @return Width
    constexpr virtual uint16_t get_w() = 0;
    /// @brief Gets height of the display
    /// @return Height
    constexpr virtual uint16_t get_h() = 0;
    /// @brief Draws single color rectangle on the given position
    /// @param x X position of top left corner
    /// @param y Y position of top left corner
    /// @param w Width
    /// @param h Height
    /// @param r Red
    /// @param g Green
    /// @param b Blue
    virtual void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b) = 0;
    /// @brief Draws line of given color
    /// @param from_x Start X
    /// @param from_y Start Y
    /// @param to_x End X
    /// @param to_y End Y
    /// @param r Red
    /// @param g Green
    /// @param b Blue
    virtual void draw_line(uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y, uint8_t r, uint8_t g, uint8_t b) = 0;
    /// @brief Draws single character of given color on given position
    /// @param c Character to draw
    /// @param x Top left X
    /// @param y Top left Y
    /// @param r Red
    /// @param g Green
    /// @param b Blue
    virtual void draw_char(char c, uint16_t x, uint16_t y,  uint8_t r, uint8_t g, uint8_t b) = 0;
    /// @brief Draws string of given color on given position
    /// @param str String to draw
    /// @param x Top left X
    /// @param y Top left Y
    /// @param r Red
    /// @param g Green
    /// @param b Blue
    virtual void draw_text(const std::string& str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) = 0;
    /// @brief Draws 6-6-6 RGB buffer in given rectangle
    /// @param buff Buffer to draw
    /// @param x Top left X
    /// @param y Top left Y
    /// @param w Width
    /// @param h Height
    virtual void draw_buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    /// @brief Draws grayscale buffer in given rectangle
    /// @param buff Buffer to draw
    /// @param x Top left X
    /// @param y Top left Y
    /// @param w Width
    /// @param h Height
    virtual void draw_grayscale(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
    /// @brief Draws 5-6-5 RGB buffer in given rectangle
    /// @param buff Buffer to draw
    /// @param x Top left X
    /// @param y Top left Y
    /// @param w Width
    /// @param h Height
    virtual void draw_565buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
};

/// @brief Implementation of LCD driver
/// @tparam SPI - selected SPI host (VSPI_HOST, HSPI_HOST, ...)
/// @tparam W - Width of the display
/// @tparam H - Height of the display
/// @tparam PINS - Pins that the display uses
template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
class Lcd : public LcdBase {
    /// @brief Handle of the LCD SPI device
    spi_device_handle_t spi_handle_;
    /// @brief Length of balancing buffer
    constexpr static size_t buff_len_ = W * 50 * 3;
    /// @brief Balancing buffer
    uint8_t* buff_;
    void send_command(Command cmd);
    void send_data(const uint8_t* data, int len);
    void send_data(uint8_t);
    void send_addr_pair(uint16_t a1, uint16_t a2);
    void set_area(uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y);
public:
    Lcd();
    ~Lcd();
    /// @brief Initialize LCD
    void init();
    constexpr uint16_t get_w() override { return W; }
    constexpr uint16_t get_h() override { return H; }
    void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b) override;
    void draw_line(uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y, uint8_t r, uint8_t g, uint8_t b) override;
    void draw_char(char c, uint16_t x, uint16_t y,  uint8_t r, uint8_t g, uint8_t b) override;
    void draw_text(const std::string& str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) override;
    void draw_buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
    void draw_grayscale(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
    void draw_565buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) override;
};

template <LcdPins PINS>
void IRAM_ATTR lcd_pre_transfer_callback(spi_transaction_t* t) {
    int dc = (int)t->user;
    gpio_set_level(PINS.dc, dc);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
Lcd<SPI, PINS, W, H>::Lcd() {
    buff_ = (uint8_t*)heap_caps_malloc(buff_len_, MALLOC_CAP_DMA);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
Lcd<SPI, PINS, W, H>::~Lcd() {
    heap_caps_free(buff_);
    spi_device_release_bus(spi_handle_);
}

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
    ESP_ERROR_CHECK(spi_bus_add_device(SPI, &spi_if_cfg, &spi_handle_));
    
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    gpio_set_level(PINS.rst, 0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PINS.rst, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(spi_device_acquire_bus(spi_handle_, portMAX_DELAY));

    send_command(Command::SW_RST); // software reset
    vTaskDelay(150 / portTICK_PERIOD_MS);

    send_command(Command::SLP_OUT);  // Sleep Out

    send_command(Command::DISP_ON); // display on

// Color Mode
    send_command(Command::PIX_COL); // interface pixel format
    send_data((uint8_t)ColorMode::RGB_666); // 3 bit per pixel

    // set framerate to 60 Hz
    /*send_command(Command::FRMCTR);
    constexpr uint8_t frs = 0b1111;
    constexpr uint8_t diva = 0b00;
    constexpr uint8_t rtna = 0b10001;
    send_data((frs << 4) | diva);
    send_data(rtna);*/

    send_command(Command::MADCTL);
    send_data(0b00100000);

    draw_rect(0,0, W, H, 0x00, 0x00, 0x00); // Fill display with black
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_command(Command cmd) {
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_data[0] = (uint8_t)cmd;             //The data is the cmd itself
    t.user = (void*)0;              //D/C needs to be set to 0
    t.flags = SPI_TRANS_USE_TXDATA;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle_, &t));

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
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle_, &t)); //Transmit!
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_data(uint8_t data) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.user = (void*)1;              //D/C needs to be set to 1
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle_, &t)); //Transmit!
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::send_addr_pair(uint16_t a1, uint16_t a2) {
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.user = (void*)1;              //D/C needs to be set to 1
    t.flags = SPI_TRANS_USE_TXDATA;
    t.length = 8 * (sizeof(a1) + sizeof(a2));
    t.tx_data[0] = a1 >> 8;
    t.tx_data[1] = a1 & 0xFF;
    t.tx_data[2] = a2 >> 8;
    t.tx_data[3] = a2 & 0xFF;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_handle_, &t)); //Transmit!
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::set_area(uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y) {
    // Display is originally 320x480. Driver exchanges these to make it 480x320
    send_command(Command::CA_SET);
    send_addr_pair(from_x, to_x-1);

	send_command(Command::PA_SET);
    send_addr_pair(from_y, to_y-1);
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b) {
    uint16_t max_x = (x + w <= W) ? x + w : W;
    uint16_t max_y = (y + h <= H) ? y + h : H;
    set_area(x,y,(max_x), (max_y));
    send_command(Command::MEM_WR);
    auto len = (max_x - x)*(max_y - y)*3;
    int j = 0;
    int ctr = 0;
    while (j < len) {
        for (int i = 0; i < buff_len_ && j < len; i+=3, j+=3) {
            buff_[i]   = b;
            buff_[i+1] = g;
            buff_[i+2] = r;
            ctr+=3;
        }
        if (j < len || j % buff_len_ == 0) {
            send_data(buff_, buff_len_);
        }
        else {
            send_data(buff_, (len % buff_len_));
        }
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_line(uint16_t from_x, uint16_t from_y, uint16_t to_x, uint16_t to_y, uint8_t r, uint8_t g, uint8_t b) {
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
    for (int i = 0; i < font_size; ++i) {
        for (int j = 0; j < font_size; ++j) {
            if ((bmap[i] >> j) & 1) {
                draw_rect(x + j, y + i,1,1,r,g,b);
            }
        }
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_text(const std::string& str, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
    int i = 0;
    while (str[i] != '\0') {
        draw_char(str[i], x + font_size*i, y, r, g, b);
        ++i;
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    int max_x = (x + w <= W) ? x + w : W;
    int max_y = (y + h <= H) ? y + h : H;
    set_area(x, y, max_x, max_y);
    send_command(Command::MEM_WR);
    int byte_ctr = 0;
    for (int i = x; i < max_x; ++i) {
        send_data(buff + byte_ctr, (max_y - y)*3);
        byte_ctr += (max_y - y)*3;
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_grayscale(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) { 
    int max_x = (x + w <= W) ? x + w : W;
    int max_y = (y + h <= H) ? y + h : H;
    set_area(x, y, max_x, max_y);
    send_command(Command::MEM_WR);
    int j = 0;
    for (int l = 0; l < max_y; ++l) {
        for (int c = 0; c < max_x; ++c) {
            int ix = c + (l * w);
            auto r = buff[ix];
            auto g = buff[ix];
            auto b = buff[ix];
            buff_[j++] = b; 
            buff_[j++] = g;
            buff_[j++] = r;
            if (j >= buff_len_) {
                j = 0;
                send_data(buff_, buff_len_);
            }
        }
    }
    if (j > 0) {
        send_data(buff_, j);
    }
}

template <spi_host_device_t SPI, LcdPins PINS, uint16_t W, uint16_t H>
void Lcd<SPI, PINS, W, H>::draw_565buff(const uint8_t* buff, uint16_t x, uint16_t y, uint16_t w, uint16_t h) { 
    int max_x = (x + w <= W) ? x + w : W;
    int max_y = (y + h <= H) ? y + h : H;
    set_area(x, y, max_x, max_y);
    send_command(Command::MEM_WR);
    int j = 0;
    for (int l = 0; l < max_y; ++l) {
        for (int c = 0; c < max_x; ++c) {
            int i = ((c) + (l * w)) * 2;
            auto r = (buff[i] & 0xf8);
            auto g = ((buff[i] & 0x07) << (3+2)) | ((buff[i+1] & 0xe0) >> 3);
            auto b = (buff[i+1] & 0x1f) << 3;
            buff_[j++] = b; 
            buff_[j++] = g;
            buff_[j++] = r;
            if (j >= buff_len_) {
                j = 0;
                send_data(buff_, buff_len_);
            }
        }
    }
    if (j > 0) {
        send_data(buff_, j);
    }
}

}
#endif