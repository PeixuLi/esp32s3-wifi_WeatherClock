#include <stdio.h>
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "register_button.h"
#include "app_ui_set.h"
#include "app_ui.h"

static const char *TAG = "app_ui.c";

extern lv_obj_t *btn1, *btn2, *btn3;

static void event1(void);
static void event2(void);
static void event3(void);

void btn1_lv_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        event1();
    }
}

void btn2_lv_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        event2();
    }
}

void btn3_lv_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        event3();
    }
}

static void event1(void)
{
    static bool sw = 0;
    ESP_LOGI(TAG, "LVGL Button1 Clicked");
    lv_obj_t *label = lv_obj_get_child(btn1, 0);

    if (sw == 0)
    {
        sw = 1;
        lv_label_set_text(label, "Clicked!");
    }
    else
    {
        sw = 0;
        lv_label_set_text(label, "title 1");
    }
}

static void event2(void)
{
    static bool sw = 0;
    ESP_LOGI(TAG, "LVGL Button2 Clicked");
    lv_obj_t *label = lv_obj_get_child(btn2, 0);

    if (sw == 0)
    {
        sw = 1;
        lv_label_set_text(label, "turn on");
    }
    else
    {
        sw = 0;
        lv_label_set_text(label, "turn off");
    }
}


static void event3(void)
{
    static bool sw = 0;
    ESP_LOGI(TAG, "LVGL Button1 Clicked");
    lv_obj_t *label = lv_obj_get_child(btn3, 0);

    if (sw == 0)
    {
        sw = 1;
        lv_label_set_text(label, "Clicked!");
    }
    else
    {
        sw = 0;
        lv_label_set_text(label, "title 3");
    }
}
