#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include "lcd.hpp"
#include "temp_i2c.hpp"
#include "inet_comm.hpp"
#include "camera.hpp"

#include <string>

namespace ConsoleCommander {

class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
    I2cTempDriver::Temperature* therm_;
    InetComm::InetComm* inet_;
    CameraDriver::Camera* cam_;
    float temp_;
    float hum_;
    bool scan_on = false;
    void handle_cmd(const std::string& cmd);
    void handle_input();
public:
    Commander(LcdDriver::LcdBase* lcd, InetComm::InetComm* inet, CameraDriver::Camera* cam);
    void loop();
    void therm_update(float temp, float hum);
};

}

#endif