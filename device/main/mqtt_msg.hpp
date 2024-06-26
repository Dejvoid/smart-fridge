#ifndef MQTT_MSG_HPP_
#define MQTT_MSG_HPP_

#include <string>

constexpr std::string_view NOTIF_TOPIC = "notifications";
constexpr std::string_view NOTIF_TOPIC_ALL = "notifications/#";

constexpr std::string_view THERM_TOPIC = "temperature";
constexpr std::string_view HUM_TOPIC = "humidity";

constexpr std::string_view PRODUCT_TOPIC = "product";
constexpr std::string_view PRODUCT_TOPIC_ALL = "product/#";
constexpr std::string_view PRODUCT_ADD = "product/add";
constexpr std::string_view PRODUCT_RM = "product/rm";

struct MqttMessage {
    std::string topic = "";
    std::string data = "";
};

#endif