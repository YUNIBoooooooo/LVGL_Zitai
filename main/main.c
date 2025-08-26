#include <stdio.h>
#include "mywifi.h"
#include "mylvgl.h"
#include "qmi8658.h"
#include "i2c_pca9557.h"
#include "lcd.h"

// 声明全局 GUI 变量
lv_ui guider_ui;

void app_main(void)
{
    // I2C初始化 - 必须先初始化，因为LCD依赖I2C
    bsp_i2c_init();

    // LVGL初始化（包含LCD和触摸屏初始化）
    lvgl_init();

    // 姿态传感器初始化
    qmi8658_init();

    // WIFI初始化
    wifi_init();

    // 按键初始化
    key_init();

    // 创建LVGL任务和传感器任务
    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 4096 * 2, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(qmi8658_task, "qmi8658_task", 4096, NULL, 4, NULL, 1);
}
