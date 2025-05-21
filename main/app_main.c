#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_event.h"
#include "lcd_ST7789.h"
#include "wifi_mode_sta.h"
#include "app_ui.h"
#include "app_message.h"

static const char *TAG = "app_main.c";

extern esp_lcd_panel_io_handle_t   lcd_io_handle;

void lvgl_task(void *pvParameters) 
{
    create_ui();
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Peripheral initialization!");
    // 创建一个默认事件循环（线程）
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(nvs_flash_init());
    LCD_Init();
    wifi_creat_sta();

    xTaskCreatePinnedToCore(lvgl_task, "LVGL", 8192, NULL, 5, NULL, 1);
    
    while (1) 
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
