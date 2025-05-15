#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "app_ui.h"
#include "ui_home.h"
#include "wifi_sntp_get.h"
#include "http_get_weather.h"

lv_obj_t *scr;
//home_page
lv_obj_t *line1,*line2;
lv_obj_t *date_label;
lv_obj_t *time_label;
lv_obj_t *weather_label;

lv_timer_t *lv_timer_handle;
lv_timer_t *lv_weather_handle;

extern lv_disp_t *disp_handle;
extern Now_Weather_t now_weather;
struct tm localTime;

static void lv_updateDate_cb(lv_event_t* e);
static void lv_updateTime_cb(lv_timer_t* timer_handle);
static void lv_updateWeather_cb(lv_event_t * e);

///////////////////////////////////////////////////////////////////////////////////////
void lv_reflash_weather(void)
{
    lvgl_port_lock(0);
    lv_obj_send_event(weather_label, LV_OBJ_FLAG_CLICKABLE, NULL);
    lvgl_port_unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void lv_reflash_data(void* parameter)
{
    // 创建显示时间定时器
    while (1)
    {
        lv_timer_handle = lv_timer_create(lv_updateTime_cb, 1000, time_label);
        // 发送更新日期信息
        lv_obj_send_event(time_label, LV_OBJ_FLAG_CLICKABLE, NULL);
        break;
    }
    vTaskDelete(NULL);
}

//事件回调区
static void lv_updateDate_cb(lv_event_t * e)
{
    ESP_LOGI("lv_updateDate_cb", "enter");
    Get_local_time(&localTime);
    
    lv_obj_t* label = lv_event_get_user_data(e); 

    char format_time[64];
    strftime(format_time, sizeof(format_time), " %Y-%m-%d %A", &localTime);
    lv_label_set_text_fmt(label, "%s", format_time); // 支持printf格式[1,7](@ref)
}

static void lv_updateTime_cb(lv_timer_t * timer_handle)
{
    Get_local_time(&localTime);
    lv_obj_t* label = (lv_obj_t*)lv_timer_get_user_data(timer_handle); 

    char format_time[64];
    strftime(format_time, sizeof(format_time), " %H : %M : %S ", &localTime);
    lv_label_set_text_fmt(label, "%s", format_time); // 支持printf格式[1,7](@ref)

    if(localTime.tm_hour == 0 && localTime.tm_min == 0 && localTime.tm_sec == 0)
    {
        lv_obj_send_event(label, LV_OBJ_FLAG_CLICKABLE, NULL);
    }
}

static void lv_updateWeather_cb(lv_event_t * e)
{
    lv_obj_t* label = lv_event_get_target(e);
    char str[100];

    snprintf(str, sizeof(str), 
        "%s    temp:%d  %s",
        now_weather.location, now_weather.temperature, now_weather.weather_situation);

    lv_label_set_text_fmt(label, "%s", str); // 支持printf格式[1,7](@ref)
}

///////////////////////////////////////////////////////////////////////////////////////
void lv_set_background(void)
{
    // 获取当前活动屏幕
    scr = lv_disp_get_scr_act(NULL);

    // 设置屏幕背景颜色
    lv_obj_set_style_bg_color(scr, lv_color_hex(LV_RED), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0); // 确保背景完全覆盖

    // 画线
    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 5);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    
    // 这个数组应该是静态、全局或动态分配的，不能是函数中的局部变量
    static lv_point_precise_t line1_points[] = {{0, 50}, {320, 50}};

    line1 = lv_line_create(lv_screen_active());
    lv_line_set_points(line1, line1_points, 2); /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    lv_obj_move_foreground(line1); // 确保线条在背景上层

    /*Create a line and apply the new style*/
    
    static lv_point_precise_t line2_points[] = {{0, 190}, {320, 190}};

    line2 = lv_line_create(lv_screen_active());
    lv_line_set_points(line2, line2_points, 2); /*Set the points*/
    lv_obj_add_style(line2, &style_line, 0);
    lv_obj_move_foreground(line2); // 确保线条在背景上层
}

