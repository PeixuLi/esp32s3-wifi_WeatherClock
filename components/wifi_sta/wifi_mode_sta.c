#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "lwip/inet.h"
#include "wifi_mode_sta.h"
#include "wifi_sntp_get.h"
#include "ui_home.h"
#include "http_get_weather.h"

static const char *TAG = "wifi_mode_sta.c";

/* 根据热点来设置 */
#define WIFI_STA_SSID "xu-PC"      // 要连接的wifi或热点 的名称
#define WIFI_STA_PASS "12344321.." // 要连接的wifi或热点 的密码
#define STA_StaticIP "192.168.137.4"
#define STA_StaticMask "255.255.255.0"
#define STA_StaticGateway "192.168.137.1"

uint8_t connect_count = 0;
extern void Get_weather_task(void *pvParameters);


static void wifi_event_callback(void *event_handler_arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
        ESP_LOGI("wifi_event_callback", "wifi connecting...");
    }
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI("wifi_event_callback", "wifi connected!");
    }
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        connect_count++;
        if (connect_count <= 5)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP_ERROR_CHECK(esp_wifi_connect());
            ESP_LOGI("wifi_event_callback", "wifi connecting...");
        }
        else
        {
            ESP_LOGW("wifi_event_callback", "wifi connected failed!");
        }
    }
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *info = (ip_event_got_ip_t *)event_data;
        connect_count = 0;
        /*
            取地址（&）是因为 IP2STR 宏需要一个指向 ip4_addr 结构体的指针，而非结构体本身。
            通过传递指针，宏可以正确解析出 IP 地址的四个字节，并格式化为字符串
        */
        ESP_LOGI("wifi_event_callback", "STA IP:" IPSTR "\n", IP2STR(&info->ip_info.ip));

        int err = sntp_get_time();
        if(err == 0)
        {
            xTaskCreatePinnedToCore(lv_reflash_data, "lv_reflash_data", 4096, NULL, 6, NULL, 1);
            xTaskCreate(Get_weather_task, "Get_weather_task", 4096, NULL, 4, NULL);
        }  
    }
}

void wifi_creat_sta(void)
{
    ESP_LOGI(TAG, "--wifi driver Init...");
    // 初始化网卡的底层配置
    // 创建一个 LwIP 核心任务，并初始化 LwIP 相关工作
    ESP_ERROR_CHECK(esp_netif_init());

    /* ************ 配置成静态IP *********** */
    // 以默认方式创建一个sta类型的网卡
    esp_netif_t *sta_netif_handle = esp_netif_create_default_wifi_sta();
#if 1
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(sta_netif_handle)); // 停止该网卡的DHCP客户端

    esp_netif_ip_info_t STA_Info = {
        .ip.addr = inet_addr(STA_StaticIP),
        .netmask.addr = inet_addr(STA_StaticMask),
        .gw.addr = inet_addr(STA_StaticGateway)};
    ESP_ERROR_CHECK(esp_netif_set_ip_info(sta_netif_handle, &STA_Info));

    // 配置 DNS 服务器（例如 Google DNS）
    esp_netif_dns_info_t dns_info;
    dns_info.ip.u_addr.ip4.addr = inet_addr("114.114.114.114"); // 主 DNS
    dns_info.ip.type = IPADDR_TYPE_V4;
    ESP_ERROR_CHECK(esp_netif_set_dns_info(sta_netif_handle, ESP_NETIF_DNS_MAIN, &dns_info));

    dns_info.ip.u_addr.ip4.addr = inet_addr("223.5.5.5"); // 备用 DNS
    ESP_ERROR_CHECK(esp_netif_set_dns_info(sta_netif_handle, ESP_NETIF_DNS_BACKUP, &dns_info));

/* ************ 配置成静态IP end *********** */
#else
    ESP_ERROR_CHECK(esp_netif_dhcpc_start(sta_netif_handle));
#endif
    // 初始化wifi底层配置
    //  WIFI_INIT_CONFIG_DEFAULT 是一个默认配置的宏
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* 注册事件 */
    /*调用函数esp_wifi_start();成功后触发该事件*/
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_START, wifi_event_callback, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, wifi_event_callback, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_event_callback, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_callback, NULL, NULL));

    // 配置wifi(STA mode)
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_STA_SSID,
            .password = WIFI_STA_PASS,
        }};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_ps(WIFI_PS_NONE); // 不使用省电模式
    ESP_LOGI(TAG, "--wifi config finished!");
}
