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
//comm
#include "power_data.h"
#include "comm_uart.h"
#include "comm_powerctrl.h"
//lvgl user
#include "lvgl_user.h"
//WiFi
#include "wifi_webserver.h"

#define LED_RUN_PIN     GPIO_NUM_6

void LED_Task( void *pvParmeters );

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

