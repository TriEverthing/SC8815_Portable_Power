/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-07-07 10:12:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-07-08 20:41:52
 * @FilePath: \rda5820_fm_v2\components\RotaryEncoder\RotaryEncoder.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * RotaryEncoder.c
 *
 *  Created on: Jun 8, 2024
 *      Author: bluerose
 */
#include "rotaryencoder.h"
#include <driver/gpio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_pm.h"
#include "iot_knob.h"
#include "esp_sleep.h"
#include "iot_button.h"
#include "esp_idf_version.h"
#include "button_gpio.h"
//lvgl
#include "lvgl.h"
//nxp guider
#include <gui_guider.h>
#include <events_init.h>

extern lv_ui guider_ui ;

uint16_t keypad_val = 0 ;
uint16_t encoder_val = 0 ;
int16_t knob_diff = 0 ;


#define ENCODER_PA_PIN          GPIO_NUM_21
#define ENCODER_PB_PIN          GPIO_NUM_14
#define ENCODER_SW_PIN          GPIO_NUM_17

#define BUTTON_K0_PIN           GPIO_NUM_0
#define BUTTON_K1_PIN           GPIO_NUM_7
#define BUTTON_K2_PIN           GPIO_NUM_8
#define BUTTON_ACTIVE_LEVEL     0

static const char *TAG = "button_encoder";

const char *knob_event_table[] = 
{
    "KNOB_LEFT",
    "KNOB_RIGHT",
    "KNOB_H_LIM",
    "KNOB_L_LIM",
    "KNOB_ZERO",
};

static knob_handle_t knob = NULL;

int16_t get_knob_diffval(void)
{
    int16_t knob_res;
    knob_res = iot_knob_get_count_value(knob);
    iot_knob_clear_count_value(knob);
    if( knob_res * knob_diff < 0 )
    {
        knob_diff = 0;
    }
    knob_diff = knob_diff + knob_res;
    knob_res =  knob_diff / 2 ;
    knob_diff = knob_diff - knob_res*2;
    return knob_res;
}

static void knob_event_cb(void *arg, void *data)
{
    
    /*ESP_LOGI(TAG, "knob event %s, %d", knob_event_table[(knob_event_t)data], iot_knob_get_count_value(knob));
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_UNDEFINED) 
    {
        ESP_LOGI(TAG, "Wake up from light sleep, reason %d", cause);
    }*/
}

void knob_init(void)
{
    knob_config_t cfg = {
        .default_direction = 1,
        .gpio_encoder_a = ENCODER_PA_PIN,
        .gpio_encoder_b = ENCODER_PB_PIN,
#if CONFIG_PM_ENABLE
        .enable_power_save = true,
#endif
    };

    knob = iot_knob_create(&cfg);
    assert(knob);
    esp_err_t err = iot_knob_register_cb(knob, KNOB_LEFT, knob_event_cb, (void *)KNOB_LEFT);
    err |= iot_knob_register_cb(knob, KNOB_RIGHT, knob_event_cb, (void *)KNOB_RIGHT);
    err |= iot_knob_register_cb(knob, KNOB_H_LIM, knob_event_cb, (void *)KNOB_H_LIM);
    err |= iot_knob_register_cb(knob, KNOB_L_LIM, knob_event_cb, (void *)KNOB_L_LIM);
    err |= iot_knob_register_cb(knob, KNOB_ZERO, knob_event_cb, (void *)KNOB_ZERO);
    ESP_ERROR_CHECK(err);
}


static void button0_event_cb(void *arg, void *data)
{
    button_event_t btn_event ;
    btn_event = iot_button_get_event((button_handle_t)arg);
    switch(btn_event)
    {
        case BUTTON_SINGLE_CLICK:
            keypad_val = 1 << 0 ;
        break;   
        case BUTTON_LONG_PRESS_START:
            keypad_val = 2 << 0 ;
        break;    
        default:
            keypad_val = 0xF << 0 ;
        break;
    }

    /*iot_button_print_event((button_handle_t)arg);
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_UNDEFINED) 
    {
        ESP_LOGI(TAG, "Wake up from light sleep, reason %d", cause);
    }*/
}

static void button1_event_cb(void *arg, void *data)
{
    button_event_t btn_event ;
    btn_event = iot_button_get_event((button_handle_t)arg);
    switch(btn_event)
    {
        case BUTTON_SINGLE_CLICK:
            keypad_val = 1 << 4 ;
        break;   
        case BUTTON_LONG_PRESS_START:
            keypad_val = 2 << 4 ;
        break;    
        default:
            keypad_val = 0xF << 4 ;
        break;
    }
    /*iot_button_print_event((button_handle_t)arg);
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_UNDEFINED) 
    {
        ESP_LOGI(TAG, "Wake up from light sleep, reason %d", cause);
    }*/
}

