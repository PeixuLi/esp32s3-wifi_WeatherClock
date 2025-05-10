#ifndef __LCD_ST7789_H_
#define __LCD_ST7789_H_

#define LCD_WEIGH   240
#define LCD_HIGH    320

#define LCD_HOST    SPI2_HOST
#define LCD_DAT     2
#define LCD_CLK     1
#define LCD_BK      39
#define LCD_CS      40
#define LCD_DC      41
#define LCD_RST     42

void LCD_Init(void);

#endif
