#include <stdio.h>
#include <time.h>
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "register_button.h"
#include "app_ui.h"
#include "ui_home.h"

static const char *TAG = "app_ui.c";

///////////////////////////////////////////////////////////////////////////

void create_ui(void)
{
    // 锁定 LVGL 任务（确保线程安全）
    lvgl_port_lock(0);
    create_homePage();
    //BaseType_t core_id = xPortGetCoreID();
    //printf("Function %s is running on core %d\n", __func__, core_id);
    // 释放锁
    lvgl_port_unlock();
}