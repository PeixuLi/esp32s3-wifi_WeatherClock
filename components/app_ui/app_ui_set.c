#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "register_button.h"
#include "app_button.h"
#include "app_ui_set.h"
#include "app_ui.h"

extern lv_disp_t * disp_handle;

// 按键状态标志（用于 input_read_cb）
extern volatile bool btn1_pressed;
extern volatile bool btn2_pressed;
extern volatile bool btn3_pressed;

lv_obj_t *btn1 = NULL;
lv_obj_t *btn2 = NULL;
lv_obj_t *btn3 = NULL;
lv_indev_t *indev = NULL; // LVGL 输入设备

void input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
    lv_group_t *group = lv_indev_get_group(indev);
    lv_obj_t *focused_obj = lv_group_get_focused(group);

    if (btn1_pressed) 
    {
        btn1_pressed = false; // 清除标志
        last_key = LV_KEY_PREV; 
        data->state = LV_INDEV_STATE_PRESSED;  
        ESP_LOGI("input_read_cb", "LV_KEY_UP triggered");
    }
    else if (btn2_pressed)
    {
        btn2_pressed = false; // 清除标志
        last_key = LV_KEY_NEXT;   
        data->state = LV_INDEV_STATE_PRESSED;  
        //lv_group_focus_next(group); // 向下切换焦点
        ESP_LOGI("input_read_cb", "LV_KEY_DOWN triggered");
    }
    else if (btn3_pressed)
    {
        btn3_pressed = false; // 清除标志

        last_key = LV_KEY_ENTER;    //表示“确认”，触发焦点控件的默认动作（如点击按钮）
        data->state = LV_INDEV_STATE_PRESSED;  
        ESP_LOGI("input_read_cb", "LV_KEY_ENTER triggered");
        ESP_LOGI("input_read_cb", "Current focused obj: %p", focused_obj);
    }
    else 
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

void create_ui(void) 
{
    // 锁定 LVGL 任务（确保线程安全）
    lvgl_port_lock(0);

    // 获取当前活动屏幕
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    // 设置屏幕背景颜色
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0); // 确保背景完全覆盖

    // 创建一个标签
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello, ESP32-S3 with LVGL!");
    lv_obj_set_width(label, 200); // 限制宽度，避免溢出
    //lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    // 设置字体
    lv_obj_set_style_text_font(label, &lv_font_montserrat_22, 0); // 使用 22 像素的 Montserrat 字体
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0); 
    
    lvgl_create_button();

    // 释放锁
    lvgl_port_unlock();
}

// 创建按钮 1
static void create_lv_button1(void)
{
    lvgl_port_lock(0);

    btn1 = lv_button_create(lv_scr_act());
    lv_obj_set_size(btn1, 150, 50);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -60);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x0000FF), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn1, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CLICK_FOCUSABLE); // 确保可聚焦

    lv_obj_set_style_border_color(btn1, lv_color_hex(0xFFFF00), LV_STATE_FOCUSED); //黄色边框
    lv_obj_set_style_border_width(btn1, 4, LV_STATE_FOCUSED); // 边框宽度加大
    lv_obj_set_style_border_opa(btn1, LV_OPA_COVER, LV_STATE_FOCUSED); // 边框不透明
    //lv_obj_set_style_bg_color(btn1, lv_color_hex(0xFFFF00), LV_STATE_FOCUSED); // 黄色背景
    //lv_obj_set_style_bg_opa(btn1, LV_OPA_80, LV_STATE_FOCUSED); // 背景略透明
    //lv_obj_set_style_text_color(btn1, lv_color_hex(0xFF0000), LV_STATE_FOCUSED); // 红色文字

    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "title 1");
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);

    lv_obj_add_event_cb(btn1, btn1_lv_event_cb, LV_EVENT_CLICKED, NULL);

    lvgl_port_unlock();
}

// 创建按钮 2
static void create_lv_button2(void)
{
    lvgl_port_lock(0);

    //创建一个按键
    btn2 = lv_button_create(lv_scr_act());
    lv_obj_set_size(btn2, 150, 50);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x0000FF), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn2, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);

    //被聚焦时
    lv_obj_set_style_border_color(btn2, lv_color_hex(0xFFFF00), LV_STATE_FOCUSED); // 白色边框
    lv_obj_set_style_border_width(btn2, 4, LV_STATE_FOCUSED); // 边框宽度加大
    lv_obj_set_style_border_opa(btn2, LV_OPA_COVER, LV_STATE_FOCUSED); // 边框不透明

    //被选中时
    // 选中状态（CHECKED）
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xFFFFFF), LV_STATE_CHECKED); // 白色背景（选中）
    lv_obj_set_style_bg_opa(btn2, LV_OPA_COVER, LV_STATE_CHECKED);
    lv_obj_set_style_text_color(btn2, lv_color_hex(0x000000), LV_STATE_CHECKED); // 黑色文字

    lv_obj_t *label = lv_label_create(btn2);
    lv_label_set_text(label, "checkable");
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);

    lv_obj_add_event_cb(btn2, btn2_lv_event_cb, LV_EVENT_CLICKED, NULL);

    lvgl_port_unlock();
}

// 创建按钮 3
static void create_lv_button3(void)
{
    lvgl_port_lock(0);

    btn3 = lv_button_create(lv_scr_act());
    lv_obj_set_size(btn3, 150, 50);
    lv_obj_align(btn3, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0x0000FF), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn3, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_remove_flag(btn3, LV_OBJ_FLAG_PRESS_LOCK);
    
    lv_obj_set_style_border_color(btn3, lv_color_hex(0xFFFF00), LV_STATE_FOCUSED); // 黄色边框
    lv_obj_set_style_border_width(btn3, 4, LV_STATE_FOCUSED); // 边框宽度加大
    lv_obj_set_style_border_opa(btn3, LV_OPA_COVER, LV_STATE_FOCUSED); // 边框不透明
    //lv_obj_set_style_bg_color(btn3, lv_color_hex(0xFFFF00), LV_STATE_FOCUSED); // 黄色背景
    //lv_obj_set_style_bg_opa(btn3, LV_OPA_80, LV_STATE_FOCUSED); // 背景略透明
    //lv_obj_set_style_text_color(btn3, lv_color_hex(0xFF0000), LV_STATE_FOCUSED); // 红色文字

    lv_obj_t *label = lv_label_create(btn3);
    lv_label_set_text(label, "title 3");
    lv_obj_center(label);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);

    lv_obj_add_event_cb(btn3, btn3_lv_event_cb, LV_EVENT_CLICKED, NULL);

    lvgl_port_unlock();
}


// 初始化 LVGL 按键输入设备
static void lvgl_keypad_init(void)
{
    lv_group_t *group = lv_group_create();//创建一个组
    lv_group_add_obj(group, btn1);  //将一个对象添加到组中。
    lv_group_add_obj(group, btn2);  //将一个对象添加到组中。
    lv_group_add_obj(group, btn3);  //将一个对象添加到组中。
    lv_group_set_default(group);

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD); // 设置为指针类型
    lv_indev_set_read_cb(indev, input_read_cb); // 设置读取回调函数

    lv_indev_set_group(indev, group); //将一个组与一个输入设备关联起来

}

void lvgl_create_button(void)
{
    Button_Init();
    create_lv_button1();
    create_lv_button2();
    create_lv_button3();
    lvgl_keypad_init();
}