#include "ch32l103x_bsp.h"
#include "usbd_cdc_data.h"

#include "lwrb/lwrb.h"


/*******************************************************************************/
/* Variable Definition */
struct cdc_line_coding line_coding;
uint8_t dtr_enable = 0, rts_enable = 0;

 /* Endpoint Buffer */
 #if (( CDC_UEP_INT & 0x0F ) == CDC_UEP_OUT )
 __attribute__ ((aligned(4))) uint8_t USBFS_CDC_OUT_Buf[DEF_USBD_FS_PACK_SIZE*2];   /* Endpoint descriptor , 0x02 OUT */
uint8_t * USBFS_CDC_INT_Buf = USBFS_CDC_OUT_Buf + DEF_USBD_FS_PACK_SIZE;            /* Interrupt upload endpoint descriptor , 0x81 IN */
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_IN_Buf[DEF_USBD_FS_PACK_SIZE];       /* Endpoint descriptor , 0x83 IN */
#elif (( CDC_UEP_IN & 0x0F ) == CDC_UEP_OUT )
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_OUT_Buf[DEF_USBD_FS_PACK_SIZE*2];    /* Endpoint descriptor , 0x02 OUT */
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_INT_Buf[DEF_USBD_FS_PACK_SIZE];      /* Interrupt upload endpoint descriptor , 0x81 IN */
uint8_t USBFS_CDC_IN_Buf = USBFS_CDC_OUT_Buf + DEF_USBD_FS_PACK_SIZE;               /* Endpoint descriptor , 0x83 IN */
#else
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_INT_Buf[DEF_USBD_FS_PACK_SIZE];      /* Interrupt upload endpoint descriptor , 0x81 IN */
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_OUT_Buf[DEF_USBD_FS_PACK_SIZE];      /* Endpoint descriptor , 0x02 OUT */
__attribute__ ((aligned(4))) uint8_t USBFS_CDC_IN_Buf[DEF_USBD_FS_PACK_SIZE];       /* Endpoint descriptor , 0x83 IN */
#endif


#if USE_FreeRTOS
extern volatile BaseType_t xSchedulerRunning;
SemaphoreHandle_t UsbIsrSemaphore;
SemaphoreHandle_t UsbLwrbTxMutex;
//uint8_t UsbLwrbTxMutex_is_ok = 0;
#endif

/* TX Buffer */
uint8_t LwrbTxBuf[USB_CDC_TX_BUF_SIZE];
lwrb_t cdctx_lwrb;

/*********************************************************************
 * @fn      EPx_IN_Callback
 *
 * @brief   IN Endpoint Callback Function 
 *
 * @return  none
 */ 
#if ( CDC_UEP_INT == 0x81 )
void EP1_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x82 )
void EP2_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x83 )
void EP3_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x84 )
void EP4_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x85 )
void EP5_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x86 )
void EP6_IN_Callback(void)
#elif ( CDC_UEP_INT == 0x87 )
void EP7_IN_Callback(void)
#endif
{
    /* Add User Code */
    
}

/*********************************************************************
 * @fn      EPx_OUT_Callback
 *
 * @brief   OUT Endpoint Callback Function 
 *
 * @return  none
 */ 
#if ( CDC_UEP_OUT == 0x01 )
void EP1_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x02 )
void EP2_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x03 )
void EP3_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x04 )
void EP4_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x05 )
void EP5_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x06 )
void EP6_OUT_Callback(void)
#elif ( CDC_UEP_OUT == 0x07 )
void EP7_OUT_Callback(void)
#endif
{
    /* Add User Code */

}

/*********************************************************************
 * @fn      EPx_IN_Callback
 *
 * @brief   IN Endpoint Callback Function 
 *
 * @return  none
 */ 
#if ( CDC_UEP_IN == 0x81 )
void EP1_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x82 )
void EP2_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x83 )
void EP3_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x84 )
void EP4_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x85 )
void EP5_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x86 )
void EP6_IN_Callback(void)
#elif ( CDC_UEP_IN == 0x87 )
void EP7_IN_Callback(void)
#endif
{

}


/*********************************************************************
 * @fn      usb_dev_non_standard_requset
 *
 * @brief   usb non-standard request processing 
 *
 * @return  none
 */ 
