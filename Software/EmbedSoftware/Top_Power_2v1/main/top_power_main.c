/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_chip_info.h"
//#include "esp_flash.h"
#include "esp_system.h"
//ESP32-IDF include files
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <driver/spi_common.h>
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
//FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
//lvgl
#include "lvgl.h"
//lvgl drivers
#include "lvgl_helpers.h"
//nxp guider
#include <gui_guider.h>
#include <events_init.h>
//
#include "lvgl_key.h"
//comm
#include "power_data.h"
#include "comm_uart.h"
#include "comm_powerctrl.h"
//WiFi
#include "wifi_webserver.h"

#define LED_RUN_PIN     GPIO_NUM_6

void LED_Task( void *pvParmeters );
void guiTask(void *pvParameter) ;

lv_indev_t * indev_keypad = NULL ;
lv_indev_t * indev_encoder = NULL ;

void lv_tick_task(void *arg)
{
    lv_tick_inc(1);
}


void app_main(void)
{
    //LED pin
    static gpio_config_t gpiocfg = 
    {
        .pin_bit_mask = (1ULL << LED_RUN_PIN ) , 
        .mode = GPIO_MODE_OUTPUT ,
        .pull_down_en = 0 ,
        .pull_up_en = 0 ,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpiocfg);

    comm_powerctrl_init();

    xTaskCreate( LED_Task , "LED_Task" , configMINIMAL_STACK_SIZE , NULL , 1 , NULL );

    xTaskCreate( WiFi_Task , "WiFi_Task" , configMINIMAL_STACK_SIZE*2 , NULL , 1 , NULL );

    xTaskCreate( guiTask , "Gui_Task" , configMINIMAL_STACK_SIZE*4 , NULL , 1 , NULL );

    xTaskCreate( comm_powerctrl_Task , "Power_Task" , configMINIMAL_STACK_SIZE*2, NULL , 1 , NULL );

    xTaskCreate( comm_uart_recv_Task , "Rev_Task" , configMINIMAL_STACK_SIZE*2 , NULL , 2 , NULL );
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
lv_ui guider_ui ;
SemaphoreHandle_t xGuiSemaphore ;
extern disp_backlight_h bckl_handle ;

static void power_switch_event_handler(lv_event_t * even_t)
{
    lv_event_code_t code = lv_event_get_code(even_t);
    lv_obj_t * obj = lv_event_get_target(even_t);
    if( code == LV_EVENT_VALUE_CHANGED ) 
    {
        if( lv_obj_has_state(obj, LV_STATE_CHECKED) )
        {
            ESP_LOGI("lvgl event","Power Siwcth ON.");
            lv_label_set_text( guider_ui.ST7789V3_Status_Lable1 , "ON" );
            power_data.set_switch_status = 1 ;
            comm_powerctrl_set_output(1);  
            comm_powerctrl_set_mode( 0 , power_data.set_voltage , power_data.set_current );
        }
        else
        {
            ESP_LOGI("lvgl event","Power Siwcth OFF.");
            lv_label_set_text( guider_ui.ST7789V3_Status_Lable1 , "OFF" );
            power_data.set_switch_status = 0 ;
            comm_powerctrl_set_output(0);  
        }

    }
}

static void power_setvoltage_event_handler(lv_event_t * even_t)
{
    lv_event_code_t code = lv_event_get_code(even_t);
    lv_obj_t * obj = lv_event_get_target(even_t);
    int32_t power_voltage_t ;
    if( code == LV_EVENT_VALUE_CHANGED ) 
    {
        power_voltage_t = lv_spinbox_get_value(obj);
        power_data.set_voltage = power_voltage_t * 10 ;
        ESP_LOGI("lvgl event","Power Voltage:%dmV.", power_data.set_voltage );
        comm_powerctrl_set_mode( 0 , power_data.set_voltage , power_data.set_current );
    }
}

static void power_setcurrent_event_handler(lv_event_t * even_t)
{
    lv_event_code_t code = lv_event_get_code(even_t);
    lv_obj_t * obj = lv_event_get_target(even_t);
    int32_t power_current_t ;
    if( code == LV_EVENT_VALUE_CHANGED ) 
    {
        power_current_t = lv_spinbox_get_value(obj);
        power_data.set_current = power_current_t * 10 ;
        ESP_LOGI("lvgl event","Power Current:%dmA.", power_data.set_current );
        comm_powerctrl_set_mode( 0 , power_data.set_voltage , power_data.set_current );
    }
}


void guiTask(void *pvParameter) 
{
    uint8_t blacklight = 0 ;

    (void) pvParameter;

    /* lvgl init */
    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    /** Create double buffer:
     *  LVGL will draw the display's content to a buffer and writes it your display.
     *  You should use DMA to write the buffer's content to the display.
     *  It will enable LVGL to draw the next part of the screen to the other buffer while
     *  the data is being sent form the first buffer. It makes rendering and flushing parallel.
     */
    ESP_LOGI("lvgl", "Create double buffer.");
    static lv_disp_draw_buf_t disp_buf ;
    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE*sizeof(lv_color_t), MALLOC_CAP_DMA) ;
    assert(buf1 != NULL);
    //lv_disp_draw_buf_init(&disp_buf, buf1, NULL, DISP_BUF_SIZE );
    /* Use double buffered when not working with monochrome displays */
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE*sizeof(lv_color_t), MALLOC_CAP_DMA) ;
    assert(buf2 != NULL);
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE );

    ESP_LOGI("lvgl", "Setting the Drivers in LVGL.");
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_driver_flush;

