/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32l103_usbfs_device.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/01/19
* Description        : This file contains all the functions prototypes for the
*                      USBOTG firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CH32l103_USBFS_DEVICE_H_
#define __CH32l103_USBFS_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "string.h"
#include "usbd_desc.h"
#include "ch32l103_usb.h"

/******************************************************************************/
/* Global Define */
#ifndef __PACKED
  #define __PACKED   __attribute__((packed))
#endif

#ifndef   __WEAK
  #define __WEAK      __attribute__((weak))
#endif

/* end-point number */
#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00
#define DEF_UEP0                      0x00
#define DEF_UEP1                      0x01
#define DEF_UEP2                      0x02
#define DEF_UEP3                      0x03
#define DEF_UEP4                      0x04
#define DEF_UEP5                      0x05
#define DEF_UEP6                      0x06
#define DEF_UEP7                      0x07
#define DEF_UEP_NUM                   8

#define USBFSD_UEP_MOD_BASE           0x5000000C
#define USBFSD_UEP_DMA_BASE           0x50000010
#define USBFSD_UEP_LEN_BASE           0x50000030
#define USBFSD_UEP_CTL_BASE           0x50000032
#define USBFSD_UEP_RX_EN              0x08
#define USBFSD_UEP_TX_EN              0x04
#define USBFSD_UEP_BUF_MOD            0x01
#define DEF_UEP_DMA_LOAD              0 /* Direct the DMA address to the data to be processed */
#define DEF_UEP_CPY_LOAD              1 /* Use memcpy to move data to a buffer */
#define USBFSD_UEP_MOD(n)             (*((volatile uint8_t *)(USBFSD_UEP_MOD_BASE+n)))
#define USBFSD_UEP_CTRL(n)            (*((volatile uint8_t *)(USBFSD_UEP_CTL_BASE+n*0x04)))
#define USBFSD_UEP_DMA(n)             (*((volatile uint32_t *)(USBFSD_UEP_DMA_BASE+n*0x04)))
#define USBFSD_UEP_BUF(n)             ((uint8_t *)(*((volatile uint32_t *)(USBFSD_UEP_DMA_BASE+n*0x04)))+0x20000000)
#define USBFSD_UEP_TLEN(n)            (*((volatile uint16_t *)(USBFSD_UEP_LEN_BASE+n*0x04)))


#define USB_SET_DMA(ep_idx, addr)    (*(volatile uint32_t *)((uint32_t)(&USBFSD->UEP0_DMA) + 4 * ep_idx) = addr)
#define USB_SET_TX_LEN(ep_idx, len)  (*(volatile uint16_t *)((uint32_t)(&USBFSD->UEP0_TX_LEN) + 4 * ep_idx) = len)
#define USB_GET_TX_LEN(ep_idx)       (*(volatile uint16_t *)((uint32_t)(&USBFSD->UEP0_TX_LEN) + 4 * ep_idx))
#define USB_SET_TX_CTRL(ep_idx, val) (*(volatile uint8_t *)((uint32_t)(&USBFSD->UEP0_TX_CTRL) + 4 * ep_idx) = val)
#define USB_GET_TX_CTRL(ep_idx)      (*(volatile uint8_t *)((uint32_t)(&USBFSD->UEP0_TX_CTRL) + 4 * ep_idx))
#define USB_SET_RX_CTRL(ep_idx, val) (*(volatile uint8_t *)((uint32_t)(&USBFSD->UEP0_RX_CTRL) + 4 * ep_idx) = val)
#define USB_GET_RX_CTRL(ep_idx)      (*(volatile uint8_t *)((uint32_t)(&USBFSD->UEP0_RX_CTRL) + 4 * ep_idx))

/* Setup Request Packets */
#define pUSBFS_SetupReqPak                 ((PUSB_SETUP_REQ)USBFS_EP0_Buf)


/* exported types ------------------------------------------------------------*/
typedef void (*pEP_Callback)(void);

/*******************************************************************************/
/* Variable Definition */
/* Global */
extern const    uint8_t  *pUSBFS_Descr;

/* Setup Request */
extern volatile uint8_t  USBFS_SetupReqCode;
extern volatile uint8_t  USBFS_SetupReqType;
extern volatile uint16_t USBFS_SetupReqValue;
extern volatile uint16_t USBFS_SetupReqIndex;
extern volatile uint16_t USBFS_SetupReqLen;

/* USB Device Status */
extern volatile uint8_t  USBFS_DevConfig;
extern volatile uint8_t  USBFS_DevAddr;
extern volatile uint8_t  USBFS_DevSleepStatus;
extern volatile uint8_t  USBFS_DevEnumStatus;

/* Endpoint Buffer */
extern __attribute__ ((aligned(4))) uint8_t USBFS_EP0_Buf[ ];
/* USB IN Endpoint Busy Flag */
extern volatile uint8_t  USBFS_Endp_Busy[ ];

/* Endpoint Callback Function Pionter Array */
extern pEP_Callback EP_IN_Callback[DEF_UEP_NUM-1];
extern pEP_Callback EP_OUT_Callback[DEF_UEP_NUM-1];
/******************************************************************************/
/* external functions */
void usb_dev_init(uint8_t sta );

void usb_ep_conf(uint8_t ep_idx);

void usb_dev_endpoint_init(void);

uint8_t usb_dev_non_standard_requset(void);

void usbfs_dev_isr(void);

void usb_dev_send_resume(void);

/* IN Endpoint Callback Function */
void EP1_IN_Callback(void);
void EP2_IN_Callback(void);
void EP3_IN_Callback(void);
void EP4_IN_Callback(void);
void EP5_IN_Callback(void);
void EP6_IN_Callback(void);
void EP7_IN_Callback(void);
/* OUT Endpoint Callback Function */
void EP1_OUT_Callback(void);
void EP2_OUT_Callback(void);
void EP3_OUT_Callback(void);
void EP4_OUT_Callback(void);
void EP5_OUT_Callback(void);
void EP6_OUT_Callback(void);
void EP7_OUT_Callback(void);

#ifdef __cplusplus
}
#endif


#endif /* USER_CH32l103_USB_DEVICE_H_ */