static void button2_event_cb(void *arg, void *data)
{
    button_event_t btn_event ;
    btn_event = iot_button_get_event((button_handle_t)arg);
    switch(btn_event)
    {
        case BUTTON_SINGLE_CLICK:
            keypad_val = 1 << 8 ;
        break;   
        case BUTTON_LONG_PRESS_START:
            keypad_val = 2 << 8 ;
            //encoder_val = 2 << 8 ;
        break;    
        default:
            keypad_val = 0xF << 8 ;
        break;
    }
    /*iot_button_print_event((button_handle_t)arg);
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_UNDEFINED) 
    {
        ESP_LOGI(TAG, "Wake up from light sleep, reason %d", cause);
    }*/
}

static void button3_event_cb(void *arg, void *data)
{
    button_event_t btn_event ;
    btn_event = iot_button_get_event((button_handle_t)arg);
    switch(btn_event)
    {
        case BUTTON_SINGLE_CLICK:
            keypad_val = 1 << 12 ;
        break;   
        case BUTTON_LONG_PRESS_START:
            keypad_val = 2 << 12 ;
        break;    
        default:
            keypad_val = 0xF << 12 ;
        break;
    }
    /*iot_button_print_event((button_handle_t)arg);
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause != ESP_SLEEP_WAKEUP_UNDEFINED) 
    {
        ESP_LOGI(TAG, "Wake up from light sleep, reason %d", cause);
    }*/
}


void button_init(void)
{
    button_config_t btn_cfg =
    {
        .short_press_time = 20 , //20ms
        .long_press_time = 1500 , //2s
    };
    button_gpio_config_t gpio_cfg = 
    {
        .gpio_num = BUTTON_K0_PIN ,
        .active_level = BUTTON_ACTIVE_LEVEL,
        .enable_power_save = true,
    };

    button_handle_t btn0 , btn1 , btn2 , btn3 ;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn0);
    assert(ret == ESP_OK);
    ret |= iot_button_register_cb(btn0, BUTTON_SINGLE_CLICK, NULL, button0_event_cb, NULL);
    ret |= iot_button_register_cb(btn0, BUTTON_LONG_PRESS_START, NULL, button0_event_cb, NULL);
    //ret |= iot_button_register_cb(btn0, BUTTON_PRESS_END, NULL, button0_event_cb, NULL);
    ESP_ERROR_CHECK(ret);

    gpio_cfg.gpio_num = BUTTON_K1_PIN ;
    ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn1);
    assert(ret == ESP_OK);
    ret |= iot_button_register_cb(btn1, BUTTON_SINGLE_CLICK, NULL, button1_event_cb, NULL);
    ret |= iot_button_register_cb(btn1, BUTTON_LONG_PRESS_START, NULL, button1_event_cb, NULL);
    //ret |= iot_button_register_cb(btn1, BUTTON_PRESS_END, NULL, button1_event_cb, NULL);
    ESP_ERROR_CHECK(ret);

    gpio_cfg.gpio_num = BUTTON_K2_PIN ;
    ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn2);
    assert(ret == ESP_OK);
    ret |= iot_button_register_cb(btn2, BUTTON_SINGLE_CLICK, NULL, button2_event_cb, NULL);
    ret |= iot_button_register_cb(btn2, BUTTON_LONG_PRESS_START, NULL, button2_event_cb, NULL);
    //ret |= iot_button_register_cb(btn2, BUTTON_PRESS_END, NULL, button2_event_cb, NULL);
    ESP_ERROR_CHECK(ret);

    
    gpio_cfg.gpio_num = ENCODER_SW_PIN ;
    ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn3);
    assert(ret == ESP_OK);
    ret |= iot_button_register_cb(btn3, BUTTON_SINGLE_CLICK, NULL, button3_event_cb, NULL);
    ret |= iot_button_register_cb(btn3, BUTTON_LONG_PRESS_START, NULL, button3_event_cb, NULL);
    //ret |= iot_button_register_cb(btn3, BUTTON_PRESS_END, NULL, button3_event_cb, NULL);
    ESP_ERROR_CHECK(ret);
}

void knob_keypad_init(void)
{
    keypad_val = 0 ;
    knob_init();
    button_init();
    /*while(1)
    {

    }*/
}