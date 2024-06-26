/**
 * Development temporarily discontinued due to hardware failure
 * 
 * Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf 
*/

#include "one_wire.hpp"
#include <driver/gpio.h>
#include <esp_attr.h>
#include <esp_timer.h>

using namespace OneWireDriver;

constexpr gpio_num_t pin = GPIO_NUM_15;

void IRAM_ATTR OneWireDriver::timer_callback(void* args) {
    gpio_set_level(pin, 1);
    //gpio_set_direction(pin, gpio_mode_t::GPIO_MODE_INPUT);
    printf("Hello\n");
};

/**
 * Temp Conv Time:          t_conv      max 93.75 ms (9-bit) | 187.5 ms (10-bit) | 375 ms (11-bit) | 750 ms (12-bit)
 * Tim to strong pullup:    t_spon      max 10 us
 * Time slot:               t_slot      60-120 us
 * Recovery time:           t_rec       min 1 us
 * Wr 0 Low Time:           t_low0      60-120 us
 * Wr 1 Low Time:           t_low1      1-15 us
 * Rd Data Valid:           t_rdv       max 15 us
 * Rst Time H:              t_rsth      min 480 us
 * Rst Time L:              t_rstl      min 480 us
 * Presence detect H:       t_pdhigh    15-60 us
 * Presence detect L:       t_pdlow     60-240 us
*/

// Write 0 Low Time (min) [in us]
constexpr int wr_0_l = 60;

constexpr int wr_1_l = 1;

constexpr int res_tim_h = 480;
constexpr int res_tim_l = 480;



void OneWire::init() {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, gpio_mode_t::GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(pin, gpio_pull_mode_t::GPIO_PULLUP_ONLY);
    esp_timer_handle_t tim;
    esp_timer_create_args_t t_args;
    t_args.arg = &one;
    t_args.callback = timer_callback;
    t_args.dispatch_method = esp_timer_dispatch_t::ESP_TIMER_TASK;
    t_args.name = "OW timer";
    t_args.skip_unhandled_events = true;
    ESP_ERROR_CHECK(esp_timer_create(&t_args,&tim));
    gpio_set_level(pin, 0);
    esp_timer_start_once(tim, 480);
    
}

void OneWire::send_rst() {
    alarm_cfg.alarm_count = res_tim_l;
    ESP_ERROR_CHECK(gpio_set_level(pin, 0));
    // low for t_rstl

    // wait for t_rstH
}

//void OneWire::send_1() {
//    // low for t_low1 
//}
//
//void OneWire::send_0() {
//    // low for t_low0
//}