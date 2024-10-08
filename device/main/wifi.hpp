/**
 * WiFi connection header
 * 
 * Takes care of WiFi connection. 
*/
#ifndef WIFI_HPP_
#define WIFI_HPP_

#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <string_view>
#include <array>

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

namespace WifiDriver {

    static EventGroupHandle_t s_wifi_event_group;
    static int s_retry_num = 0;

    /// @brief Basic handler of connection associated events.
    /// @param arg          Arguments for the handler (currently unused)
    /// @param event_base   Group of event
    /// @param event_id     Concrete event
    /// @param event_data   Event data
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

/// @brief Takes care of Wifi and overall connnection initiation and keeping.
class Wifi {
public:
    /// @brief Initialize wifi connection and IP stack
    void init(const std::string_view ssid, const std::string_view password);
};
}
#endif