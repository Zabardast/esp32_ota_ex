

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include <string.h>

static const char *STA_WIFI = "wifi STA";

#include "keys.c"
#include "tcp_client.c"

// OTA
#define HASH_LEN 32

#include "esp_https_ota.h"  // ESP_BOOTLOADER_OFFSET

#include "esp_ota_ops.h"

static const char *TAG = "OTA";

// "http://yourserver.com/firmware.bin" in the future use a dns or a static ip on publishing machine
#define CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL "http://10.0.0.218:8000/ota.bin"

//

void simple_ota_example_task();

void station_got_ip(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_id == IP_EVENT_STA_GOT_IP) {
        
        ip_event_got_ip_t* ip_event_data = (ip_event_got_ip_t*) event_data;
        
        ESP_LOGI(STA_WIFI, "new ip : "IPSTR , IP2STR(&ip_event_data->ip_info.ip));
        ESP_LOGI(STA_WIFI, "Start traffic!");
        // xTaskCreatePinnedToCore(tcp_send, "send_tcp_hello", 4096, NULL, 1, NULL, 0);
        xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
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

static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

void get_sha256_of_partitions()
{
    uint8_t sha_256[HASH_LEN] = { 0 };
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            break;
        }
    return ESP_OK;
}

void simple_ota_example_task()
{
    ESP_LOGI(TAG, "Starting OTA example task");
    esp_http_client_config_t config = {
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
        // .cert_pem = (char *)server_cert_pem_start,
        .cert_pem = NULL, // NULL because using http not https
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed: %i", ret);
    }

    // remove in the funture
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // setup
    vTaskDelay(1000);

    init_nvs();

    // check partition for OTA
    get_sha256_of_partitions();

    // event loop
    esp_err_t crel_err = esp_event_loop_create_default();
    if(crel_err != ESP_OK)
    {
        printf("failed to create event loop");
    }

    // setup wifi
    esp_netif_init();

    // esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_sta();

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

    // set power saving to none for faster speeds (OTA speed also)
    esp_wifi_set_ps(WIFI_PS_NONE);

    // xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
}