uint8_t usb_dev_non_standard_requset(void)
{
    uint8_t errflag = 0;
    if( USBFS_SetupReqType & USB_REQ_TYP_CLASS ) /* USB Class Request */
    {
        /* Class requests */
        switch( USBFS_SetupReqCode )
        {
            case CDC_REQUEST_SET_LINE_CODING:  /* OUT , bWlength = 7 */
                /*******************************************************************************/
                /* Line Coding Structure                                                       */
                /*-----------------------------------------------------------------------------*/
                /* Offset | Field       | Size | Value  | Description                          */
                /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
                /* 4      | bCharFormat |   1  | Number | Stop bits                            */
                /*                                        0 - 1 Stop bit                       */
                /*                                        1 - 1.5 Stop bits                    */
                /*                                        2 - 2 Stop bits                      */
                /* 5      | bParityType |  1   | Number | Parity                               */
                /*                                        0 - None                             */
                /*                                        1 - Odd                              */
                /*                                        2 - Even                             */
                /*                                        3 - Mark                             */
                /*                                        4 - Space                            */
                /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
                /*******************************************************************************/
                memcpy(&line_coding, USBFS_EP0_Buf, 7);
                //set_line_coding(&line_coding);
                break;
            case CDC_REQUEST_GET_LINE_CODING:  /* IN , bWlength = 7 */
                memcpy( USBFS_EP0_Buf, &line_coding, 7 );
                break;
            case CDC_REQUEST_SET_CONTROL_LINE_STATE: /* OUT , bWlength = 0 */
                dtr_enable = (USBFS_SetupReqValue & 0x0001);
                rts_enable = (USBFS_SetupReqValue & 0x0002);
                break;
            case CDC_REQUEST_SEND_BREAK:
                break;
            default:
                errflag = 0xff;
                break;
        }
    }
    else if( USBFS_SetupReqType & USB_REQ_TYP_VENDOR ) /* USB Vendor Request */
    {
        /* Manufacturer request */
    }
    else
    {
        errflag = 0xFF;
    }

    return errflag;
}

/*********************************************************************
 * @fn      usb_dev_endpoint_init
 *
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
void usb_dev_endpoint_init(void)
{
#if USE_FreeRTOS
    UsbIsrSemaphore = xSemaphoreCreateBinary();
#endif

 #if (( CDC_UEP_INT & 0x0F ) == CDC_UEP_OUT )
    USB_SET_DMA(CDC_UEP_OUT, (uint32_t)USBFS_CDC_OUT_Buf);    
    USB_SET_DMA(CDC_UEP_IN&0x0F, (uint32_t)USBFS_CDC_IN_Buf);    
#elif (( CDC_UEP_IN & 0x0F ) == CDC_UEP_OUT )
    USB_SET_DMA(CDC_UEP_INT&0x0F, (uint32_t)USBFS_CDC_INT_Buf);
    USB_SET_DMA(CDC_UEP_OUT, (uint32_t)USBFS_CDC_OUT_Buf);    
#else
    USB_SET_DMA((CDC_UEP_INT&0x0F), (uint32_t)USBFS_CDC_INT_Buf);
    USB_SET_DMA(CDC_UEP_OUT, (uint32_t)USBFS_CDC_OUT_Buf);    
    USB_SET_DMA((CDC_UEP_IN&0x0F), (uint32_t)USBFS_CDC_IN_Buf);    
#endif

    usb_ep_conf(CDC_UEP_INT);
    usb_ep_conf(CDC_UEP_OUT);
    usb_ep_conf(CDC_UEP_IN);
    
    line_coding.dwDTERate = 115200 ;
    line_coding.bCharFormat = 0 ; //0 - 1 Stop bit
    line_coding.bParityType = 0;
    line_coding.bDataBits = 8; //Number Data bits (5, 6, 7, 8 or 16)
}

/********************** FreeRTOS ISR & Task *************************/
#if USE_FreeRTOS
void USBFS_IRQHandler(void) __attribute__((interrupt()));

