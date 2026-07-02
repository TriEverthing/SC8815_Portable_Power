/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/26
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "ch32l103x_bsp.h"
//CH32 ADC
#include "adc_conv.h"
//SC8815
#include "sc8815_user.h"
#include "SC8815.h"
//INA226
#include "ina226_user.h"
#include "ina226.h"
//USBPD
#include "usbpd_sink.h"
//comm uart
#include "comm_uart.h"
//power ctrl
#include "comm_powerctrl.h"
//usb
#include "usbd_cdc_data.h"

void LED_Run_Task(void *pvParameters);

/* FreeRTOS Stack Size , Unit Word , Total=4KB */
#define LED_TASK_STACK_SIZE             ( 64/4)    /* 64Byte  */
#define COMM_POWER_TASK_STACK_SIZE      (512/4)    /* 512Byte */
#define USBPD_TASK_STACK_SIZE           (512/4)    /* 512Byte */
#define USBPD_ISR_TASK_STACK_SIZE       (512/4)    /* 512Byte */
#define USBFS_TASK_STACK_SIZE           (512/4)    /* 512Byte */
#define USBFS_CDC_TASK_STACK_SIZE       (512/4)    /* 512Byte */
/* FreeRTOS Task Priorities */
#define LED_TASK_PRIORITIES             (configMAX_PRIORITIES-3)
#define COMM_POWER_TASK_PRIORITIES      (configMAX_PRIORITIES-3)
#define USBPD_TASK_PRIORITIES           (configMAX_PRIORITIES-3)
#define USBPD_ISR_TASK_PRIORITIES       (configMAX_PRIORITIES-2)
#define USBFS_TASK_PRIORITIES           (configMAX_PRIORITIES-1)
#define USBFS_CDC_TASK_PRIORITIES       (configMAX_PRIORITIES-3)


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    //interrupt nesting enable(CSR-0x804 bit1 = 1)
    //NVIC_IRQChannelPreemptionPriority - range from 0 to 1.
    //NVIC_IRQChannelSubPriority - range from 0 to 3.
    SystemCoreClockUpdate();

    /* Initialize Debug system. */
    DebugInit();

    /* Initialize LED. */
    led_init( LED0 );

    /* led blink task */
    xTaskCreate((TaskFunction_t )LED_Run_Task ,
                        (const char*    )"LED_Task" ,
                        (uint16_t       )LED_TASK_STACK_SIZE ,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )LED_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);

    /* Power Control task */
    xTaskCreate((TaskFunction_t )comm_powerctrl_Task ,
                        (const char*    )"Power_Task",
                        (uint16_t       )COMM_POWER_TASK_STACK_SIZE ,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )COMM_POWER_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);

    /* CH32 USBPD task */
    xTaskCreate((TaskFunction_t )USBPD_Task ,
                        (const char*    )"PD_Task",
                        (uint16_t       )USBPD_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )USBPD_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);
    
    /* CH32 USBPD ISR task */
    xTaskCreate((TaskFunction_t )USBPD_ISR_Task ,
                        (const char*    )"PD_ISR_Task",
                        (uint16_t       )USBPD_ISR_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )USBPD_ISR_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);
#if ( DEBUG_TO_USB_CDC && DEBUG_ON_OFF )
    /* CH32 USBFS task */
    xTaskCreate((TaskFunction_t )Usb_Dev_Task ,
                        (const char*    )"Usb_Task",
                        (uint16_t       )USBFS_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )USBFS_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);

    /* CH32 USBFS CDC task */
    xTaskCreate((TaskFunction_t )Usb_Cdc_Task ,
                        (const char*    )"Cdc_Task",
                        (uint16_t       )USBFS_CDC_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )USBFS_CDC_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);
#endif
    vTaskStartScheduler();

    while(1)
    {

    }
}


void LED_Run_Task(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(500); 
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        led_toggle( LED0 );
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

#if USE_FreeRTOS
/*******************************************************************************
 * FreeRTOS Hook function
 ******************************************************************************/
#if configUSE_IDLE_HOOK
void vApplicationIdleHook(void) 
{
    //EnterStopMode_NoOperation_TimeOut();
}
#endif


#if configUSE_TICK_HOOK
void vApplicationTickHook(void) 
{
   extern volatile uint64_t ticks_count_overflow;
   ticks_count_overflow ++ ;
}
#endif

#if configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS(); 
    while(1);
}
#endif

#if configCHECK_FOR_STACK_OVERFLOW  
void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName) 
{
    while(1);
}
#endif  

#if configUSE_DAEMON_TASK_STARTUP_HOOK
void vApplicationDaemonTaskStartupHook( void )
{

}
#endif

#endif
