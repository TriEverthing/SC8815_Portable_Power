/*
* Copyright 2025 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
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



void setup_scr_ST7789V3(lv_ui *ui)
{
	//Write codes ST7789V3
	ui->ST7789V3 = lv_obj_create(NULL);
	lv_obj_set_size(ui->ST7789V3, 320, 172);
	lv_obj_set_scrollbar_mode(ui->ST7789V3, LV_SCROLLBAR_MODE_OFF);

	//Write style for ST7789V3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->ST7789V3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_OutputPower_Lable
	ui->ST7789V3_OutputPower_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_OutputPower_Lable, "0.0000");
	lv_label_set_long_mode(ui->ST7789V3_OutputPower_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_OutputPower_Lable, 139, 125);
	lv_obj_set_size(ui->ST7789V3_OutputPower_Lable, 180, 46);

	//Write style for ST7789V3_OutputPower_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_OutputPower_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_OutputPower_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_OutputPower_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_OutputPower_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_OutputPower_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_OutputPower_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_OutputPower_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_OutputPower_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_OutputPower_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_OutputPower_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_OutputPower_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_OutputPower_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_OutputPower_Lable, lv_color_hex(0xff00db), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_OutputPower_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_OutputPower_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_OutputPower_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_OutputPower_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_OutputPower_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_OutputPower_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_PowerUnit_Lable
	ui->ST7789V3_PowerUnit_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_PowerUnit_Lable, "W");
	lv_label_set_long_mode(ui->ST7789V3_PowerUnit_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_PowerUnit_Lable, 290, 126);
	lv_obj_set_size(ui->ST7789V3_PowerUnit_Lable, 24, 46);

	//Write style for ST7789V3_PowerUnit_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_PowerUnit_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_PowerUnit_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_PowerUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_PowerUnit_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_PowerUnit_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_PowerUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_OutputCurrent_Lable
	ui->ST7789V3_OutputCurrent_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_OutputCurrent_Lable, "0.0000");
	lv_label_set_long_mode(ui->ST7789V3_OutputCurrent_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_OutputCurrent_Lable, 139, 79);
	lv_obj_set_size(ui->ST7789V3_OutputCurrent_Lable, 180, 46);

	//Write style for ST7789V3_OutputCurrent_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_OutputCurrent_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_OutputCurrent_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_OutputCurrent_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_OutputCurrent_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_OutputCurrent_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_OutputCurrent_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_OutputCurrent_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_OutputCurrent_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_OutputCurrent_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_OutputCurrent_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_OutputCurrent_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_OutputCurrent_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_OutputCurrent_Lable, lv_color_hex(0xff0027), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_OutputCurrent_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_OutputCurrent_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_OutputCurrent_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_OutputCurrent_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_OutputCurrent_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_OutputCurrent_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_CurrentUnit_Lable
	ui->ST7789V3_CurrentUnit_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_CurrentUnit_Lable, "A");
	lv_label_set_long_mode(ui->ST7789V3_CurrentUnit_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_CurrentUnit_Lable, 290, 79);
	lv_obj_set_size(ui->ST7789V3_CurrentUnit_Lable, 24, 46);

	//Write style for ST7789V3_CurrentUnit_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_CurrentUnit_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_CurrentUnit_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_CurrentUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_CurrentUnit_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_CurrentUnit_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_CurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_OutputVlotage_Lable
	ui->ST7789V3_OutputVlotage_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_OutputVlotage_Lable, "0.0000");
	lv_label_set_long_mode(ui->ST7789V3_OutputVlotage_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_OutputVlotage_Lable, 139, 33);
	lv_obj_set_size(ui->ST7789V3_OutputVlotage_Lable, 180, 46);

	//Write style for ST7789V3_OutputVlotage_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_OutputVlotage_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_OutputVlotage_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_OutputVlotage_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_OutputVlotage_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_OutputVlotage_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_OutputVlotage_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_OutputVlotage_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_OutputVlotage_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_OutputVlotage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_OutputVlotage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_OutputVlotage_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_OutputVlotage_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_OutputVlotage_Lable, lv_color_hex(0x00a8ff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_OutputVlotage_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_OutputVlotage_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_OutputVlotage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_OutputVlotage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_OutputVlotage_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_OutputVlotage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_VoltageUnit_Lable
	ui->ST7789V3_VoltageUnit_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_VoltageUnit_Lable, "V");
	lv_label_set_long_mode(ui->ST7789V3_VoltageUnit_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_VoltageUnit_Lable, 290, 34);
	lv_obj_set_size(ui->ST7789V3_VoltageUnit_Lable, 24, 46);

	//Write style for ST7789V3_VoltageUnit_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_VoltageUnit_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_VoltageUnit_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_VoltageUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_VoltageUnit_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_VoltageUnit_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_VoltageUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes ST7789V3_Status_Lable1
    ui->ST7789V3_Status_Lable1 = lv_label_create(ui->ST7789V3);
    lv_label_set_text(ui->ST7789V3_Status_Lable1, "ON");
    lv_label_set_long_mode(ui->ST7789V3_Status_Lable1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->ST7789V3_Status_Lable1, 0, 0);
    lv_obj_set_size(ui->ST7789V3_Status_Lable1, 320, 32);

    //Write style for ST7789V3_Status_Lable1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->ST7789V3_Status_Lable1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->ST7789V3_Status_Lable1, lv_color_hex(0x00e0ff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->ST7789V3_Status_Lable1, &lv_font_MonosB_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->ST7789V3_Status_Lable1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->ST7789V3_Status_Lable1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->ST7789V3_Status_Lable1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->ST7789V3_Status_Lable1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->ST7789V3_Status_Lable1, lv_color_hex(0xc8c9c9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->ST7789V3_Status_Lable1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->ST7789V3_Status_Lable1, 64, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->ST7789V3_Status_Lable1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_CV_Lable
	ui->ST7789V3_CV_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_CV_Lable, "CV");
	lv_label_set_long_mode(ui->ST7789V3_CV_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_CV_Lable, 0, 125);
	lv_obj_set_size(ui->ST7789V3_CV_Lable, 47, 25);

	//Write style for ST7789V3_CV_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_CV_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_CV_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_CV_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_CV_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_CV_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_CV_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_CV_Lable, &lv_font_MonosB_22, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_CV_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_CV_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_CV_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_CV_Lable, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_CV_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_CV_Lable, lv_color_hex(0xadaca8), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_CV_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_CV_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_CV_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_CV_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_CV_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_CV_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_CC_Lable
	ui->ST7789V3_CC_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_CC_Lable, "CC");
	lv_label_set_long_mode(ui->ST7789V3_CC_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_CC_Lable, 46, 125);
	lv_obj_set_size(ui->ST7789V3_CC_Lable, 48, 25);

	//Write style for ST7789V3_CC_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_CC_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_CC_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_CC_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_CC_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_CC_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_CC_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_CC_Lable, &lv_font_MonosB_22, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_CC_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_CC_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_CC_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_CC_Lable, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_CC_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_CC_Lable, lv_color_hex(0xadaca8), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_CC_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_CC_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_CC_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_CC_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_CC_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_CC_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_OCP_Lable
	ui->ST7789V3_OCP_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_OCP_Lable, "OCP");
	lv_label_set_long_mode(ui->ST7789V3_OCP_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_OCP_Lable, 93, 125);
	lv_obj_set_size(ui->ST7789V3_OCP_Lable, 47, 25);

	//Write style for ST7789V3_OCP_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_OCP_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_OCP_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_OCP_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_OCP_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_OCP_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_OCP_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_OCP_Lable, &lv_font_MonosB_22, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_OCP_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_OCP_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_OCP_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_OCP_Lable, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_OCP_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_OCP_Lable, lv_color_hex(0xadaca8), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_OCP_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_OCP_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_OCP_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_OCP_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_OCP_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_OCP_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_IPV4_Lable
	ui->ST7789V3_IPV4_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_IPV4_Lable, "00:00:00:00");
	lv_label_set_long_mode(ui->ST7789V3_IPV4_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_IPV4_Lable, 0, 149);
	lv_obj_set_size(ui->ST7789V3_IPV4_Lable, 140, 22);

	//Write style for ST7789V3_IPV4_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_IPV4_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->ST7789V3_IPV4_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->ST7789V3_IPV4_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->ST7789V3_IPV4_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_IPV4_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_IPV4_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_IPV4_Lable, &lv_font_MonosB_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_IPV4_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_IPV4_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_IPV4_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_IPV4_Lable, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_IPV4_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->ST7789V3_IPV4_Lable, lv_color_hex(0xadaca8), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->ST7789V3_IPV4_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_IPV4_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_IPV4_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_IPV4_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_IPV4_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_IPV4_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_InputVoltage_Lable
	ui->ST7789V3_InputVoltage_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_InputVoltage_Lable, "VIN:12.13V");
	lv_label_set_long_mode(ui->ST7789V3_InputVoltage_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_InputVoltage_Lable, 212, 2);
	lv_obj_set_size(ui->ST7789V3_InputVoltage_Lable, 102, 16);

	//Write style for ST7789V3_InputVoltage_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_InputVoltage_Lable, lv_color_hex(0xff00cd), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_InputVoltage_Lable, &lv_font_MonosB_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_InputVoltage_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_InputVoltage_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_InputVoltage_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_Temp_Lable
	ui->ST7789V3_Temp_Lable = lv_label_create(ui->ST7789V3);
	lv_label_set_text(ui->ST7789V3_Temp_Lable, "NTC:25.6");
	lv_label_set_long_mode(ui->ST7789V3_Temp_Lable, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->ST7789V3_Temp_Lable, 212, 16);
	lv_obj_set_size(ui->ST7789V3_Temp_Lable, 102, 18);

	//Write style for ST7789V3_Temp_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->ST7789V3_Temp_Lable, lv_color_hex(0xff00cd), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->ST7789V3_Temp_Lable, &lv_font_MonosB_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->ST7789V3_Temp_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->ST7789V3_Temp_Lable, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->ST7789V3_Temp_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_SetVolatgeUnit_Lable
    ui->ST7789V3_SetVolatgeUnit_Lable = lv_label_create(ui->ST7789V3);
    lv_label_set_text(ui->ST7789V3_SetVolatgeUnit_Lable, "V");
    lv_label_set_long_mode(ui->ST7789V3_SetVolatgeUnit_Lable, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->ST7789V3_SetVolatgeUnit_Lable, 0, 33);
    lv_obj_set_size(ui->ST7789V3_SetVolatgeUnit_Lable, 140, 46);

    //Write style for ST7789V3_SetVolatgeUnit_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->ST7789V3_SetVolatgeUnit_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->ST7789V3_SetVolatgeUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->ST7789V3_SetVolatgeUnit_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->ST7789V3_SetVolatgeUnit_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->ST7789V3_SetVolatgeUnit_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->ST7789V3_SetVolatgeUnit_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->ST7789V3_SetVolatgeUnit_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->ST7789V3_SetVolatgeUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->ST7789V3_SetVolatgeUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->ST7789V3_SetVolatgeUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->ST7789V3_SetVolatgeUnit_Lable, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->ST7789V3_SetVolatgeUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->ST7789V3_SetVolatgeUnit_Lable, lv_color_hex(0x00ffbd), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->ST7789V3_SetVolatgeUnit_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->ST7789V3_SetVolatgeUnit_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->ST7789V3_SetVolatgeUnit_Lable, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->ST7789V3_SetVolatgeUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->ST7789V3_SetVolatgeUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->ST7789V3_SetVolatgeUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_SetVoltageSpinbox
    ui->ST7789V3_SetVoltageSpinbox = lv_spinbox_create(ui->ST7789V3);
	lv_obj_set_pos( ui->ST7789V3_SetVoltageSpinbox , 4 , 40 );
	lv_obj_set_size( ui->ST7789V3_SetVoltageSpinbox , 106 , 32 );
    lv_spinbox_set_digit_format(ui->ST7789V3_SetVoltageSpinbox, 4, 2);
    lv_spinbox_set_range(ui->ST7789V3_SetVoltageSpinbox, 0, 3200 );
    lv_spinbox_set_cursor_pos(ui->ST7789V3_SetVoltageSpinbox, 1 - 1);
	lv_spinbox_set_value(ui->ST7789V3_SetVoltageSpinbox , 0 );
	lv_spinbox_set_rollover(ui->ST7789V3_SetVoltageSpinbox,true);
	//Write style for ST7789V3_SetVoltageSpinbox, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_SetVoltageSpinbox,0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->ST7789V3_SetVoltageSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->ST7789V3_SetVoltageSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->ST7789V3_SetVoltageSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->ST7789V3_SetVoltageSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->ST7789V3_SetVoltageSpinbox, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->ST7789V3_SetVoltageSpinbox, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->ST7789V3_SetVoltageSpinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->ST7789V3_SetVoltageSpinbox, &lv_font_MonosB_32, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_SetVoltageSpinbox,1,LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_SetVoltageSpinbox, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->ST7789V3_SetVoltageSpinbox, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->ST7789V3_SetVoltageSpinbox, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for ST7789V3_SetVoltageSpinbox, Part: LV_PART_CURSOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->ST7789V3_SetVoltageSpinbox, 0, LV_PART_CURSOR|LV_STATE_DEFAULT);
	//Write style for ST7789V3_SetVoltageSpinbox, Part: LV_PART_CURSOR, State: LV_STATE_EDITED.
    lv_obj_set_style_text_color(ui->ST7789V3_SetVoltageSpinbox, lv_color_hex(0xffe500), LV_PART_CURSOR|LV_STATE_EDITED);

    //Write codes ST7789V3_SetCurrentUnit_Lable
    ui->ST7789V3_SetCurrentUnit_Lable = lv_label_create(ui->ST7789V3);
    lv_label_set_text(ui->ST7789V3_SetCurrentUnit_Lable, "A");
    lv_label_set_long_mode(ui->ST7789V3_SetCurrentUnit_Lable, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->ST7789V3_SetCurrentUnit_Lable, 0, 79);
    lv_obj_set_size(ui->ST7789V3_SetCurrentUnit_Lable, 140, 46);

    //Write style for ST7789V3_SetCurrentUnit_Lable, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->ST7789V3_SetCurrentUnit_Lable, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->ST7789V3_SetCurrentUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->ST7789V3_SetCurrentUnit_Lable, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->ST7789V3_SetCurrentUnit_Lable, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->ST7789V3_SetCurrentUnit_Lable, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->ST7789V3_SetCurrentUnit_Lable, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->ST7789V3_SetCurrentUnit_Lable, &lv_font_MonosB_40, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->ST7789V3_SetCurrentUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->ST7789V3_SetCurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->ST7789V3_SetCurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->ST7789V3_SetCurrentUnit_Lable, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->ST7789V3_SetCurrentUnit_Lable, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->ST7789V3_SetCurrentUnit_Lable, lv_color_hex(0xffb100), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->ST7789V3_SetCurrentUnit_Lable, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->ST7789V3_SetCurrentUnit_Lable, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->ST7789V3_SetCurrentUnit_Lable, 3, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->ST7789V3_SetCurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->ST7789V3_SetCurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->ST7789V3_SetCurrentUnit_Lable, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes ST7789V3_SetCurrentSpinbox
	ui->ST7789V3_SetCurrentSpinbox = lv_spinbox_create(ui->ST7789V3);
	lv_obj_set_pos( ui->ST7789V3_SetCurrentSpinbox , 4 , 86 );
	lv_obj_set_size( ui->ST7789V3_SetCurrentSpinbox , 106 , 32 );
    lv_spinbox_set_digit_format(ui->ST7789V3_SetCurrentSpinbox, 4, 2);
    lv_spinbox_set_range(ui->ST7789V3_SetCurrentSpinbox, 0, 1000 );
    lv_spinbox_set_cursor_pos(ui->ST7789V3_SetCurrentSpinbox, 1 - 1);
	lv_spinbox_set_value(ui->ST7789V3_SetCurrentSpinbox , 0 );
	lv_spinbox_set_rollover(ui->ST7789V3_SetCurrentSpinbox,true);
	//Write style for ST7789V3_SetCurrentSpinbox, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->ST7789V3_SetCurrentSpinbox,0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->ST7789V3_SetCurrentSpinbox, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->ST7789V3_SetCurrentSpinbox, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->ST7789V3_SetCurrentSpinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->ST7789V3_SetCurrentSpinbox, &lv_font_MonosB_32, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->ST7789V3_SetCurrentSpinbox,1,LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->ST7789V3_SetCurrentSpinbox, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->ST7789V3_SetCurrentSpinbox, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->ST7789V3_SetCurrentSpinbox, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	//Write style for ST7789V3_SetCurrentSpinbox, Part: LV_PART_MAIN, State: LV_STATE_FOCUSED.
	lv_obj_set_style_pad_top(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_MAIN | LV_STATE_FOCUSED );

    //Write style for ST7789V3_SetCurrentSpinbox, Part: LV_PART_CURSOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->ST7789V3_SetCurrentSpinbox, 0, LV_PART_CURSOR|LV_STATE_DEFAULT);

	//Write style for ST7789V3_SetCurrentSpinbox, Part: LV_PART_CURSOR, State: LV_STATE_EDITED.
    lv_obj_set_style_text_color(ui->ST7789V3_SetCurrentSpinbox, lv_color_hex(0xffe500), LV_PART_CURSOR|LV_STATE_EDITED);

	//PowerSwitch
	ui->PowerSwitch = lv_switch_create(ui->ST7789V3);
	lv_obj_set_pos(ui->PowerSwitch, 7 , 4 );
    lv_obj_set_size(ui->PowerSwitch, 50 , 25 );

	//The custom code of ST7789V3.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->ST7789V3);

}
