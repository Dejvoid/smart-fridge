/**
 * commander.hpp
 * This file contains definition of the ConsoleCommander class which interconnects individual components and provides them to the user
 */
#ifndef COMMANDER_HPP_
#define COMMANDER_HPP_

#include "lcd.hpp"
#include "temp_i2c.hpp"
#include "mqtt_msg.hpp"
#include "mqtt_comm.hpp"
#include "camera.hpp"

#include <string>

class MqttComm;
struct Notification;

/*namespace InetComm {
    class Connection;
}*/

namespace ConsoleCommander {

constexpr int max_notif_cnt = 10;
constexpr int frame_w = 320;
constexpr int frame_h = 240;

/// @brief Class responsible for input/output handling and interconnection individual components. Provides access to components over the commands and takes care of drawing notifications
class Commander {
    std::string line_;
    LcdDriver::LcdBase* lcd_;
    I2cTempDriver::TemperatureBase* therm_;
    I2cTempDriver::HumidityBase* hum_sensor_;
    MqttComm* mqtt_;
    CameraDriver::CameraBase* cam_;
    float temp_;
    float hum_;
    bool scan_on = false;
    MqttMessage msg_;
    QueueHandle_t notif_q;
    /// @brief Executes command string
    /// @param cmd - command to execute
    void handle_cmd(const std::string& cmd);
    /// @brief Read input from console
    void handle_input();
    /// @brief Process notification. Show it on display.
    /// @param notification - Notification text and priority
    void notify(const Notification* notif);
public:
    /// @brief Constructor taking in components of the system
    /// @param lcd - Pointer to LCD component 
    /// @param mqtt - Pointer to MQTT component
    /// @param cam - Pointer to camera component
    Commander(LcdDriver::LcdBase* lcd, MqttComm* mqtt, CameraDriver::CameraBase* cam);
    /// @brief Call this in loop to process user input, gather notifications and draw preview of the camera (if scanning is on)
    void loop();
    /// @brief Updates internal temperature and humidity values and prints them on the display. Calling this in a loop may cause showed data to flicker.
    void therm_update(float temp, float hum);
};

}

#endif