/**
 * @file esp_lcd_touch.c
 * @brief 最简单的触摸屏集成代码示例
 */

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_lcd_touch.h"

static const char *TAG = "TOUCH";

/**
 * @brief LVGL触摸回调 - 复制到mylvgl.c
 */
#if 0 // 删除这行和endif使用
void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    uint16_t x, y;
    uint8_t touch_cnt = 0;
    esp_lcd_touch_handle_t tp = lv_indev_get_user_data(indev);
    bool touched = esp_lcd_touch_get_coordinates(tp, &x, &y, NULL, &touch_cnt, 1);
    
    if (touched && touch_cnt > 0) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

/*******************************************************************************
 * Public API functions - 公共API函数实现
 *******************************************************************************/
/**
 * @brief 让触摸屏进入睡眠模式
 * @param tp 触摸屏句柄
 * @return ESP_OK: 成功, ESP_FAIL: 失败或不支持
 */
esp_err_t esp_lcd_touch_enter_sleep(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);
    if (tp->enter_sleep == NULL)
    {
        ESP_LOGE(TAG, "Sleep mode not supported!"); // 该触摸屏不支持睡眠模式
        return ESP_FAIL;
    }
    else
    {
        return tp->enter_sleep(tp); // 调用具体驱动的睡眠函数
    }
}

/**
 * @brief 让触摸屏退出睡眠模式（唤醒）
 * @param tp 触摸屏句柄
 * @return ESP_OK: 成功, ESP_FAIL: 失败或不支持
 */
esp_err_t esp_lcd_touch_exit_sleep(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);
    if (tp->exit_sleep == NULL)
    {
        ESP_LOGE(TAG, "Sleep mode not supported!"); // 该触摸屏不支持睡眠模式
        return ESP_FAIL;
    }
    else
    {
        return tp->exit_sleep(tp); // 调用具体驱动的唤醒函数
    }
}

/**
 * @brief 从触摸屏读取原始触摸数据
 * @param tp 触摸屏句柄
 * @return ESP_OK: 成功读取, 其他: 读取失败
 * @note 这个函数只是读取数据，不进行坐标处理。需要调用get_coordinates获取处理后的坐标
 */
esp_err_t esp_lcd_touch_read_data(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);
    assert(tp->read_data != NULL); // 确保具体驱动实现了读取函数

    return tp->read_data(tp); // 调用具体驱动的数据读取函数
}

/**
 * @brief 获取触摸坐标并进行坐标变换处理
 * @param tp 触摸屏句柄
 * @param x 返回的X坐标数组指针
 * @param y 返回的Y坐标数组指针
 * @param strength 返回的触摸压力值数组指针（可选）
 * @param point_num 返回的实际触摸点数量
 * @param max_point_num 最大支持的触摸点数量
 * @return true: 检测到触摸, false: 无触摸
 *
 * 这是触摸屏驱动的核心函数，执行以下处理流程：
 * 1. 调用具体驱动获取原始坐标
 * 2. 执行用户自定义坐标处理（如果有）
 * 3. 执行软件坐标变换（镜像、旋转、交换XY等）
 */
bool esp_lcd_touch_get_coordinates(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    bool touched = false;

    assert(tp != NULL);
    assert(x != NULL);
    assert(y != NULL);
    assert(tp->get_xy != NULL); // 确保具体驱动实现了坐标获取函数

    // 调用具体驱动获取原始触摸坐标
    touched = tp->get_xy(tp, x, y, strength, point_num, max_point_num);
    if (!touched)
    {
        return false; // 没有检测到触摸
    }

    /* 执行用户自定义的坐标处理函数（如果配置了） */
    if (tp->config.process_coordinates != NULL)
    {
        tp->config.process_coordinates(tp, x, y, strength, point_num, max_point_num);
    }

    /* 检查是否需要软件坐标调整
     * 当硬件不支持某些变换时，使用软件实现 */
    bool sw_adj_needed = ((tp->config.flags.mirror_x && (tp->set_mirror_x == NULL)) ||
                          (tp->config.flags.mirror_y && (tp->set_mirror_y == NULL)) ||
                          (tp->config.flags.swap_xy && (tp->set_swap_xy == NULL)));

    /* 对所有触摸点进行坐标调整 */
    for (int i = 0; (sw_adj_needed && i < *point_num); i++)
    {

        /* X轴镜像（如果硬件不支持则用软件实现） */
        if (tp->config.flags.mirror_x && tp->set_mirror_x == NULL)
        {
            x[i] = tp->config.x_max - x[i]; // 水平翻转
        }

        /* Y轴镜像（如果硬件不支持则用软件实现） */
        if (tp->config.flags.mirror_y && tp->set_mirror_y == NULL)
        {
            y[i] = tp->config.y_max - y[i]; // 垂直翻转
        }

        /* 交换X和Y坐标（如果硬件不支持则用软件实现）
         * 用于屏幕旋转90度或270度的情况 */
        if (tp->config.flags.swap_xy && tp->set_swap_xy == NULL)
        {
            uint16_t tmp = x[i];
            x[i] = y[i];
            y[i] = tmp;
        }
    }

    return touched; // 返回是否检测到有效触摸
}

