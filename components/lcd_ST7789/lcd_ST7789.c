#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_ops.h"
#include "lcd_ST7789.h"
#include "lvgl_set.h"

static const char *TAG = "LCD_ST7789";

esp_lcd_panel_io_handle_t   lcd_io_handle;
esp_lcd_panel_handle_t      lcd_panel_handle;

uint16_t LCD_width = 240;
uint16_t LCD_height = 320;

void lcd_set_direction(lcd_rotation_t rotation, esp_lcd_panel_io_handle_t io)
{
    uint8_t madctl_value = 0x00;

    switch(rotation)
    {
    case LCD_ROTATION_0:
        LCD_width = ST7789_WIDTH;
        LCD_height = ST7789_HEIGHT;
        madctl_value = 0x00;// 纵屏
        lv_update_direction(0);

        break;
    
    case LCD_ROTATION_90:
        LCD_width = ST7789_HEIGHT;
        LCD_height = ST7789_WIDTH;
        madctl_value = 0x60; // 横屏
        lv_update_direction(1);

        break;
    
    case LCD_ROTATION_180:
        LCD_width = ST7789_WIDTH;
        LCD_height = ST7789_HEIGHT;
        madctl_value = 0xC0; // 倒置纵屏
        lv_update_direction(2);

        break;

    case LCD_ROTATION_270:
        LCD_width = ST7789_HEIGHT;
        LCD_height = ST7789_WIDTH;
        madctl_value = 0xA0; // 反向横屏
        lv_update_direction(3);

        break;
    }

    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(lcd_io_handle, 0x36, &madctl_value, 1));
    //vTaskDelay(pdMS_TO_TICKS(120));
}

static void lcd_vendor_specific_init(esp_lcd_panel_io_handle_t io)
{
    //第一列：命令（Command）  表示要发送到ST7789的寄存器命令代码
    //第二列：参数长度（Parameter Length）  表示该命令后续的参数数量
    //第三列及后续：参数数据（Parameters）  命令的具体参数值（uint8_t类型，16进制），数量由第二列指定
    static const uint8_t vendor_specific_init[] = {
        0x01, 0,       // 软复位
        0x11, 0,       // 退出睡眠模式
        0x3A, 1, 0x55, // COLMOD: RGB565 16位/像素
        0x36, 1, 0x00, // MADCTL: 正常方向，RGB顺序（依赖BGR设置）
        0xC5, 1, 0x1A, // VCOM设置（厂家特定）
        0xB2, 5, 0x05, 0x05, 0x00, 0x33, 0x33, // Porch设置：时序控制
        0xB7, 1, 0x05, // Gate控制：栅极电压
        0xBB, 1, 0x3F, // VCOM设置：公共电压（厂家值）
        0xC0, 1, 0x2C, // LCM控制：液晶模块参数
        0xC2, 2, 0x01, 0xFF, // VDV/VRH命令启用
        0xC3, 1, 0x0F, // VRH设置：参考电压（厂家值）
        0xC4, 1, 0x20, // VDV设置：偏差电压
        0xC6, 1, 0x0F, // 帧率控制：约60Hz（降低以减少闪烁）
        0xD0, 2, 0xA4, 0xA1, // 电源控制
        0xE8, 1, 0x03, // 显示时序
        0xE9, 3, 0x09, 0x09, 0x08, // 显示时序
        //0xE0, 14, 0xD0, 0x05, 0x09, 0x09, 0x08, 0x14, 0x28, 0x33, 0x3F, 0x07, 0x13, 0x14, 0x28, 0x30, // 正伽马校正（厂家值）
        //0xE1, 14, 0xD0, 0x05, 0x09, 0x09, 0x08, 0x03, 0x24, 0x32, 0x32, 0x3B, 0x14, 0x13, 0x28, 0x2F, // 负伽马校正（厂家值）
        0xE0, 14, 0xD0, 0x02, 0x07, 0x0A, 0x0A, 0x18, 0x2D, 0x38, 0x3F, 0x0C, 0x17, 0x18, 0x2E, 0x36,
        0xE1, 14, 0xD0, 0x02, 0x07, 0x0A, 0x0A, 0x04, 0x29, 0x36, 0x36, 0x3E, 0x17, 0x16, 0x2E, 0x35,
        0x20, 0,       // 关闭显示反转
        0x29, 0,       // 开启显示
    };
    for (size_t i = 0; i < sizeof(vendor_specific_init); i += vendor_specific_init[i + 1] + 2) {
        ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io, vendor_specific_init[i], &vendor_specific_init[i + 2], vendor_specific_init[i + 1]));
        if (vendor_specific_init[i] == 0x11) vTaskDelay(pdMS_TO_TICKS(120));
        if (vendor_specific_init[i] == 0x01) vTaskDelay(pdMS_TO_TICKS(120));
        if (vendor_specific_init[i] == 0x20) vTaskDelay(pdMS_TO_TICKS(120));
        if (vendor_specific_init[i] == 0x29) vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void LCD_Init(void)
{
    // 启用esp_lcd调试日志
    esp_log_level_set("esp_lcd", ESP_LOG_DEBUG);
    
    /* 设置SPI总线 */
    ESP_LOGI(TAG, "SPI Bus Initialized");
    spi_bus_config_t bus_config = {
        .mosi_io_num = LCD_DAT,
        .miso_io_num = -1,
        .sclk_io_num = LCD_CLK,
        .quadwp_io_num = -1,                  // 必须设置且为 `-1`
        .quadhd_io_num = -1,                  // 必须设置且为 `-1`
        .max_transfer_sz = LCD_width * LCD_height * 2,// 表示 SPI 单次传输允许的最大字节数上限，通常设为全屏大小即可
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_config, SPI_DMA_CH_AUTO));
                    // 第 1 个参数表示使用的 SPI 主机 ID，和后续创建接口设备时保持一致
                    // 第 3 个参数表示使用的 DMA 通道号，默认设置为 `SPI_DMA_CH_AUTO` 即可

    ESP_LOGI(TAG, "LCD Panel Initialized");                
    /*配置控制背光的gpio "Turn off LCD backlight"); */
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_BK,
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    /* LCD属性配置 */
    //Create LCD panel IO handle, for SPI interface
    esp_lcd_panel_io_spi_config_t lcd_io_config = {
        .cs_gpio_num = LCD_CS,
        .dc_gpio_num = LCD_DC,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .pclk_hz = 60000000,
        .spi_mode = 0,
        .trans_queue_depth = 10,   // SPI 设备传输数据的队列深度，一般设为 10 即可
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &lcd_io_config, &lcd_io_handle));

    /* Install ST7789 panel driver */
    esp_lcd_panel_dev_config_t panel_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = LCD_RST,
        //.data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
        .data_endian = LCD_RGB_DATA_ENDIAN_BIG,
    };  
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcd_io_handle,&panel_dev_config,&lcd_panel_handle));

    // 初始化面板
    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel_handle));
    lcd_vendor_specific_init(lcd_io_handle);
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel_handle, true));

    //初始化lvgl
    lcd_lvgl_Init(&lcd_io_handle);
    vTaskDelay(pdMS_TO_TICKS(120));
    lcd_set_direction(LCD_ROTATION_90, lcd_io_handle);

    // 背光控制
    gpio_set_level(LCD_BK, 1); // 点亮背光
}
