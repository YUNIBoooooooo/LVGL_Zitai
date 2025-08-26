#ifndef LCD_H
#define LCD_H

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "i2c_pca9557.h"
#include "events_init.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_ft5x06.h"

#define LCD_HOST SPI2_HOST
#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 40
#define PIN_NUM_CLK 41
#define PIN_NUM_CS -1
#define PIN_NUM_DC 39
#define PIN_NUM_RST GPIO_NUM_NC
#define PIN_NUM_BK_LIGHT 42
#define LCD_H_RES 320
#define LCD_V_RES 240
#define LCD_LEDC_CH LEDC_CHANNEL_1
#define BSP_LCD_BACKLIGHT PIN_NUM_BK_LIGHT

#define PCA9557_OUTPUT_PORT 0x01
#define PCA9557_CONFIGURATION_PORT 0x03
#define LCD_CS_GPIO BIT(0)
#define PA_EN_GPIO BIT(1)

#define FT6X36_I2C_ADDR 0x38
#define PCA9557_SENSOR_ADDR 0x19

// 导出给外部使用的函数
esp_lcd_panel_handle_t lcd_init(void);
esp_err_t bsp_display_backlight_on(void);
esp_err_t bsp_display_brightness_set(int brightness_percent); // 导出亮度设置函数
esp_lcd_touch_handle_t touch_init(void);

#endif // !LCD_H