#if (CONFIG_ESP_LCD_TOUCH_MAX_BUTTONS > 0)
/**
 * @brief 获取触摸按键状态
 * @param tp 触摸屏句柄
 * @param n 按键编号
 * @param state 返回的按键状态（按下/释放）
 * @return ESP_OK: 成功, ESP_ERR_NOT_SUPPORTED: 不支持按键功能
 *
 * 某些触摸屏控制器支持虚拟按键功能，可以在屏幕特定区域定义按键
 */
esp_err_t esp_lcd_touch_get_button_state(esp_lcd_touch_handle_t tp, uint8_t n, uint8_t *state)
{
    assert(tp != NULL);
    assert(state != NULL);

    *state = 0; // 默认状态为未按下

    if (tp->get_button_state)
    {
        return tp->get_button_state(tp, n, state); // 调用具体驱动的按键检测函数
    }
    else
    {
        return ESP_ERR_NOT_SUPPORTED; // 该触摸屏不支持按键功能
    }

    return ESP_OK;
}
#endif

/**
 * @brief 设置XY坐标交换
 * @param tp 触摸屏句柄
 * @param swap true: 交换XY坐标, false: 不交换
 * @return ESP_OK: 设置成功
 *
 * 用于屏幕旋转90度或270度时，将触摸的X、Y坐标互换
 * 如果硬件支持，优先使用硬件实现；否则使用软件实现
 */
esp_err_t esp_lcd_touch_set_swap_xy(esp_lcd_touch_handle_t tp, bool swap)
{
    assert(tp != NULL);

    tp->config.flags.swap_xy = swap; // 更新配置标志

    /* 如果硬件支持XY交换，调用硬件实现 */
    if (tp->set_swap_xy)
    {
        return tp->set_swap_xy(tp, swap);
    }

    return ESP_OK; // 硬件不支持时，将在get_coordinates中用软件实现
}

/**
 * @brief 获取当前XY坐标交换设置
 * @param tp 触摸屏句柄
 * @param swap 返回当前的交换设置状态
 * @return ESP_OK: 获取成功
 */
esp_err_t esp_lcd_touch_get_swap_xy(esp_lcd_touch_handle_t tp, bool *swap)
{
    assert(tp != NULL);
    assert(swap != NULL);

    /* 如果硬件支持获取交换状态，调用硬件实现 */
    if (tp->get_swap_xy)
    {
        return tp->get_swap_xy(tp, swap);
    }
    else
    {
        *swap = tp->config.flags.swap_xy; // 返回软件配置的状态
    }

    return ESP_OK;
}

/**
 * @brief 设置X轴镜像（水平翻转）
 * @param tp 触摸屏句柄
 * @param mirror true: 启用X轴镜像, false: 禁用
 * @return ESP_OK: 设置成功
 *
 * X轴镜像会将触摸的X坐标进行水平翻转：x_new = x_max - x_old
 * 常用于屏幕水平翻转180度的情况
 */
esp_err_t esp_lcd_touch_set_mirror_x(esp_lcd_touch_handle_t tp, bool mirror)
{
    assert(tp != NULL);

    tp->config.flags.mirror_x = mirror; // 更新配置标志

    /* 如果硬件支持X轴镜像，调用硬件实现 */
    if (tp->set_mirror_x)
    {
        return tp->set_mirror_x(tp, mirror);
    }

    return ESP_OK; // 硬件不支持时，将在get_coordinates中用软件实现
}

/**
 * @brief 获取当前X轴镜像设置
 * @param tp 触摸屏句柄
 * @param mirror 返回当前的X轴镜像设置状态
 * @return ESP_OK: 获取成功
 */
esp_err_t esp_lcd_touch_get_mirror_x(esp_lcd_touch_handle_t tp, bool *mirror)
{
    assert(tp != NULL);
    assert(mirror != NULL);

    /* 如果硬件支持获取镜像状态，调用硬件实现 */
    if (tp->get_mirror_x)
    {
        return tp->get_mirror_x(tp, mirror);
    }
    else
    {
        *mirror = tp->config.flags.mirror_x; // 返回软件配置的状态
    }

    return ESP_OK;
}

/**
 * @brief 设置Y轴镜像（垂直翻转）
 * @param tp 触摸屏句柄
 * @param mirror true: 启用Y轴镜像, false: 禁用
 * @return ESP_OK: 设置成功
 *
 * Y轴镜像会将触摸的Y坐标进行垂直翻转：y_new = y_max - y_old
 * 常用于屏幕垂直翻转180度的情况
 */
