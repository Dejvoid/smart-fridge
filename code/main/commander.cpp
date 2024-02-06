#include "commander.hpp"
#include <string>
#include <iostream>

using namespace ConsoleCommander;

Commander::Commander(LcdDriver::LcdBase* lcd) : lcd_(lcd) {

}

void Commander::loop() {
    int c = std::getchar();
    if (c != EOF) {
        if (c != '\n')
            line_ += c;
        else {
            lcd_->draw_rect(300, 200, line_.length()*8, 8, 0x00, 0x00, 0x00);
            lcd_->draw_text(line_, 300, 200, 0x00, 0xff, 0x00);
            std::cout << line_ << std::endl;
            line_.clear();
        }
    }
}