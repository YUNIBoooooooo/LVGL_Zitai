#include "mywifi.h"
#include "nvs.h"

#define KEY_GPIO GPIO_NUM_0 // 假设使用 GPIO 0 作为按键输入

// ======================== 事件位定义 ========================
static const int CONNECTED_BIT = BIT0;     // 位0：WiFi连接成功标志
static const int ESPTOUCH_DONE_BIT = BIT1; // 位1：ESP-Touch配网完成标志

// ======================== 全局变量 ========================
static EventGroupHandle_t s_wifi_event_group; // 事件组句柄，用于任务间通信
static const char *TAG = "mywifi";

// 配网状态管理
static bool app_nvs_get_prov_flag(void)
{
    nvs_handle_t h;
    uint8_t v = 0;
    if (nvs_open("app", NVS_READONLY, &h) == ESP_OK)
    {
        nvs_get_u8(h, "prov", &v);
        nvs_close(h);
    }
    return v == 1;
}

static void app_nvs_set_prov_flag(bool on)
{
    nvs_handle_t h;
    if (nvs_open("app", NVS_READWRITE, &h) == ESP_OK)
    {
        nvs_set_u8(h, "prov", on ? 1 : 0);
        nvs_commit(h);
        nvs_close(h);
    }
}

static void event_handler(void *arg,                   // 用户参数（未使用）
                          esp_event_base_t event_base, // 事件基础类型
                          int32_t event_id,            // 事件ID
                          void *event_data)            // 事件数据
{
    // 处理WiFi STA启动事件
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect(); // WiFi启动后立即尝试连接
        ESP_LOGI(TAG, "WiFi STA启动，开始连接...");
    }
    // 处理WiFi断开连接事件
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect(); // 断开后自动重连
        ESP_LOGI(TAG, "WiFi断开连接，正在重连...");
    }
    // 处理获取IP地址事件
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char ip[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, ip, sizeof(ip));
        ESP_LOGI(TAG, "WiFi连接成功！获取到IP地址: %s", ip);
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT); // 设置连接成功标志

        static char label_text[20] = "连接成功"; // 标签显示的简化文本
        // 更新传感器标签显示的文本
        if (guider_ui.screen_label_2 != NULL)
        {
            lv_label_set_text(guider_ui.screen_label_2, label_text);
        }
    }
    // 处理SmartConfig扫描完成事件
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG, "SmartConfig扫描完成");
    }
    // 处理SmartConfig找到信道事件
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(TAG, "SmartConfig找到目标信道");
    }
    // 处理SmartConfig获取SSID和密码事件
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(TAG, "SmartConfig获取到WiFi信息");

        // 获取SmartConfig传递的WiFi信息
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;                // WiFi配置结构体
        bzero(&wifi_config, sizeof(wifi_config)); // 清零配置结构体

        // 复制SSID和密码到WiFi配置中
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));

        ESP_LOGI(TAG, "SSID: %s", wifi_config.sta.ssid);
        ESP_LOGI(TAG, "密码: %s", wifi_config.sta.password);

        // 先断开当前连接，然后应用新的WiFi配置
        esp_wifi_disconnect();                          // 断开当前连接
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // 设置新的WiFi配置
        esp_wifi_connect();                             // 使用新配置连接WiFi
        app_nvs_set_prov_flag(true);                    // 标记已配网
    }
    // 处理SmartConfig发送确认完成事件
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        ESP_LOGI(TAG, "SmartConfig配网完成");
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT); // 设置配网完成标志
    }
}

