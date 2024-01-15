#ifndef ONE_WIRE_HPP_
#define ONE_WIRE_HPP_

#include <driver/gptimer.h>

bool timer_callback(gptimer_t*, const gptimer_alarm_event_data_t*, void*);

class OneWire {
    gptimer_handle_t tim;
    gptimer_config_t tim_cfg;
    gptimer_alarm_config_t alarm_cfg;
    gptimer_event_callbacks_t tim_callback;
public:
    void init();
};

#endif