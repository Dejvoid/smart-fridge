#ifndef MQTT_COMM_HPP_
#define MQTT_COMM_HPP_

#include <string>
#include <mqtt_client.h>

#include "mqtt_msg.hpp"
#include "commander.hpp"

#include <esp_log.h>

namespace ConsoleCommander {
    class Commander;
}

// Maximal length of a single notification.
constexpr int max_notif_len = 20;

extern const uint8_t client_cert_pem_start[] asm("_binary_device_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_device_crt_end");
extern const uint8_t client_key_pem_start[] asm("_binary_device_key_start");
extern const uint8_t client_key_pem_end[] asm("_binary_device_key_end");
extern const uint8_t ca_cert_pem_start[] asm("_binary_ca_crt_start");
extern const uint8_t ca_cert_pem_end[] asm("_binary_ca_crt_end");

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
    bool connected = false;
public:
    MqttComm(const std::string& uri);
    void connect();
    void subscribe(const std::string& topic, int qos = 0);
    void publish(const MqttMessage& msg);
    void publish(const std::string& topic, const std::string& msg);
    void terminate();
    void process_msg(const esp_mqtt_event_handle_t& e);
    void update_connected(bool connected);
};

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE("MQTT", "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    MqttComm* handler_class = (MqttComm*)handler_args;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        printf("mqtt connected\n");
        handler_class->update_connected(true);
        handler_class->subscribe(NOTIF_TOPIC_ALL.data());
        break;
    case MQTT_EVENT_DISCONNECTED:
        printf("Disconnected\n");
        handler_class->update_connected(false);
        //ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        printf("subscribed\n");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        printf("unsubscribed");
        break;
    case MQTT_EVENT_PUBLISHED:
        printf("published\n");
        break;
    case MQTT_EVENT_DATA: 
        printf("got data\n"); {
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        handler_class->process_msg(event);
        } break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI("MQTT", "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI("MQTT", "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        //printf("Before connection mqtt\n");
    break;
    default:
        ESP_LOGI("MQTT", "Other event id:%d", event->event_id);
        break;
    }
}

#endif
