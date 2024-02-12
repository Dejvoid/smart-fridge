#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include "lcd.hpp"

#include <string>

namespace ConsoleCommander {

class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
public:
    Commander(LcdDriver::LcdBase* lcd_);
    void loop();
    void therm_update(float temp, float hum);
};

}

#endif