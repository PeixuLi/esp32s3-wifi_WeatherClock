#ifndef __APP_UI_H_
#define __APP_UI_H_

#include "lvgl.h"
#include "esp_lvgl_port.h"

/* 基本颜色 */
#define LV_BLACK        0x000000  // 黑色 (R:0, G:0, B:0)
#define LV_WHITE        0xFFFFFF  // 白色 (R:255, G:255, B:255)
#define LV_RED          0xFF0000  // 红色 (R:255, G:0, B:0)
#define LV_GREEN        0x00FF00  // 绿色 (R:0, G:255, B:0)
#define LV_BLUE         0x0000FF  // 蓝色 (R:0, G:0, B:255)

/* 灰度颜色 */
#define LV_GRAY         0x808080  // 中灰色 (R:128, G:128, B:128)
#define LV_LIGHT_GRAY   0xC0C0C0  // 浅灰色 (R:192, G:192, B:192)
#define LV_DARK_GRAY    0x404040  // 深灰色 (R:64, G:64, B:64)

/* 常见彩色 */
#define LV_YELLOW       0xFFFF00  // 黄色 (R:255, G:255, B:0)
#define LV_CYAN         0x00FFFF  // 青色 (R:0, G:255, B:255)
#define LV_MAGENTA      0xFF00FF  // 品红 (R:255, G:0, B:255)
#define LV_ORANGE       0xFFA500  // 橙色 (R:255, G:165, B:0)
#define LV_PURPLE       0x800080  // 紫色 (R:128, G:0, B:128)
#define LV_PINK         0xFFC0CB  // 粉色 (R:255, G:192, B:203)

/* 其他实用颜色 */
#define LV_BROWN        0xA52A2A  // 棕色 (R:165, G:42, B:42)
#define LV_OLIVE        0x808000  // 橄榄绿 (R:128, G:128, B:0)
#define LV_SKY_BLUE     0x87CEEB  // 天蓝色 (R:135, G:206, B:235)
#define LV_GOLD         0xFFD700  // 金色 (R:255, G:215, B:0)
#define LV_VIOLET       0xEE82EE  // 紫罗兰 (R:238, G:130, B:238)

void create_ui(void);

#endif 