void USBFS_IRQHandler(void) 
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    NVIC_DisableIRQ(USBFS_IRQn);
    xSemaphoreGiveFromISR(UsbIsrSemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void Usb_Dev_Task(void *pvParameters)
{
    usb_dev_init(ENABLE);
    while(1)
    {
        if( xSemaphoreTake( UsbIsrSemaphore, portMAX_DELAY) == pdPASS )
        {
            usbfs_dev_isr();
            NVIC_EnableIRQ(USBFS_IRQn);
        }
    }
}

void Usb_Cdc_Task(void *pvParameters)
{
    uint8_t ep_idx = CDC_UEP_IN % DEF_UEP_NUM;
    uint8_t nbytes = 0 ;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(10); 
    xLastWakeTime = xTaskGetTickCount();
#if USE_FreeRTOS
    /*  Creates a new mutex type semaphore instance */
    UsbLwrbTxMutex = xSemaphoreCreateMutex();
#endif
    while(UsbLwrbTxMutex == NULL);
    while(1)
    {
        if( dtr_enable )
        {
            if( USBFS_Endp_Busy[ep_idx] == 0 )
            { 
                if ( xSemaphoreTake(UsbLwrbTxMutex, 0) == pdTRUE) 
                {
                    /* Read data from buffer */
                    nbytes = lwrb_read( &cdctx_lwrb, USBFS_CDC_IN_Buf, DEF_USBD_FS_PACK_SIZE);
                    xSemaphoreGive(UsbLwrbTxMutex);
                    if( nbytes )
                    {
                        /* Set end-point busy */
                        USBFS_Endp_Busy[ep_idx] = 0x01;
                        /* tx length */
                        USB_SET_TX_LEN(ep_idx,nbytes);
                        /* response ack */
                        USBFSD_UEP_CTRL(ep_idx) = (USBFSD_UEP_CTRL(ep_idx) & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
                    }
                    else if( USB_GET_TX_LEN(ep_idx)%DEF_USBD_FS_PACK_SIZE == 0 && USB_GET_TX_LEN(ep_idx) )  /* zero-length packet */
                    {
                        /* Set end-point busy */
                        USBFS_Endp_Busy[ep_idx] = 0x01;
                        /* tx length */
                        USB_SET_TX_LEN(ep_idx,0);
                        /* response ack */
                        USBFSD_UEP_CTRL(ep_idx) = (USBFSD_UEP_CTRL(ep_idx) & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;                    
                    }
                }
            }                
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }        
}
#endif

/**
 * @brief  None.
 * @param  None.
 * @return None.
 */
void UsbDebugInit(void)
{
    dtr_enable = 0 ;
    rts_enable = 0 ;
    /* Initialize buffer */
    lwrb_init(&cdctx_lwrb, LwrbTxBuf, USB_CDC_TX_BUF_SIZE);
}

char chPrintBuf[USB_PRINT_BUF_SIZE];
/**
 * @brief  usb vcp printf to buffer uart_rx_buffer, the
 *         received number is counted by APP_Rx_ptr_in
 * @param  None
 * @retval None
 */
int usb_printf(const char *fmt, ...)
{
    va_list arg;
    int len = 0;

#if USE_FreeRTOS
    //taskENTER_CRITICAL();
    if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
    {
        /* FreeRTOS Mutex Lock */
        if(xSemaphoreTake(UsbLwrbTxMutex, portMAX_DELAY) != pdTRUE) 
        {
            return -1; 
        }
    }
#endif

    va_start( arg, fmt);
    len = vsnprintf( chPrintBuf, USB_PRINT_BUF_SIZE, (const char *)fmt, arg);
    va_end(arg);

    if( len > 0 )
    {
        if( len >= USB_PRINT_BUF_SIZE )
        {
            /* string cut off */
            len = USB_PRINT_BUF_SIZE;
        }
        /* Write data to lwrb buffer. */
        lwrb_overwrite(&cdctx_lwrb, chPrintBuf, len);
    }
    else
    {
        /* Error */
        //lwrb_overwrite(&cdctx_lwrb, "Error\r\n", sizeof("Error\r\n"));        
    }

#if USE_FreeRTOS
    //taskEXIT_CRITICAL();
    /* FreeRTOS Mutex Unlock */
    if( xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
    {
        xSemaphoreGive(UsbLwrbTxMutex);
    }
#endif
    return len;
}
