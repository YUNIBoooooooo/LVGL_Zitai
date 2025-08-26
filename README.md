# LVGL_Zitai (ESP32-S3 + ST7789 + FT6x36)

基于 ESP-IDF 的 ESP32-S3 项目，集成 LVGL 图形界面、ST7789 LCD、FT6x36 触摸、QMI8658 姿态传感器与 Wi‑Fi（SmartConfig）。已将滑动条与屏幕亮度联动，可通过滑动条实时调节背光亮度。

## 硬件与外设
- SoC: ESP32‑S3
- LCD: ST7789 320x240（SPI）
- 触摸: FT6x36（I2C, 0x38）
- 传感器: QMI8658（I2C）
- I/O 扩展: PCA9557（I2C）
- 背光: LEDC PWM，GPIO 42（反相输出）

主要引脚（见 `main/lcd.h`）：
- MOSI: IO40, SCLK: IO41, DC: IO39, CS: -1, RST: NC, BLK: IO42

## 主要功能
- LVGL 显示与双缓冲刷新（RGB565）
- 触摸读数与坐标映射；在 `touchpad_read` 中做了 Y 轴手动偏移修正
- 滑动条联动背光亮度（`generated/events_init.c`）
- QMI8658 姿态数据显示（`update_sensor_display`）
- Wi‑Fi STA 与 SmartConfig 配网（`main/mywifi.c`）

## 目录结构
- `main/` 核心源码（`lcd.c/h`, `mylvgl.c/h`, `mywifi.c/h`, `qmi8658.c/h`）
- `generated/` GUI Guider 生成的界面文件
- `custom/` 自定义配置（`lv_conf_ext.h` 等）