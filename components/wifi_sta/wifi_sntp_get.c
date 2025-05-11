#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "wifi_sntp_get.h"

static const char *TAG = "wifi_sntp_get.c";

//SNTP 时间同步
void sntp_get_time(void)
{
    
    //初始化sntp
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    //等待系统时间设置
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "等待系统时间... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if (retry == retry_count) {
        ESP_LOGE(TAG, "无法从 SNTP 服务器获取时间");
        return;
    }
    ESP_LOGI(TAG, "时间已设置");
}