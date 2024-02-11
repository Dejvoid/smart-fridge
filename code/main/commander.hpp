#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include "lcd.hpp"

#include <string>
#include <vector>

namespace ConsoleCommander {

class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
    std::vector<std::string> notifications_;
public:
    Commander(LcdDriver::LcdBase* lcd_);
    void loop();
    void therm_update(float temp, float hum);
};

}

#endif