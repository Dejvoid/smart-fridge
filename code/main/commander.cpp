#include "commander.hpp"

#include <iostream>

using namespace ConsoleCommander;

Commander::Commander(LcdDriver::LcdBase* lcd) : lcd_(lcd) {}

void Commander::loop() {
    int c = std::getchar();
    if (c != EOF) {
        if (c != '\n')
            line_ += c;
        else {
            //lcd_->draw_rect(temp_pos_x, temp_pos_y, temp_end_x, temp_end_y, 0x00, 0x00, 0x00);
            //lcd_->draw_text(line_, temp_pos_x, temp_pos_y, 0x00, 0xff, 0x00);
            std::cout << line_ << std::endl;
            line_.clear();
        }
    }
}

void Commander::therm_update(float temp, float hum) {
    uint16_t temp_pos_x = lcd_->get_w() - 15*LcdDriver::font_size;
    uint16_t temp_pos_y = 0;
    uint16_t temp_end_x = lcd_->get_w();
    uint16_t temp_end_y = LcdDriver::font_size;
    uint16_t hum_pos_x = temp_pos_x;
    uint16_t hum_pos_y = temp_end_y;
    uint16_t hum_end_x = lcd_->get_w();
    uint16_t hum_end_y = hum_pos_y + LcdDriver::font_size;
    lcd_->draw_rect(temp_pos_x, temp_pos_y, temp_end_x, temp_end_y, 0x00, 0x00, 0x00);
    std::string temp_str = "Temp: " + std::to_string(temp) +"°C";
    std::string hum_str = "Hum: " + std::to_string(hum) + "%";
    lcd_->draw_text(temp_str, temp_pos_x, temp_pos_y, 0x00, 0xff, 0x00);
    lcd_->draw_rect(hum_pos_x, hum_pos_y, hum_end_x, hum_end_y, 0x00, 0x00, 0x00);
    lcd_->draw_text(hum_str, hum_pos_x, hum_pos_y, 0xff, 0xff, 0xff);
}