#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include "lcd.hpp"
#include "temp_i2c.hpp"
#include "inet_comm.hpp"
#include "camera.hpp"

#include <string>

namespace InetComm {
    class Connection;
}

namespace ConsoleCommander {

constexpr int max_notif_cnt = 10;
constexpr int max_notif_len = 32;

/// @brief Class responsible for input/output handling. Connects individual components
class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
    I2cTempDriver::Temperature* therm_;
    InetComm::Connection* inet_;
    CameraDriver::Camera* cam_;
    float temp_;
    float hum_;
    bool scan_on = false;
    QueueHandle_t notif_q;
    void handle_cmd(const std::string& cmd);
    void handle_input();
    void notify(const char* notification);
public:
    /// @brief 
    /// @param lcd - Pointer to LCD component 
    /// @param inet - Pointer to connection component
    /// @param cam - Pointer to camera component
    Commander(LcdDriver::LcdBase* lcd, InetComm::Connection* inet, CameraDriver::Camera* cam);
    void loop();
    void therm_update(float temp, float hum);
};

}

#endif