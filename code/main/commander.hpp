#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include <array>
#include <string>
#include "lcd.hpp"

namespace ConsoleCommander {

class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
public:
    Commander(LcdDriver::LcdBase* lcd_);
    void loop();
};

}

#endif