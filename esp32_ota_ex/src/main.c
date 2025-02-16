

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include <string.h>

static const char *STA_WIFI = "wifi STA";
static const char *TCP_CLIENT = "TCP client";

#include "keys.c"
#include "tcp_client.c"



void station_got_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_id == IP_EVENT_STA_GOT_IP) {
        
        ip_event_got_ip_t* ip_event_data = (ip_event_got_ip_t*) event_data;
        
        ESP_LOGI(STA_WIFI, "new ip : "IPSTR , IP2STR(&ip_event_data->ip_info.ip));
        ESP_LOGI(TCP_CLIENT, "Start traffic!");
        xTaskCreatePinnedToCore(tcp_send, "send_tcp_hello", 4096, NULL, 1, NULL, 0);
    } else {
        ESP_LOGI(STA_WIFI, "no action for this event_id");
    }
}

void init_nvs()
{
    //  Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void app_main()
{
    // setup
    vTaskDelay(1000);

    init_nvs();

    // event loop
    esp_err_t crel_err = esp_event_loop_create_default();
    if(crel_err != ESP_OK)
    {
        printf("failed to create event loop");
    }

    // setup wifi
    esp_netif_init();

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // esp_event_handler_register
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, station_got_ip , NULL);


    // config wifi
    esp_wifi_set_mode(WIFI_MODE_STA);

    // wifi settings for the Station
    wifi_config_t n_wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA3_PSK,
            .scan_method = WIFI_FAST_SCAN,
            .pmf_cfg = {
                .capable = true,
                .required = true
            },
        },
    };


    // needs STA to be initialized
    esp_wifi_set_config(ESP_IF_WIFI_STA, &n_wifi_config);
    // starts wifi from the previous settings
    esp_err_t wf_result = esp_wifi_start();

    printf("Wifi start result: %i\n", wf_result);

    // connect to wifi AP
    esp_wifi_connect();
}