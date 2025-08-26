/*
 * Copyright 2025 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "qmi8658.h"

void setup_scr_screen(lv_ui *ui)
{
    // Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_cont_1
    ui->screen_cont_1 = lv_obj_create(ui->screen);
    lv_obj_set_pos(ui->screen_cont_1, 0, 0);
    lv_obj_set_size(ui->screen_cont_1, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen_cont_1, LV_SCROLLBAR_MODE_OFF);

    // Write style for screen_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_cont_1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_cont_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_cont_1, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_cont_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cont_1, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_slider_1
    ui->screen_slider_1 = lv_slider_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_slider_1, 39, 60);
    lv_obj_set_size(ui->screen_slider_1, 221, 10);
    lv_slider_set_range(ui->screen_slider_1, 0, 100);
    lv_slider_set_mode(ui->screen_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_slider_1, 50, LV_ANIM_OFF);

    // Write style for screen_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_slider_1, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_slider_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_slider_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write style for screen_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_slider_1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // Write style for screen_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_slider_1, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_slider_1, lv_color_hex(0x2195f6), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_slider_1, 50, LV_PART_KNOB | LV_STATE_DEFAULT);

    // Write codes screen_label_1
    ui->screen_label_1 = lv_label_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_label_1, 5, 33);
    lv_obj_set_size(ui->screen_label_1, 100, 32);
    lv_label_set_text(ui->screen_label_1, "亮度\n");
    lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);

    // Write style for screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_1, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_img_1
    ui->screen_img_1 = lv_image_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_img_1, 11, 135);
    lv_obj_set_size(ui->screen_img_1, 38, 36);
    lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_img_1, &_dat_RGB565A8_38x36);
    lv_image_set_pivot(ui->screen_img_1, 50, 50);
    lv_image_set_rotation(ui->screen_img_1, 0);

    // Write style for screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_img_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_img_1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_label_2
    ui->screen_label_2 = lv_label_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_label_2, 131, 95);
    lv_obj_set_size(ui->screen_label_2, 100, 32);
    lv_label_set_text(ui->screen_label_2, "未连接");
    lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);

    // Write style for screen_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_2, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_label_3
    ui->screen_label_3 = lv_label_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_label_3, 8, 95);
    lv_obj_set_size(ui->screen_label_3, 123, 32);
    lv_label_set_text(ui->screen_label_3, "Wi-Fi连接状态：\n");
    lv_label_set_long_mode(ui->screen_label_3, LV_LABEL_LONG_WRAP);

    // Write style for screen_label_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_3, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_label_sensor - 传感器数据显示
    ui->screen_label_sensor = lv_label_create(ui->screen_cont_1);
    lv_obj_set_pos(ui->screen_label_sensor, 18, 195);
    lv_obj_set_size(ui->screen_label_sensor, 300, 40);
    lv_label_set_text(ui->screen_label_sensor, "qmi8658: X:0.0 Y:0.0 Z:0.0");
    lv_label_set_long_mode(ui->screen_label_sensor, LV_LABEL_LONG_WRAP);

    // Write style for screen_label_sensor, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_label_sensor, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_label_sensor, &lv_font_SourceHanSerifSC_Regular_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_label_sensor, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_label_sensor, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_label_sensor, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Write codes screen_spangroup_1
    ui->screen_spangroup_1 = lv_spangroup_create(ui->screen_cont_1);
    lv_spangroup_set_align(ui->screen_spangroup_1, LV_TEXT_ALIGN_LEFT);
    lv_spangroup_set_overflow(ui->screen_spangroup_1, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(ui->screen_spangroup_1, LV_SPAN_MODE_BREAK);
    // create span
    ui->screen_spangroup_1_span = lv_spangroup_new_span(ui->screen_spangroup_1);
    lv_style_set_text_color(lv_span_get_style(ui->screen_spangroup_1_span), lv_color_hex(0x000000));
    lv_style_set_text_decor(lv_span_get_style(ui->screen_spangroup_1_span), LV_TEXT_DECOR_NONE);
    lv_style_set_text_font(lv_span_get_style(ui->screen_spangroup_1_span), &lv_font_montserratMedium_12);

    // Write style state: LV_STATE_DEFAULT for &style_screen_spangroup_1_main_main_default
    static lv_style_t style_screen_spangroup_1_main_main_default;
    ui_init_style(&style_screen_spangroup_1_main_main_default);

    lv_style_set_border_width(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_radius(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_bg_opa(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_pad_top(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_pad_right(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_pad_bottom(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_pad_left(&style_screen_spangroup_1_main_main_default, 0);
    lv_style_set_shadow_width(&style_screen_spangroup_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_spangroup_1, &style_screen_spangroup_1_main_main_default, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_spangroup_refr_mode(ui->screen_spangroup_1);

    // The custom code of screen.

    // Update current screen layout.
    lv_obj_update_layout(ui->screen);
}
