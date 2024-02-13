#include "commander.hpp"

#include <freertos/task.h>

using namespace ConsoleCommander;

Commander::Commander(LcdDriver::LcdBase* lcd, InetComm::Connection* inet, CameraDriver::Camera* cam) : lcd_(lcd), inet_(inet), cam_(cam) {
    notif_q = xQueueCreate(max_notif_cnt, sizeof(char*));
    inet_->notif_q = &notif_q;
}

void Commander::notify(const char* notif) {
    uint8_t color[3] = {0x00, 0x00, 0x00};
    switch (notif[0]) {
        case '1': // low priority = GREEN
            color[1] = 0xff;
            break;
        case '2': // mid priority = YELLOW
            color[0] = 0xff;
            color[1] = 0xff;
            break;
        case '3': // high priority = RED
            color[0] = 0xff;
            break;
        default:
            color[0] = 0xff;
            color[1] = 0xff;
            color[2] = 0xff;
            break;
    }
    lcd_->draw_rect(0, lcd_->get_h() - LcdDriver::font_size, max_notif_len * LcdDriver::font_size, LcdDriver::font_size, 0x00,0x00,0x00);
    lcd_->draw_text(notif, 0, lcd_->get_h() - LcdDriver::font_size, color[0], color[1], color[2]);
}

void Commander::loop() {
    char* notification;
    if (notif_q != NULL && xQueueReceive(notif_q, &notification, 0) == pdTRUE) {
        notify(notification);
    }

    handle_input();
    if (scan_on) {
        auto fb = cam_->get_frame();
        if (fb == NULL) {
            printf("Error getting framebuffer\n");
        }
        else {
            lcd_->draw_grayscale(fb->buf, 0, 0, fb->width, fb->height);
            //lcd.draw_565buff(fb->buf, 0, 0, fb->width, fb->height);
            esp_code_scanner_symbol_t scan;
            if (cam_->scan_code(scan)) {
                ESP_LOGI("Camera scan", "Decoded %s symbol \"%s\"\n", scan.type_name, scan.data);
                std::string msg = "Scan: ";
                msg += scan.data;
                inet_->send_msg(std::string(scan.data));
                scan_on = false;
            }
        }
        cam_->ret_frame();
    }
}

void Commander::therm_update(float temp, float hum) {
    temp_ = temp;
    hum_ = hum;
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

void Commander::handle_cmd(const std::string& cmd) {
    if (cmd == "send") {
        inet_->send_msg("Temp: "+std::to_string(temp_)+"; Hum: "+std::to_string(hum_)+"");
    }
    else if (cmd == "start scan") {
        scan_on = true;
    }
    else if (cmd == "stop scan") {
        scan_on = false;
    }
    else {
        printf("Unknown command!\n");
    }
}

void Commander::handle_input() {
    int c = getchar();
    if (c != EOF) {
        printf("%c",c);
        if (c != '\n')
            line_ += c;
        else {
            handle_cmd(line_);
            line_.clear();
        }
    }
}