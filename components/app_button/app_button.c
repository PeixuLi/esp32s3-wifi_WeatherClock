#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "app_button.h"
#include "register_button.h"

static const char *TAG = "app_button.c";

volatile bool btn1_pressed = false;
volatile bool btn2_pressed = false;
volatile bool btn3_pressed = false;

void button1_event_cb(void *arg, void *data)
{
    button_event_t event = iot_button_get_event(arg);

    //ESP_LOGI(TAG, "%s", iot_button_get_event_str(event));
    if (event == BUTTON_SINGLE_CLICK || event == BUTTON_LONG_PRESS_UP) {
        //ESP_LOGI(TAG, "\tTICKS[%"PRIu32"]", iot_button_get_ticks_time(arg));
        ESP_LOGI("button1_event_cb", "%s", iot_button_get_event_str(event));
        btn1_handle = (button_handle_t)arg; // 更新状态
        btn1_pressed = true;
    }
}

void button2_event_cb(void *arg, void *data)
{
    //iot_button_print_event((button_handle_t)arg);
    button_event_t event = iot_button_get_event(arg);

    ESP_LOGI(TAG, "%s", iot_button_get_event_str(event));
    if (event == BUTTON_SINGLE_CLICK) {
        //ESP_LOGI(TAG, "\tTICKS[%"PRIu32"]", iot_button_get_ticks_time(arg));
        btn2_handle = (button_handle_t)arg; // 更新状态
        btn2_pressed = true;
    }
    else if (event == BUTTON_DOUBLE_CLICK) {
        ESP_LOGI(TAG, "\tTICKS[%"PRIu32"]", iot_button_get_ticks_time(arg));
    }

}

void button3_event_cb(void *arg, void *data)
{
    //iot_button_print_event((button_handle_t)arg);
    button_event_t event = iot_button_get_event(arg);

    ESP_LOGI(TAG, "%s", iot_button_get_event_str(event));
    if (event == BUTTON_SINGLE_CLICK) {
        //ESP_LOGI(TAG, "\tTICKS[%"PRIu32"]", iot_button_get_ticks_time(arg));
        btn3_handle = (button_handle_t)arg; // 更新状态
        btn3_pressed = true;
    }
    else if (event == BUTTON_DOUBLE_CLICK) {
        ESP_LOGI(TAG, "\tTICKS[%"PRIu32"]", iot_button_get_ticks_time(arg));
    }

}
