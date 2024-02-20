/**
 * commander.hpp
 * This file contains definition of the ConsoleCommander class which interconnects individual components and provides them to the user
 */
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
// Maximal length of a signle notification.
constexpr int max_notif_len = 32;

/// @brief Class responsible for input/output handling and interconnection individual components. Provides access to components over the commands and takes care of drawing notifications
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
    /// @brief Executes command string
    /// @param cmd - command to execute
    void handle_cmd(const std::string& cmd);
    /// @brief Read input from console
    void handle_input();
    /// @brief Process notification. Show it on display.
    /// @param notification - Notification text
    void notify(const char* notification);
public:
    /// @brief Constructor taking in components of the system
    /// @param lcd - Pointer to LCD component 
    /// @param inet - Pointer to connection component
    /// @param cam - Pointer to camera component
    Commander(LcdDriver::LcdBase* lcd, InetComm::Connection* inet, CameraDriver::Camera* cam);
    /// @brief Call this in loop to process user input, gather notifications and draw preview of the camera (if scanning is on)
    void loop();
    /// @brief Updates internal temperature and humidity values and prints them on the display. Calling this in a loop may cause showed data to flicker.
    void therm_update(float temp, float hum);
};

}

#endif