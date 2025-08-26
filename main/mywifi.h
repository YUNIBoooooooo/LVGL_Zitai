#ifndef MYWIFI_H
#define MYWIFI_H

#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"       // 事件处理
#include "esp_smartconfig.h" // 智能配网功能
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "driver/gpio.h"
#include "mylvgl.h"



void wifi_init(void);
void wifi_connect(void);
void key_init(void);

#endif // !MYWIFI_H