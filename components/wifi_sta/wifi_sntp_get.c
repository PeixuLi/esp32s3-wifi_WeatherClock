#include <time.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "wifi_sntp_get.h"

static const char *TAG = "wifi_sntp_get.c";

void Get_local_time(struct tm *localTime)
{
    time_t now;

    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1);//"CST-8" 表示上海时间（UTC+8，无夏令时）
    tzset();

    time(&now);//获取 UTC 时间戳，time() 函数的功能是获取当前的系统时间，通常是自 1970 年 1 月 1 日（UTC）以来的秒数。
    localtime_r(&now, localTime);//转换为本地时间结构体
}

//SNTP 时间同步，SNTP 仅提供 UTC 时间，必须设置时区以获取本地时间
int sntp_get_time(void)
{
    //初始化sntp
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    esp_sntp_setservername(1, "ntp1.aliyun.com");
    esp_sntp_setservername(2, "cn.pool.ntp.org");

    //等待系统时间设置
    int retry = 0;
    const int retry_count = 10;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) 
    {
        ESP_LOGI(TAG, "等待系统时间... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if (retry == retry_count) 
    {
        ESP_LOGE(TAG, "无法从 SNTP 服务器获取时间");
        return -1;
    }

    ESP_LOGI(TAG, "时间已设置");
    
    return 0;
}