esp_err_t esp_lcd_touch_set_mirror_y(esp_lcd_touch_handle_t tp, bool mirror)
{
    assert(tp != NULL);

    tp->config.flags.mirror_y = mirror; // 更新配置标志

    /* 如果硬件支持Y轴镜像，调用硬件实现 */
    if (tp->set_mirror_y)
    {
        return tp->set_mirror_y(tp, mirror);
    }

    return ESP_OK; // 硬件不支持时，将在get_coordinates中用软件实现
}

/**
 * @brief 获取当前Y轴镜像设置
 * @param tp 触摸屏句柄
 * @param mirror 返回当前的Y轴镜像设置状态
 * @return ESP_OK: 获取成功
 */
esp_err_t esp_lcd_touch_get_mirror_y(esp_lcd_touch_handle_t tp, bool *mirror)
{
    assert(tp != NULL);
    assert(mirror != NULL);

    /* 如果硬件支持获取镜像状态，调用硬件实现 */
    if (tp->get_mirror_y)
    {
        return tp->get_mirror_y(tp, mirror);
    }
    else
    {
        *mirror = tp->config.flags.mirror_y; // 返回软件配置的状态
    }

    return ESP_OK;
}

/**
 * @brief 删除触摸屏驱动实例，释放相关资源
 * @param tp 触摸屏句柄
 * @return ESP_OK: 删除成功
 *
 * 调用具体驱动的删除函数，释放内存、注销中断等清理工作
 */
esp_err_t esp_lcd_touch_del(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    if (tp->del != NULL)
    {
        return tp->del(tp); // 调用具体驱动的删除函数
    }

    return ESP_OK;
}

/**
 * @brief 注册触摸屏中断回调函数
 * @param tp 触摸屏句柄
 * @param callback 中断回调函数指针
 * @return ESP_OK: 注册成功, ESP_ERR_INVALID_ARG: 无效参数, 其他: GPIO配置失败
 *
 * 当触摸屏检测到触摸事件时，会通过GPIO中断触发回调函数
 * 这样可以避免轮询检测，提高响应速度和降低功耗
 */
esp_err_t esp_lcd_touch_register_interrupt_callback(esp_lcd_touch_handle_t tp, esp_lcd_touch_interrupt_callback_t callback)
{
    esp_err_t ret = ESP_OK;
    assert(tp != NULL);

    /* 检查是否配置了中断引脚 */
    if (tp->config.int_gpio_num == GPIO_NUM_NC)
    {
        return ESP_ERR_INVALID_ARG; // 未配置中断引脚
    }

    tp->config.interrupt_callback = callback; // 保存回调函数

    if (callback != NULL)
    {
        /* 安装GPIO中断服务（如果尚未安装） */
        ret = gpio_install_isr_service(0);
        /* 如果用户之前已经安装了ISR服务，返回invalid state是正常的 */
        if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "GPIO ISR install failed");
            return ret;
        }
        /* 启用GPIO中断并添加中断处理函数 */
        ret = gpio_intr_enable(tp->config.int_gpio_num);
        ESP_RETURN_ON_ERROR(ret, TAG, "GPIO ISR install failed");
        ret = gpio_isr_handler_add(tp->config.int_gpio_num, (gpio_isr_t)tp->config.interrupt_callback, tp);
        ESP_RETURN_ON_ERROR(ret, TAG, "GPIO ISR install failed");
    }
    else
    {
        /* 移除GPIO中断处理函数 */
        ret = gpio_isr_handler_remove(tp->config.int_gpio_num);
        ESP_RETURN_ON_ERROR(ret, TAG, "GPIO ISR remove handler failed");
        ret = gpio_intr_disable(tp->config.int_gpio_num);
        ESP_RETURN_ON_ERROR(ret, TAG, "GPIO ISR disable failed");
    }

    return ESP_OK;
}

/**
 * @brief 注册带用户数据的触摸屏中断回调函数
 * @param tp 触摸屏句柄
 * @param callback 中断回调函数指针
 * @param user_data 用户自定义数据指针，会传递给回调函数
 * @return ESP_OK: 注册成功, 其他: 注册失败
 *
 * 相比普通的回调注册，这个函数允许传递额外的用户数据
 */
esp_err_t esp_lcd_touch_register_interrupt_callback_with_data(esp_lcd_touch_handle_t tp, esp_lcd_touch_interrupt_callback_t callback, void *user_data)
{
    assert(tp != NULL);

    tp->config.user_data = user_data;                               // 保存用户数据
    return esp_lcd_touch_register_interrupt_callback(tp, callback); // 调用普通的回调注册函数
}
