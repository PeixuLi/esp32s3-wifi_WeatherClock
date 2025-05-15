#ifndef __LCD_ST7789_H_
#define __LCD_ST7789_H_

#include "esp_lcd_types.h"

#define ST7789_WIDTH     320
#define ST7789_HEIGHT    240

// 定义屏幕旋转角度
typedef enum {
    LCD_ROTATION_0 = 0,   // 默认纵屏
    LCD_ROTATION_90,      // 横屏
    LCD_ROTATION_180,     // 倒置纵屏
    LCD_ROTATION_270      // 反向横屏
} lcd_rotation_t;

// 根据旋转角度动态设置分辨率
extern uint16_t LCD_width;
extern uint16_t LCD_height;

#define LCD_HOST    SPI2_HOST
#define LCD_DAT     2
#define LCD_CLK     1
#define LCD_BK      39
#define LCD_CS      40
#define LCD_DC      41
#define LCD_RST     42

void LCD_Init(void);
void lcd_set_direction(lcd_rotation_t rotation, esp_lcd_panel_io_handle_t io);

#endif
