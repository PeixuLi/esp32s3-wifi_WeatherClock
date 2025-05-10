#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "register_button.h"

static const char *TAG = "register_button.c";

#define button_num1  GPIO_NUM_14
#define button_num2  GPIO_NUM_13
#define button_num3  GPIO_NUM_12

button_handle_t btn1_handle, btn2_handle, btn3_handle;

void Register_Button(uint32_t button_num, button_handle_t* btn, button_cb_t button_event_cb)
{
    ESP_LOGI(TAG, ">>>>>Register_Button...");
    button_config_t btn_cfg = {0};
    button_gpio_config_t gpio_cfg = {
        .gpio_num = button_num,
        .active_level = 0,
    };

    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, btn);
    assert(ret == ESP_OK);

    ret = iot_button_register_cb(*btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, NULL);

    ESP_ERROR_CHECK(ret);
}

void Register_Button_Event(button_handle_t* btn, button_event_t event, button_cb_t button_event_cb)
{
    esp_err_t ret = iot_button_register_cb(*btn, event, NULL, button_event_cb, NULL);
    ESP_ERROR_CHECK(ret);
}

void Button_Init(void)
{

    Register_Button(button_num1, &btn1_handle, button1_event_cb);
    Register_Button_Event(&btn1_handle, BUTTON_SINGLE_CLICK, button1_event_cb);
    Register_Button_Event(&btn1_handle, BUTTON_DOUBLE_CLICK, button1_event_cb);
    Register_Button_Event(&btn1_handle, BUTTON_LONG_PRESS_UP, button1_event_cb);

    Register_Button(button_num2, &btn2_handle, button2_event_cb);
    Register_Button_Event(&btn2_handle, BUTTON_SINGLE_CLICK, button2_event_cb);
    Register_Button_Event(&btn2_handle, BUTTON_DOUBLE_CLICK, button2_event_cb);

    Register_Button(button_num3, &btn3_handle, button3_event_cb);
    Register_Button_Event(&btn3_handle, BUTTON_SINGLE_CLICK, button3_event_cb);
    Register_Button_Event(&btn3_handle, BUTTON_DOUBLE_CLICK, button3_event_cb);

    ESP_LOGI(TAG, ">>>>>Button_Init finished");
}