void wifi_init(void)
{
    // 初始化NVS存储（仅初始化，不要每次擦整块）
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    ESP_LOGI(TAG, "NVS存储初始化完成");

    // 如果需要强制清除Wi-Fi配置，取消注释下面这行
    // ESP_ERROR_CHECK(esp_wifi_restore());

    // 第1步：初始化网络接口层
    ESP_ERROR_CHECK(esp_netif_init());

    // 第2步：创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 第3步：创建默认WiFi STA接口
    esp_netif_create_default_wifi_sta();

    // 第4步：初始化WiFi驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 关键：让Wi-Fi使用FLASH存储（驱动会在其内部命名空间保存参数）
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));

    // 第5步：创建事件组（用于任务间通信）
    s_wifi_event_group = xEventGroupCreate();

    // 第6步：注册事件处理函数
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    // 第7步：设置WiFi模式并启动
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (app_nvs_get_prov_flag())
    {
        ESP_LOGI(TAG, "已配网，尝试连接保存的网络");
    }
    else
    {
        ESP_LOGI(TAG, "未配网，等待按键启动SmartConfig");
    }

    ESP_LOGI(TAG, "WiFi系统初始化完成");
}

static void smartconfig_task(void *parm) // 任务参数（未使用）
{
    EventBits_t uxBits; // 用于接收事件位的变量

    ESP_LOGI(TAG, "启动SmartConfig配网任务");

    // 配置SmartConfig参数
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT(); // 使用默认配置
    esp_smartconfig_set_type(SC_TYPE_ESPTOUCH);                          // 设置配网类型为ESP-Touch
    esp_smartconfig_start(&cfg);                                         // 启动SmartConfig

    ESP_LOGI(TAG, "请使用ESP-Touch手机APP进行配网");
    ESP_LOGI(TAG, "等待手机发送WiFi配置信息...");

    // 主循环：等待事件
    while (1)
    {
        // 等待事件位：WiFi连接成功 或 ESP-Touch配网完成
        uxBits = xEventGroupWaitBits(s_wifi_event_group,                // 事件组句柄
                                     CONNECTED_BIT | ESPTOUCH_DONE_BIT, // 等待的事件位
                                     true,                              // 读取后清除事件位
                                     false,                             // 只要有一个事件位被设置就返回
                                     portMAX_DELAY);                    // 无限等待

        // 检查WiFi是否连接成功
        if (uxBits & CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "WiFi连接成功！");
        }

        // 检查ESP-Touch配网是否完成
        if (uxBits & ESPTOUCH_DONE_BIT)
        {
            ESP_LOGI(TAG, "ESP-Touch配网流程完成");

            esp_smartconfig_stop(); // 停止SmartConfig
            ESP_LOGI(TAG, "SmartConfig已停止");
            vTaskDelete(NULL); // 删除当前任务（自我删除）
        }
    }
}

// 按键中断处理函数（ISR安全版本）
static volatile bool start_smartconfig = false;

void IRAM_ATTR key_isr_handler(void *arg)
{
    start_smartconfig = true; // 设置标志，在主任务中处理
}

// 监控任务，处理按键事件
static void key_monitor_task(void *pvParameters)
{
    while (1)
    {
        if (start_smartconfig)
        {
            start_smartconfig = false;
            ESP_LOGI(TAG, "按键触发，启动SmartConfig配网");
            xTaskCreatePinnedToCore(smartconfig_task, "smartconfig_task", 4096, NULL, 5, NULL, 0);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void key_init()
{
    // 初始化按键
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,      // 下降沿触发中断
        .mode = GPIO_MODE_INPUT,             // 设置为输入模式
        .pin_bit_mask = (1ULL << KEY_GPIO),  // 设置输入引脚
        .pull_down_en = GPIO_PULLUP_DISABLE, // 禁用下拉
        .pull_up_en = GPIO_PULLUP_DISABLE    // 禁用上拉
    };

    gpio_config(&io_conf);

    gpio_install_isr_service(0);                           // 安装 GPIO 中断服务
    gpio_isr_handler_add(KEY_GPIO, key_isr_handler, NULL); // 添加中断处理函数

    // 创建按键监控任务
    xTaskCreate(key_monitor_task, "key_monitor", 2048, NULL, 3, NULL);
}