/**
 * Development temporarily discontinued due to hardware failure
*/
#ifndef ONE_WIRE_HPP_
#define ONE_WIRE_HPP_

#include <driver/gptimer.h>

namespace OneWireDriver {

void timer_callback(void* args);

class OneWire {
    bool one;
    gptimer_handle_t tim = NULL;
    gptimer_config_t tim_cfg;
    gptimer_alarm_config_t alarm_cfg;
public:
    void init();
    void send_rst();
};

};

#endif