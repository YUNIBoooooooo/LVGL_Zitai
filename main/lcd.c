#include "lcd.h"

static const char *TAG = "TP";

static esp_err_t bsp_display_brightness_init(void)
{
    const ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK};
    const ledc_channel_config_t channel_cfg = {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = true};
    ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));
    ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));
    return ESP_OK;
}

/* lcd_init()：配置SPI总线，创建面板IO和驱动程序，复位、初始化和配置ST7789。
 * 此函数重用问题中定义的配置并返回有效的面板句柄。
 */
esp_lcd_panel_handle_t lcd_init(void)
{
    // 初始化PCA9557 IO扩展芯片
    pca9557_init();

    // 初始化背光PWM
    bsp_display_brightness_init();

    // SPI总线配置
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * sizeof(uint16_t)};
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // 创建面板IO
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 2,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL};
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                             &io_cfg, &io_handle));

    // 创建ST7789面板
    esp_lcd_panel_handle_t panel;
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_endian = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16};
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &panel));

    // 复位并初始化面板
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel));
    lcd_cs(0); // 确保CS为低电平
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));

    // 配置扫描方向：反转颜色，交换轴，水平镜像
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel, true, false));

    // 打开显示；背光将单独打开
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel, true));

    return panel;
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent < 0)
        brightness_percent = 0;
    if (brightness_percent > 100)
        brightness_percent = 100;
    uint32_t duty = (1023 * brightness_percent) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));
    return ESP_OK;
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}

/**
 * @brief 触摸屏初始化
 */
esp_lcd_touch_handle_t touch_init(void)
{
    ESP_LOGI(TAG, "开始初始化触摸屏...");

    // 创建触摸屏IO配置
    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = {
        .dev_addr = 0x38, // FT6x36地址
        .control_phase_bytes = 1,
        .dc_bit_offset = 0,
        .lcd_cmd_bits = 8,
        .flags.disable_control_phase = 1,
    };

    // 使用现有的I2C总线创建触摸屏IO
    esp_err_t ret = esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)BSP_I2C_NUM, &tp_io_config, &tp_io);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "触摸屏IO创建失败: %s", esp_err_to_name(ret));
        return NULL;
    }

    // 触摸屏配置 - 与LCD坐标变换完全一致
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,          // 使用LCD实际分辨率
        .y_max = LCD_V_RES,          // 使用LCD实际分辨率
        .rst_gpio_num = GPIO_NUM_NC, // 无复位引脚
        .int_gpio_num = GPIO_NUM_NC, // 无中断引脚（轮询模式）
        .levels.reset = 0,
        .levels.interrupt = 0,
        .flags.swap_xy = true,   // 暂时禁用：不交换XY轴
        .flags.mirror_x = true,  // 暂时禁用：不镜像X轴
        .flags.mirror_y = false, // 暂时禁用：不镜像Y轴
    };

    // 创建FT5x06触摸屏驱动
    esp_lcd_touch_handle_t tp_handle = NULL;
    ret = esp_lcd_touch_new_i2c_ft5x06(tp_io, &tp_cfg, &tp_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "触摸屏驱动创建失败: %s", esp_err_to_name(ret));
        return NULL;
    }

    // 测试触摸屏通信
    ESP_LOGI(TAG, "测试触摸屏通信...");
    uint16_t test_x, test_y;
    uint8_t test_cnt;
    esp_err_t test_ret = esp_lcd_touch_read_data(tp_handle);
    if (test_ret == ESP_OK)
    {
        bool test_touched = esp_lcd_touch_get_coordinates(tp_handle, &test_x, &test_y, NULL, &test_cnt, 1);
        ESP_LOGI(TAG, "通信测试成功，当前状态: %s", test_touched ? "有触摸" : "无触摸");
    }
    else
    {
        ESP_LOGW(TAG, "通信测试失败: %s", esp_err_to_name(test_ret));
    }

    ESP_LOGI(TAG, "✅ 触摸屏初始化完成 (FT6x36 @ 0x38)");
    ESP_LOGI(TAG, "   分辨率: %dx%d", tp_cfg.x_max, tp_cfg.y_max);
    ESP_LOGI(TAG, "   模式: 轮询 (无中断引脚)");
    ESP_LOGI(TAG, "   坐标映射: swap_xy=%s, mirror_x=%s, mirror_y=%s",
             tp_cfg.flags.swap_xy ? "true" : "false",
             tp_cfg.flags.mirror_x ? "true" : "false",
             tp_cfg.flags.mirror_y ? "true" : "false");

    return tp_handle;
}