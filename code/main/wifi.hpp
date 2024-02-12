#ifndef WIFI_HPP_
#define WIFI_HPP_

#include <esp_wifi.h>
#include <freertos/event_groups.h>

#define EXAMPLE_ESP_WIFI_SSID 
#define EXAMPLE_ESP_WIFI_PASS 

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

namespace WifiDriver {

    static EventGroupHandle_t s_wifi_event_group;
    static int s_retry_num = 0;

    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
            esp_wifi_connect();
        } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (s_retry_num < 20) {
                esp_wifi_connect();
                s_retry_num++;
                printf("retry to connect to the AP\n");
            } else {
                xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            }
            printf("connect to the AP fail\n");
        } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            printf("got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        }
    }

class Wifi {
public:
    void init();
    
};
}
#endif