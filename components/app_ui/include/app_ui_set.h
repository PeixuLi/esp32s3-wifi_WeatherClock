#ifndef __APP_UI_SET_H_
#define __APP_UI_SET_H_

#include "lvgl.h"
#include "button_types.h"

void create_ui(void); 
void lvgl_create_button(void);
void input_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

#endif 