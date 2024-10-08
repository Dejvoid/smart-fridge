#include "wifi.hpp"

using namespace WifiDriver;

void Wifi::init(const std::string_view ssid, const std::string_view password) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();



    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_cfg = { 
        .sta =  {
            .ssid =         "",
            .password       = "",
            .scan_method    = WIFI_FAST_SCAN,
            .bssid_set      = 0,
            .bssid          = {}, 
            .channel        = 0,
            .listen_interval= {},
            .sort_method    = {},
            .threshold      = {
                .rssi       = {},
                .authmode   = WIFI_AUTH_WPA2_PSK},
            .pmf_cfg        = {
                .capable    = false,
                .required   = false},
            .rm_enabled      = {},
            .btm_enabled     = {},
            .mbo_enabled     = {}, // For IDF 4.4 and higher
            .ft_enabled      = {},
            .owe_enabled     = {},
            .reserved        = {},
            .sae_pwe_h2e     = {},
            
        }
    };
    
    // This is unfortunate but there's no other way
    std::copy(ssid.begin(), ssid.end(), wifi_cfg.sta.ssid);
    std::copy(password.begin(), password.end(), wifi_cfg.sta.password); 
    printf("Target SSID: %s\n", wifi_cfg.sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    printf("wifi_init_sta finished.\n");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        printf( "connected to ap SSID:%s ",
                 ssid.data());
    } else if (bits & WIFI_FAIL_BIT) {
        printf("Failed to connect to SSID:%s",
                 ssid.data());
    } else {
        printf("UNEXPECTED EVENT");
    }
}