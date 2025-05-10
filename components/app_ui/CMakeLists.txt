idf_component_register(SRCS "app_ui.c" "lvgl_set.c" "app_ui_set.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver 
                             bsp_button 
                             app_button 
                             lvgl 
                             esp_lcd 
                             esp_lvgl_port 
                             lcd_ST7789)