#if defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT || defined CONFIG_DISPLAY_ORIENTATION_PORTRAIT_INVERTED
    disp_drv.rotated = 1;
#endif

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    /*Set the resolution of the display*/
#if (LVGL_VERSION_MAJOR >= 8)
    disp_drv.hor_res = LV_HOR_RES_MAX ;
    disp_drv.ver_res = LV_VER_RES_MAX ;
#endif

    ESP_LOGI("lvgl", "Register the display in LVGL.");
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
    /*Set a display buffer*/
    disp_drv.draw_buf = &disp_buf;
    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = 
    {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1 * 1000));

    ESP_LOGI("lvgl", "Create a GUI-Guider application.");
    /* Create a GUI-Guider application */
    setup_ui(&guider_ui);

    lv_spinbox_set_value(guider_ui.ST7789V3_SetVoltageSpinbox , power_data.set_voltage / 10 );
    lv_spinbox_set_value(guider_ui.ST7789V3_SetCurrentSpinbox , power_data.set_current / 10 );

    /* Initialize lvgl keypad & knob dribers */
    lvgl_indev_driver_init();
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */
    static lv_indev_drv_t keypad_indev_drv , encoder_indev_drv ;
    /*------------------
     * Keypad
     * -----------------*/
    /*Register a keypad input device*/
    lv_indev_drv_init(&keypad_indev_drv);
    keypad_indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&keypad_indev_drv);

    /*------------------
    * Encoder
    * -----------------*/
    /*Register a encoder input device*/
    lv_indev_drv_init(&encoder_indev_drv);
    encoder_indev_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_indev_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&encoder_indev_drv);

    /* To associate a group with an input device */
	lv_group_t *group=lv_group_create();
    lv_indev_set_group(indev_keypad, group);	
    lv_indev_set_group(indev_encoder, group);
    lv_group_set_editing(group, false);   // true: edit mode; false: navigate mode 

    lv_group_add_obj(group ,guider_ui.PowerSwitch);
	lv_group_add_obj(group ,guider_ui.ST7789V3_SetVoltageSpinbox);
	lv_group_add_obj(group ,guider_ui.ST7789V3_SetCurrentSpinbox);   

    lv_obj_add_event_cb( guider_ui.PowerSwitch , power_switch_event_handler , LV_EVENT_VALUE_CHANGED , NULL );
    lv_obj_add_event_cb( guider_ui.ST7789V3_SetVoltageSpinbox , power_setvoltage_event_handler , LV_EVENT_VALUE_CHANGED , NULL );
    lv_obj_add_event_cb( guider_ui.ST7789V3_SetCurrentSpinbox , power_setcurrent_event_handler , LV_EVENT_VALUE_CHANGED , NULL );

    xGuiSemaphore = xSemaphoreCreateMutex();

    while(1) 
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if ( xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdTRUE ) 
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);

            if( blacklight == 0 )
            {
                vTaskDelay( pdMS_TO_TICKS(20) );
                disp_backlight_set(bckl_handle, 40);
                blacklight = 1 ;
            }
       }
    }

    /* A task should NEVER return */
    free(buf1);
    free(buf2);

    vTaskDelete(NULL);
}

void LED_Task( void *pvParmeters )
{
    TickType_t xLastWakeTime;
    const TickType_t led_period = pdMS_TO_TICKS(500); 
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, led_period );
        gpio_set_level( LED_RUN_PIN , 0 );
        vTaskDelayUntil(&xLastWakeTime, led_period );
        gpio_set_level( LED_RUN_PIN , 1 );
    }
}

