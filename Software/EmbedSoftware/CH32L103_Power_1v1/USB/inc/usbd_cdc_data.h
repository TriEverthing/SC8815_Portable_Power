#ifndef __USBD_CDC_DATA_H__
#define __USBD_CDC_DATA_H__

/* includes -----------------------------------------------------------------------*/
#include "stdint.h"
#include "ch32l103_usbfs_device.h"

/* exported types ------------------------------------------------------------*/
/* Line coding structure for GET_LINE_CODING / SET_LINE_CODING class requests*/
/* Format of the data returned when a GetLineCoding request is received */
/* (usbcdc11.pdf, 6.2.13) */
struct cdc_line_coding {
    uint32_t dwDTERate;  /* Data terminal rate in bits per second */
    uint8_t bCharFormat; /* Number of stop bits */
    uint8_t bParityType; /* Parity bit type */
    uint8_t bDataBits;   /* Number of data bits */
} __PACKED;

/* exported macro ------------------------------------------------------------*/
#define CDC_REQUEST_SET_LINE_CODING                0x20
#define CDC_REQUEST_GET_LINE_CODING                0x21
#define CDC_REQUEST_SET_CONTROL_LINE_STATE         0x22
#define CDC_REQUEST_SEND_BREAK                     0x23
/* exported constants --------------------------------------------------------*/

/* exported macro ------------------------------------------------------------*/
#define USB_CDC_TX_BUF_SIZE                        (2048)
#define USB_PRINT_BUF_SIZE                         (128)
#define USB_CDC_RX_BUF_SIZE                        (512)
/* Exported define -----------------------------------------------------------*/
#define CDC_UEP_INT                                 0x81     /* Interrupt upload endpoint, 0x81 IN */
#define CDC_UEP_OUT                                 0x02     /* RX Endpoint, 0x02 OUT */
#define CDC_UEP_IN                                  0x83     /* TX Endpoint, 0x83 OUT */

/*******************************************************************************/
/* Variable Definition */
extern struct cdc_line_coding line_coding;
extern uint8_t dtr_enable, rts_enable;

/* Exported functions ------------------------------------------------------- */

#if USE_FreeRTOS
void Usb_Dev_Task(void *pvParameters);
void Usb_Cdc_Task(void *pvParameters);
#endif

void UsbDebugInit(void);

int usb_printf(const char *fmt, ...);

#endif /* __USBD_CDC_DATA_H__ */