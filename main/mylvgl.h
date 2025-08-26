#ifndef MYLVGL_H
#define MYLVGL_H

#include "lvgl.h"
#include <stdio.h>
#include <string.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../generated/gui_guider.h"
#include "../generated/events_init.h"
#include "lcd.h"

#define LCD_H_RES 320
#define LCD_V_RES 240

// 声明全局 GUI 变量
extern lv_ui guider_ui;

// 函数声明
void lvgl_init(void);
esp_lcd_panel_handle_t lcd_init(void); // 在 lcd.c 中实现
void lvgl_task(void *pvParameter);
void update_sensor_display(void); // 更新传感器数据显示

#endif // !MYLVGL_H