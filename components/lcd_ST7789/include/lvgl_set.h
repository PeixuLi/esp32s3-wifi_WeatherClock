#ifndef __LVGL_SET_H_
#define __LVGL_SET_H_

void lcd_lvgl_Init(esp_lcd_panel_io_handle_t *io_handle);
void lv_update_direction(uint8_t dir);
void lv_update(void);

#endif 