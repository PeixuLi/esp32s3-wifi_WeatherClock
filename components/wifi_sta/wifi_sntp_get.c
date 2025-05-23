#include <time.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "wifi_sntp_get.h"
#include "http_get_weather.h"
#include "ui_home.h"
#include "app_message.h"

static const char *TAG = "wifi_sntp_get.c";

bool sntp_initialized = false;
TaskHandle_t lv_start_progress_handle = NULL;
TaskHandle_t Get_weather_handle = NULL;
TaskHandle_t sntp_get_time_handle = NULL;

void sntp_sync_callback(struct timeval *tv)
{
    if (lv_start_progress_handle == NULL)
    {
        xTaskCreatePinnedToCore(lv_start_progress, "lv_start_progress", 4096, NULL, 4,
                                &lv_start_progress_handle, 1);
        ESP_LOGI(TAG, "lv_start_progress_handle任务已创建");
    }
    if (Get_weather_handle == NULL)
    {
        xTaskCreatePinnedToCore(Get_weather_task, "Get_weather_task", 4096, NULL, 4,
                                &Get_weather_handle, 1); // 统一绑定到核心0
        ESP_LOGI(TAG, "Get_weather_task任务已创建");
    }
}

void Get_local_time(struct tm *localTime)
{
    time_t now;

    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1); //"CST-8" 表示上海时间（UTC+8，无夏令时）
    tzset();

    time(&now);                   // 获取 UTC 时间戳，time() 函数的功能是获取当前的系统时间，通常是自 1970 年 1 月 1 日（UTC）以来的秒数。
    localtime_r(&now, localTime); // 转换为本地时间结构体
}

// SNTP 时间同步，SNTP 仅提供 UTC 时间，必须设置时区以获取本地时间
void sntp_get_time_task(void *parameter)
{
    if (sntp_initialized)
    {
        ESP_LOGW(TAG, "SNTP已初始化，跳过");
        sntp_get_time_handle = NULL;
        vTaskSuspend(NULL); // 挂起任务
    }

    esp_sntp_config_t config = {
        .servers = {"pool.ntp.org", "ntp1.aliyun.com", "cn.pool.ntp.org"},
        .num_of_servers = 3,
        .smooth_sync = false,
        .server_from_dhcp = false,
        .wait_for_sync = true,
        .start = true,
        .sync_cb = sntp_sync_callback,
        .renew_servers_after_new_IP = false,
        .ip_event_to_renew = IP_EVENT_STA_GOT_IP,
        .index_of_first_server = 0,
    };
    esp_netif_sntp_init(&config);
    sntp_initialized = true;

    int retry = 0;
    int retry_count = 10;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry <= retry_count)
    {
        ESP_LOGI(TAG, "等待系统时间... (%d/%d)", retry, retry_count);
        //lv_show_hint("wait sntp time...", 1000);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    if (retry > retry_count)
    {
        ESP_LOGE(TAG, "SNTP同步时间失败");
        esp_netif_sntp_deinit();
        sntp_initialized = false;
        sntp_get_time_handle = NULL;
        vTaskSuspend(NULL);
    }
    else
    {
        ESP_LOGI(TAG, "SNTP同步成功");
        sntp_get_time_handle = NULL;
        vTaskSuspend(NULL);
        //lv_show_hint("wait sntp time successed!", 1500);
    }
}