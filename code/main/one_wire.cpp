#include "one_wire.hpp"

bool timer_callback(gptimer_t*, const gptimer_alarm_event_data_t*, void*) {
    return false;
}

void OneWire::init() {
    tim_cfg.clk_src = (gptimer_clock_source_t)TIMER_SRC_CLK_DEFAULT;
    tim_cfg.direction = GPTIMER_COUNT_UP;
    tim_cfg.resolution_hz = 1'000'000;
    tim_cfg.intr_priority = 0;

    alarm_cfg.reload_count = 0;
    alarm_cfg.alarm_count = 15;
    alarm_cfg.flags.auto_reload_on_alarm = true;

    tim_callback.on_alarm = timer_callback;

    ESP_ERROR_CHECK(gptimer_new_timer(&tim_cfg, &tim));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(tim, &alarm_cfg));
    ESP_ERROR_CHECK(gptimer_enable(tim));
}