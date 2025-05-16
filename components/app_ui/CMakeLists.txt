idf_component_register(SRCS "app_message.c" "ui_home.c" "ui_button.c" "app_ui.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver 
                             bsp_button 
                             app_button 
                             lvgl 
                             esp_lcd 
                             esp_lvgl_port 
                             lcd_ST7789
                             wifi_sta
                             app_http_data)
