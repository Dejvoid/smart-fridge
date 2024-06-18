#ifndef MQTT_COMM_HPP_
#define MQTT_COMM_HPP_

#include <string>
#include <mqtt_client.h>

#include "mqtt_msg.hpp"
#include "commander.hpp"

namespace ConsoleCommander {
    class Commander;
}

// Maximal length of a single notification.
constexpr int max_notif_len = 32;

struct Notification {
    char data[max_notif_len];
    int priority;
};

class MqttComm {
    friend ConsoleCommander::Commander;
    esp_mqtt_client_handle_t client;
    QueueHandle_t* notif_q = NULL;
    Notification recv;
    Notification* recv_ptr = &recv;
public:
    MqttComm(const std::string& uri);
    void connect();
    void subscribe(const std::string& topic, int qos = 0);
    void publish(const MqttMessage& msg);
    void publish(const std::string& topic, const std::string& msg);
    void terminate();
    void process_msg(const esp_mqtt_event_handle_t& e);
};

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    MqttComm* handler_class = (MqttComm*)handler_args;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        printf("mqtt connected\n");
        handler_class->subscribe(NOTIF_TOPIC_ALL.data());
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);

        //msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);

        //msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);

        //msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        break;
    case MQTT_EVENT_DISCONNECTED:
        printf("Disconnected\n");
        //ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        printf("subscribed\n");
        //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        printf("unsubscribed");
        //ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        printf("published\n");
        //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA: 
        printf("got data\n"); {
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        handler_class->process_msg(event);
        } break;
    case MQTT_EVENT_ERROR:
        printf("error in  MQTT\n");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {

        }
        break;
    default:
        //ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

#endif
