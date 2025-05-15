idf_component_register(SRCS "lvgl_set.c" "lcd_ST7789.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver 
                             esp_lcd 
                             esp_lvgl_port
                             lvgl)
