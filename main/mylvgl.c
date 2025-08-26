#include "mylvgl.h"
#include "qmi8658.h"

static lv_display_t *disp = NULL; // 显示器句柄

// LVGL 显示刷新回调函数
static void disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    // 提取区域坐标 - LVGL使用包含式坐标(inclusive)
    int32_t x_start = area->x1;   // 左边界，例如：0
    int32_t y_start = area->y1;   // 上边界，例如：0
    int32_t x_end = area->x2 + 1; // 右边界+1，例如：319+1=320 (esp_lcd需要排他式坐标)
    int32_t y_end = area->y2 + 1; // 下边界+1，例如：39+1=40   (esp_lcd需要排他式坐标)
    // 将数据刷新到屏幕
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_end, y_end, px_map);
    lv_display_flush_ready(disp);
}

// LVGL 定时器回调
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(1); // 增加 1ms
}

// 触摸屏读取回调
void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    static uint32_t debug_counter = 0;
    uint16_t x, y;
    uint8_t touch_cnt = 0;
    esp_lcd_touch_handle_t tp = lv_indev_get_user_data(indev);

    // 每1000次调用输出一次调试信息，确认回调被调用
    if (++debug_counter % 1000 == 0)
    {
        ESP_LOGI("TOUCH", "touchpad_read 被调用 %lu 次", debug_counter);
    }

    if (tp == NULL)
    {
        ESP_LOGE("TOUCH", "触摸屏句柄为空");
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    // 先检查触摸屏是否响应
    esp_err_t ret = esp_lcd_touch_read_data(tp);
    if (ret != ESP_OK)
    {
        // 只在前100次错误时输出，避免刷屏
        static uint32_t error_count = 0;
        if (error_count++ < 100)
        {
            ESP_LOGW("TOUCH", "触摸屏读取失败: %s (错误 #%lu)", esp_err_to_name(ret), error_count);
        }
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    bool touched = esp_lcd_touch_get_coordinates(tp, &x, &y, NULL, &touch_cnt, 1);

    if (touched && touch_cnt > 0)
    {
        // 手动偏移Y轴：如果Y轴范围是84-318，偏移84使其变成0-234
        int16_t adjusted_x = x;
        int16_t adjusted_y = y - 81; // 减去81，让Y轴从0开始

        // 确保坐标在有效范围内
        if (adjusted_y < 0)
            adjusted_y = 0;
        if (adjusted_y >= LCD_V_RES)
            adjusted_y = LCD_V_RES - 1;
        if (adjusted_x < 0)
            adjusted_x = 0;
        if (adjusted_x >= LCD_H_RES)
            adjusted_x = LCD_H_RES - 1;

        data->point.x = adjusted_x;
        data->point.y = adjusted_y;
        data->state = LV_INDEV_STATE_PRESSED;

        // 输出原始坐标和调整后坐标到串口
        ESP_LOGI("TOUCH", "触摸有效: 原始(%d,%d) -> 调整后(%d,%d)", x, y, adjusted_x, adjusted_y);
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

void lvgl_init(void)
{
    static lv_color_t buf1[LCD_H_RES * 40]; // 缓冲区1：320×40=12800像素，25.6KB
    static lv_color_t buf2[LCD_H_RES * 40]; // 缓冲区2：320×40=12800像素，25.6KB
    // 1. 初始化 LVGL
    lv_init();

    // 2. 初始化 LCD 驱动
    esp_lcd_panel_handle_t panel_handle = lcd_init();

    // 3. 创建显示器
    disp = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_user_data(disp, panel_handle);
    // 4. 设置颜色格式
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    // 5. 设置缓冲区
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // 6. 设置刷新回调
    lv_display_set_flush_cb(disp, disp_flush);

    // 7. 创建定时器任务（LVGL 需要定时调用 lv_tick_inc）
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "lv_tick_task"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000)); // 1ms

    // 8. 打开背光
    bsp_display_backlight_on();

    // 9. 初始化 GUI Guider 生成的界面
    setup_ui(&guider_ui);
    events_init(&guider_ui);

    // 10. 创建触摸屏输入设备
    esp_lcd_touch_handle_t tp_handle = touch_init(); // 先初始化触摸屏
    if (tp_handle != NULL)
    {
        lv_indev_t *indev = lv_indev_create(); // 创建输入设备
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touchpad_read);
        lv_indev_set_user_data(indev, tp_handle);
        ESP_LOGI("TOUCH", "触摸屏输入设备创建成功");
    }
    else
    {
        ESP_LOGW("TOUCH", "触摸屏初始化失败，跳过输入设备创建");
    }

    ESP_LOGI("LVGL", "LVGL 初始化完成");
}

// LVGL 主任务
void lvgl_task(void *pvParameter)
{
    static uint32_t last_update_time = 0;
    const uint32_t update_interval = 100; // 100ms更新一次传感器显示

    while (1)
    {
        lv_timer_handler();

        // 每100ms更新一次传感器数据显示
        uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (current_time - last_update_time >= update_interval)
        {
            update_sensor_display();
            last_update_time = current_time;
        }

        // 让出CPU控制权10毫秒，允许其他FreeRTOS任务运行
        // 这确保了系统的实时性，避免LVGL独占CPU
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// 更新传感器数据显示函数
void update_sensor_display(void)
{
    static char label_text[100]; // 标签显示的简化文本

    // 格式化标签显示的简化文本
    snprintf(label_text, sizeof(label_text),
             "qmi8658: X:%.1f° Y:%.1f° Z:%.1f°",
             QMI8658.AngleX, QMI8658.AngleY, QMI8658.AngleZ);

    // 更新传感器标签显示的文本
    if (guider_ui.screen_label_sensor != NULL)
    {
        lv_label_set_text(guider_ui.screen_label_sensor, label_text);
    }
}