#include "mqtt_comm.hpp"
#include <cstring>

MqttComm::MqttComm(const std::string& uri) {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = uri.c_str();
    mqtt_cfg.network.timeout_ms = 1000;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, esp_mqtt_event_id_t::MQTT_EVENT_ANY, mqtt_event_handler, this);
};

 void MqttComm::connect() {
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
};
void MqttComm::subscribe(const std::string& topic, int qos) {
    //const esp_mqtt_topic_t topic = {topic.c_str(), qos};
    esp_mqtt_client_subscribe_single(client, topic.c_str(), qos);
}
void MqttComm::publish(const MqttMessage& msg) {
    printf("Publishing |%s| to topic |%s|", msg.data.c_str(), msg.topic.c_str());
    esp_mqtt_client_publish(client, msg.topic.c_str(), msg.data.c_str(), 0, 0, 0);
}
void MqttComm::publish(const std::string& topic, const std::string& msg) {
    printf("Publishing |%s| to topic |%s|", msg.c_str(), topic.c_str());
    esp_mqtt_client_publish(client, topic.c_str(), msg.c_str(),0,0,0);
}
void MqttComm::terminate() {

}
void MqttComm::process_msg(const esp_mqtt_event_handle_t& e) {
    int i = 0;
    bool is_notif = e->topic_len >= NOTIF_TOPIC.length();
    // Compare if topic is notification
    while (i < NOTIF_TOPIC.length() && is_notif) {
        is_notif &= e->topic[i] == NOTIF_TOPIC[i];
        ++i;
    }
    if (is_notif) {
        // Copy so we keep the data that we want to interpret later, the esp-mqtt unfortunately doesn't provide other way
        std::memcpy(recv.data, e->data, std::min(max_notif_len, e->data_len));
        recv.data[std::min(max_notif_len - 1, e->data_len)] = '\0';
        recv.priority = 0;
        if (e->topic_len >= NOTIF_TOPIC.length() + 2) {
            recv.priority = e->topic[NOTIF_TOPIC.length() + 1];
        }
        xQueueSendToBack(*notif_q, &recv_ptr, portMAX_DELAY);
    }
}