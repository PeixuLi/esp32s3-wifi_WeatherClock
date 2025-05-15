#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "app_button.h"
#include "app_ui.h"

// 按键状态标志（用于 input_read_cb）
extern volatile bool btn1_pressed;
extern volatile bool btn2_pressed;
extern volatile bool btn3_pressed;

lv_indev_t *indev = NULL; // LVGL 输入设备
extern lv_disp_t * disp_handle;

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

// 初始化 LVGL 按键输入设备
static void lvgl_keypad_init(void)
{
    lv_group_t *group = lv_group_create();//创建一个组
    //lv_group_add_obj(group, btn1);  //将一个对象添加到组中。
    
    lv_group_set_default(group);

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD); // 设置为指针类型
    lv_indev_set_read_cb(indev, input_read_cb); // 设置读取回调函数

    lv_indev_set_group(indev, group); //将一个组与一个输入设备关联起来
}

void lvgl_create_button(void)
{
    Button_Init();
    lvgl_keypad_init();
}