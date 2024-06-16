#include "mqtt_comm.hpp"
#include <cstring>

MqttComm::MqttComm(const std::string& uri) {
        esp_mqtt_client_config_t mqtt_cfg;
        // Zero-out the struct to be sure
        std::memset(&mqtt_cfg, 0, sizeof(esp_mqtt_client_config_t));
        mqtt_cfg.broker.address.uri = uri.c_str();

        client = esp_mqtt_client_init(&mqtt_cfg);
        esp_mqtt_client_register_event(client, esp_mqtt_event_id_t::MQTT_EVENT_ANY, mqtt_event_handler, this);
};

 void MqttComm::connect() {
        esp_mqtt_client_start(client);
};
void MqttComm::subscribe(const std::string& topic, int qos) {
        //const esp_mqtt_topic_t topic = {topic.c_str(), qos};
        esp_mqtt_client_subscribe_single(client, topic.c_str(), qos);
}
void MqttComm::publish(const MqttMessage& msg) {
        esp_mqtt_client_publish(client, msg.topic.c_str(), msg.data.c_str(), 0, 0, 0);
}
void MqttComm::publish(const std::string& topic, const std::string& msg) {
        esp_mqtt_client_publish(client, topic.c_str(), msg.c_str(),0,0,0);
}
void MqttComm::terminate() {

}