/*
 * Copyright 2025 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "../main/lcd.h" // 包含LCD头文件以使用亮度控制函数
#include "esp_log.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

static const char *TAG = "SLIDER_EVENT";

// 滑动条值变化事件回调函数
static void slider_brightness_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // 获取滑动条当前值 (0-100)
        int32_t slider_value = lv_slider_get_value(slider);

        // 设置屏幕亮度
        esp_err_t ret = bsp_display_brightness_set(slider_value);
        if (ret == ESP_OK)
        {
            ESP_LOGI(TAG, "屏幕亮度设置为: %ld%%", slider_value);
        }
        else
        {
            ESP_LOGE(TAG, "亮度设置失败: %s", esp_err_to_name(ret));
        }
    }
}

void events_init(lv_ui *ui)
{
    // 为滑动条添加值变化事件监听
    if (ui->screen_slider_1 != NULL)
    {
        lv_obj_add_event_cb(ui->screen_slider_1, slider_brightness_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
        ESP_LOGI(TAG, "滑动条亮度控制事件已注册");

        // 设置滑动条初始值并触发一次亮度设置
        int32_t initial_value = lv_slider_get_value(ui->screen_slider_1);
        bsp_display_brightness_set(initial_value);
        ESP_LOGI(TAG, "初始亮度设置为: %ld%%", initial_value);
    }
    else
    {
        ESP_LOGW(TAG, "滑动条对象不存在，无法注册事件");
    }
}