static void lv_create_dispDate(void)
{
    scr = lv_disp_get_scr_act(NULL);
    // 创建一个标签
    date_label = lv_label_create(scr);
    lv_label_set_text(date_label, "--/--/--     --");

    // lv_obj_set_width(date_label, width);
    lv_obj_align(date_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // 设置字体
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_22, 0); // 使用 30 像素的 Montserrat 字体
    lv_obj_set_style_text_color(date_label, lv_color_hex(LV_BLACK), 0);
    lv_obj_set_style_text_align(date_label, LV_TEXT_ALIGN_CENTER, 0); // 水平居中

    // 使用 Flex 布局使文本垂直居中
    lv_obj_set_style_layout(date_label, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(date_label, LV_FLEX_FLOW_COLUMN, 0);         // 垂直排列
    lv_obj_set_style_flex_main_place(date_label, LV_FLEX_ALIGN_CENTER, 0);  // 主轴（垂直）居中
    lv_obj_set_style_flex_cross_place(date_label, LV_FLEX_ALIGN_CENTER, 0); // 交叉轴（水平）居中
}

// 创建时间显示区域
static void lv_create_disTime(void)
{
    // 获取当前活动屏幕
    scr = lv_disp_get_scr_act(NULL);
    int32_t width = lv_obj_get_width(scr);

    // 创建一个标签
    time_label = lv_label_create(scr);
    lv_label_set_text(time_label, "--:--:--");

    lv_obj_set_width(time_label, width);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    // 设置字体
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0); // 使用 30 像素的 Montserrat 字体
    lv_obj_set_style_text_color(time_label, lv_color_hex(LV_BLACK), 0);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0); // 水平居中

    // 使用 Flex 布局使文本垂直居中
    lv_obj_set_style_layout(time_label, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(time_label, LV_FLEX_FLOW_COLUMN, 0);         // 垂直排列
    lv_obj_set_style_flex_main_place(time_label, LV_FLEX_ALIGN_CENTER, 0);  // 主轴（垂直）居中
    lv_obj_set_style_flex_cross_place(time_label, LV_FLEX_ALIGN_CENTER, 0); // 交叉轴（水平）居中

    /**************************************/
    //创建一个标签事件
    lv_obj_add_event_cb(time_label, lv_updateDate_cb, LV_OBJ_FLAG_CLICKABLE, date_label);   /* 分配一个事件回调 */
}

// 创建温度显示区域
static void lv_create_disWeather(void)
{
    // 获取当前活动屏幕
    scr = lv_disp_get_scr_act(NULL);
    int32_t width = lv_obj_get_width(scr);

    // 创建一个标签
    weather_label = lv_label_create(scr);
    lv_label_set_text(weather_label, "--    temp:--  --");

    lv_obj_set_width(weather_label, width);
    lv_obj_align(weather_label, LV_ALIGN_TOP_LEFT, 0, 0);

    // 设置字体
    lv_obj_set_style_text_font(weather_label, &lv_font_montserrat_22, 0); // 使用 30 像素的 Montserrat 字体
    lv_obj_set_style_text_color(weather_label, lv_color_hex(LV_BLACK), 0);
    lv_obj_set_style_text_align(weather_label, LV_TEXT_ALIGN_CENTER, 0); // 水平居中

    // 使用 Flex 布局使文本垂直居中
    lv_obj_set_style_layout(weather_label, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(weather_label, LV_FLEX_FLOW_COLUMN, 0);         // 垂直排列
    lv_obj_set_style_flex_main_place(weather_label, LV_FLEX_ALIGN_CENTER, 0);  // 主轴（垂直）居中
    lv_obj_set_style_flex_cross_place(weather_label, LV_FLEX_ALIGN_CENTER, 0); // 交叉轴（水平）居中

    /**************************************/
    //创建一个标签事件
    lv_obj_add_event_cb(weather_label, lv_updateWeather_cb, LV_OBJ_FLAG_CLICKABLE, NULL);   /* 分配一个事件回调 */
}

void create_homePage(void)
{
     // 锁定 LVGL 任务（确保线程安全）
    lvgl_port_lock(0);

    lv_set_background();
    // 创建日期显示区域
    lv_create_dispDate();
    // 创建时间显示区域
    lv_create_disTime();
    // 创建温度显示区域
    lv_create_disWeather();

    // 释放锁
    lvgl_port_unlock();
}