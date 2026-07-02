/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32l103_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/10/30
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32l103_it.h"
#include "ch32l103x_bsp.h"

#if USE_FreeRTOS
void NMI_Handler(void) __attribute__((interrupt()));
void HardFault_Handler(void) __attribute__((interrupt()));
//void USBFS_IRQHandler(void) __attribute__((interrupt()));
//void USBFSWakeUp_IRQHandler(void) __attribute__((interrupt()));
#else
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBFS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USBFSWakeUp_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#endif


/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    while (1)
    {
    }
}


/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1)
    {
    }
}

#if USE_FreeRTOS == 0
/*********************************************************************
 * @fn      USBFS_IRQHandler
 *
 * @brief   This function handles USBFS exception.
 *
 * @return  none
 */
void USBFS_IRQHandler(void) 
{
  extern void usbfs_dev_isr(void);
  usbfs_dev_isr();
}

/*********************************************************************
 * @fn      USBFSWakeUp_IRQHandler
 *
 * @brief   This function handles USBFSWakeup exception.
 *
 * @return  none
 */
void USBFSWakeUp_IRQHandler(void) 
{

}
#endif