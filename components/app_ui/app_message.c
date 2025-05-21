#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "app_message.h"

static const char *TAG = "app_message.c";

TaskHandle_t msgbox_event_handle;
msgbox_request_t msg;
lv_obj_t *current_mbox = NULL;    // 静态变量保存当前消息框
lv_timer_t *current_timer = NULL; // 静态变量保存当前定时器

static void msgbox_timer_cb(lv_timer_t *handle)
{
    lvgl_port_lock(0);
    lv_timer_pause(handle); // 先暂停，防止并发调用
    lv_obj_t *mbox = lv_timer_get_user_data(handle);
    lv_timer_del(handle);
    current_timer = NULL;

    if (mbox)
    {
        lv_msgbox_close(mbox);
        if (mbox == current_mbox)
        {
            current_mbox = NULL;
        }
    }
    lvgl_port_unlock();
}

void lv_show_hint(char *string, uint32_t close_time_delay)
{
    lvgl_port_lock(0);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "提示文本为空");
        lvgl_port_unlock();
        return;
    }
    msgbox_request_t msg;
    msg.auto_close_delay = close_time_delay;
    strncpy(msg.text, string, sizeof(msg.text) - 1);
    msg.text[sizeof(msg.text) - 1] = '\0'; // 确保字符串以空字符结尾

    // 销毁旧消息框（若存在）
    if (!current_mbox) {
        current_mbox = lv_msgbox_create(NULL); // 首次创建
        lv_obj_set_style_text_font(current_mbox, &lv_font_montserrat_18, LV_PART_MAIN);
    }
    lv_msgbox_add_text(current_mbox, string);

    // 重置或创建定时器
    if (current_timer == NULL)
    {
        current_timer = lv_timer_create(msgbox_timer_cb, msg.auto_close_delay, current_mbox);
    }
    else
    {
        lv_timer_set_period(current_timer, msg.auto_close_delay);
        lv_timer_set_user_data(current_timer, current_mbox); // 更新 user_data
        lv_timer_reset(current_timer);
    }

    lvgl_port_unlock();
}

/*
void lv_show_hint(char *string, uint32_t close_time_delay)
{
    lvgl_port_lock(0);
    if (string == NULL)
    {
        ESP_LOGE(TAG, "提示文本为空");
        return;
    }

    // 销毁旧消息框和定时器
    if (current_timer) {
        lv_timer_del(current_timer);
        current_timer = NULL;
    }
    if (current_mbox) {
        lv_msgbox_close(current_mbox);
        current_mbox = NULL;
    }

    msgbox_request_t msg;
    msg.auto_close_delay = close_time_delay;
    strncpy(msg.text, string, sizeof(msg.text) - 1);
    msg.text[sizeof(msg.text) - 1] = '\0'; // 确保字符串以空字符结尾

    // 创建新消息框
    current_mbox = lv_msgbox_create(NULL);
    lv_obj_set_style_text_font(current_mbox, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_msgbox_add_text(current_mbox, msg.text);

    current_timer = lv_timer_create(msgbox_timer_cb, msg.auto_close_delay, current_mbox);

    lvgl_port_unlock();
}
*/