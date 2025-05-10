#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "lvgl_set.h"
#include "lcd_ST7789.h"

extern esp_lcd_panel_handle_t lcd_panel_handle;
lv_disp_t * disp_handle;

void check_memory(void) 
{
    size_t free_sram = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI("Memory", "Free SRAM: %zu bytes, Free PSRAM: %zu bytes", free_sram, free_psram);
}

void lcd_lvgl_Init(esp_lcd_panel_io_handle_t *io_handle)
{
    check_memory();

    lvgl_port_cfg_t cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&cfg));

    /*Add an LCD screen to the LVGL.*/
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = *io_handle,
        .panel_handle = lcd_panel_handle,
        .buffer_size = LCD_WEIGH*LCD_HIGH,
        .trans_size = LCD_WEIGH* 10,
        .double_buffer = true,
        .hres = LCD_WEIGH,
        .vres = LCD_HIGH,
        .monochrome = false,
        //.mipi_dsi = false,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_spiram = true, // 使用 PSRAM
            .buff_dma = true,
            .swap_bytes = true,
            .sw_rotate = false,
        }
    };
    disp_handle = lvgl_port_add_disp(&disp_cfg);
    if (disp_handle == NULL) {
        ESP_LOGE("LVGL", "Failed to add display");
        check_memory(); // 检查分配失败后的内存
        return;
    }
    ESP_LOGI("LVGL", "Display added successfully");
    check_memory(); // 检查分配后的内存
    //lvgl_port_remove_disp(disp_handle);
}