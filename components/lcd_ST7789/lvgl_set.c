#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "lvgl_set.h"
#include "lcd_ST7789.h"

static const char *TAG = "lvgl_set.clvgl_set.c";

extern esp_lcd_panel_handle_t lcd_panel_handle;
lv_display_t *disp_handle;

void check_memory(void)
{
    size_t free_sram = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI("Memory", "Free SRAM: %zu bytes, Free PSRAM: %zu bytes", free_sram, free_psram);
}

void lv_update_direction(uint8_t dir)
{
    switch (dir)
    {
    case 0:
        lv_display_set_rotation(disp_handle, LV_DISPLAY_ROTATION_0);
        break;

    case 1:
        lv_display_set_rotation(disp_handle, LV_DISPLAY_ROTATION_90);
        break;

    case 2:
        lv_display_set_rotation(disp_handle, LV_DISPLAY_ROTATION_180);
        break;

    case 3:
        lv_display_set_rotation(disp_handle, LV_DISPLAY_ROTATION_270);
        break;

    default:
        break;
    }
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
        .buffer_size = LCD_width * LCD_height,
        .trans_size = LCD_width * 10,
        .double_buffer = true,
        .hres = LCD_width,
        .vres = LCD_height,
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
        }};
    disp_handle = lvgl_port_add_disp(&disp_cfg);
    if (disp_handle == NULL)
    {
        ESP_LOGE("LVGL", "Failed to add display");
        check_memory(); // 检查分配失败后的内存
        return;
    }
    ESP_LOGI("LVGL", "Display added successfully");
    check_memory(); // 检查分配后的内存
}