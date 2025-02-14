#include "keys.c"

// #include <stdio.h>

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include <string.h>


static const char *AP_WIFI = "wifi AP";

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

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    

    esp_wifi_set_mode(WIFI_MODE_AP);

    //set config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config_ap = {
        .ap = {
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .channel = 1,
            .max_connection = 5,
            .authmode = WIFI_AUTH_WPA2_WPA3_PSK
        },
    };
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap);

    // start wifi interfaces
    esp_err_t ap_result = esp_wifi_start();
    if (ap_result != ESP_OK)
    {
        ESP_LOGE(AP_WIFI, "WiFi AP start failed: %s", esp_err_to_name(ap_result));
    }else{
        ESP_LOGI(AP_WIFI, "WiFi AP started: %s", esp_err_to_name(ap_result));
    }
}