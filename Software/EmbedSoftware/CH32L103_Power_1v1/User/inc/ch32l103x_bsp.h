#ifndef __CH32L103X_BSP_H
#define __CH32L103X_BSP_H

/* includes -----------------------------------------------------------------------*/
#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
/* mpaland-printf */
#include "printf.h"
/* ch32l103 include files */
#include "ch32l103.h"
/* private includes -------------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* exported types -------------------------------------------------------------*/
typedef enum
{
  LED0                                   = 0,
  LED1                                   = 1,
  LED2                                   = 2
}led_type;

/* RCC_Exported_Types */
typedef struct
{
    uint32_t SYSCLK_Frequency; /* returns SYSCLK clock frequency expressed in Hz */
    uint32_t HCLK_Frequency;   /* returns HCLK clock frequency expressed in Hz */
    uint32_t PCLK1_Frequency;  /* returns PCLK1 clock frequency expressed in Hz */
    uint32_t PCLK2_Frequency;  /* returns PCLK2 clock frequency expressed in Hz */
    uint32_t ADCCLK_Frequency; /* returns ADCCLK clock frequency expressed in Hz */
} CH32_ClocksTypeDef;
/* exported macro ------------------------------------------------------------*/

/******************** define led ********************/
#define LED_NUM                 1
//LED0
#define LED0_NUMS               2
#define LED0_GPIO               GPIOB
#define LED0_GPIO_RCC_EN        RCC_IOPBEN

/* exported macro ------------------------------------------------------------*/
#define USART_REC_LEN       200   //defines the maximum number of received bytes of 200
#define EN_USART1_RX        0     //Enable (1) / Disable (0) Serial port 1 receive
//Receive status flag
extern uint8_t  USART_RX_BUF[USART_REC_LEN];  //Receive buffer, up to USART_REC_LEN bytes. The last byte is '\n'
extern uint16_t USART_RX_STA;
/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
#define DEBUG_ON_OFF            1
#define DEBUG_TO_UART           0
#define DEBUG_TO_USB_CDC        1
#define USE_LETTER_SHELL        0
#define USE_EasyLogger          0
#define USE_FreeRTOS            1
#define Usb_To_Uart             0
#define UART_DMA_TX             1
#define USB_Stack_CherryUSB     0   
#define USB_Stack_TinyUSB       0

#if USB_Stack_TinyUSB && USB_Stack_CherryUSB
#error "USB_Stack_CherryUSB and USB_Stack_TinyUSB don't both as 1"
#endif

#if DEBUG_TO_UART && DEBUG_TO_USB_CDC

#endif

/* UART Printf Definition */
#define DEBUG_UART1           1
#define DEBUG_UART2           2
#define DEBUG_UART3           3

/* DEBUG UATR Definition */
#ifndef DEBUG
#define DEBUG          DEBUG_UART1
#endif


#if DEBUG_ON_OFF
#if DEBUG_TO_UART
#define dbg_printf( format , ... )              uart_printf( format , ##__VA_ARGS__ )
#elif DEBUG_TO_USB_CDC
extern int usb_printf(const char *fmt, ...);
#define dbg_printf( format , ... )              usb_printf( format , ##__VA_ARGS__ )
#else
#error "DEBUG_TO_UART and DEBUG_TO_USB_CDC don't both as 1"
#endif
#else
#define dbg_printf( format , ... )
#endif

#if USE_EasyLogger
#include "elog.h"
#define elog_printf( format , ... )             dbg_printf( format , ##__VA_ARGS__ )
#endif

#if USE_FreeRTOS
#ifndef FREERTOS_CONFIG_H
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#endif
#endif

//GPIO Bit Band

/******************* define time base *****************/
#define SysTick_CTRL_ENABLE_Msk     (uint32_t)(0x00000001)
#define SysTick_CTRL_STIE_Msk       (uint32_t)(0x00000002)
#define SysTick_CTRL_STCLK_Msk      (uint32_t)(0x00000004)
#define SysTick_CTRL_STRE_Msk       (uint32_t)(0x00000008)
#define SysTick_CTRL_MODE_Msk       (uint32_t)(0x00000010)
#define SysTick_CTRL_TICKINT_Msk    (uint32_t)(0x00000020)
#define SysTick_CTRL_SWIE_Msk       (uint32_t)(0x80000000)
#define STEP_DELAY_MS      		    (uint32_t)(50)
#define TICK_COUNT_MAX      	    (uint64_t)(0xFFFFFFFFFFFFFFFF )
#define TICK_COUNT_VALUE      	    (SysTick->CNT)
/* Exported macro ------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void ch32_timebase_init(void);

void DebugInit( void );

void UartDebugInit(uint32_t baudrate);

void uart_printf(const char* fmt,...);

void CH32_GetClocksFreq(CH32_ClocksTypeDef* RCC_Clocks);

void led_init(led_type led);

void led_on(led_type led);

void led_off(led_type led);

void led_toggle(led_type led);

void soft_delay_ms( uint32_t dms );

void soft_delay_us( uint32_t dus );

void delay_init(void);

void delay_ms(uint32_t nms);

void delay_us(uint32_t nus);

void delay_xms(uint32_t nms);

void usbd_low_level_init(void);

/* External variables --------------------------------------------------------*/

#endif