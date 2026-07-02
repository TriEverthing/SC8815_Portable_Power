/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32l103_usbfs_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/01/19
* Description        : This file provides all the USBOTG firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32l103x_bsp.h"
#include "ch32l103_usbfs_device.h"


/*******************************************************************************/
/* Variable Definition */
/* Global */
const    uint8_t  *pUSBFS_Descr;

/* Setup Request */
volatile uint8_t  USBFS_SetupReqCode;
volatile uint8_t  USBFS_SetupReqType;
volatile uint16_t USBFS_SetupReqValue;
volatile uint16_t USBFS_SetupReqIndex;
volatile uint16_t USBFS_SetupReqLen;

/* endpoint 0 Variable and Buffer */
__attribute__ ((aligned(4))) uint8_t USBFS_EP0_Buf[ DEF_USBD_UEP0_SIZE ];
uint8_t ep0_send_zlp = 0;

/* USB Device Status */
volatile uint8_t  USBFS_DevConfig;
volatile uint8_t  USBFS_DevAddr;
volatile uint8_t  USBFS_DevSleepStatus;
volatile uint8_t  USBFS_DevEnumStatus;

/* Endpoint Callback Function Pionter Array */
pEP_Callback EP_IN_Callback[DEF_UEP_NUM-1]=
{
    EP1_IN_Callback,
    EP2_IN_Callback,
    EP3_IN_Callback,
    EP4_IN_Callback,
    EP5_IN_Callback,
    EP6_IN_Callback,
    EP7_IN_Callback
};
pEP_Callback EP_OUT_Callback[DEF_UEP_NUM-1]=
{
    EP1_OUT_Callback,
    EP2_OUT_Callback,
    EP3_OUT_Callback,
    EP4_OUT_Callback,
    EP5_OUT_Callback,
    EP6_OUT_Callback,
    EP7_OUT_Callback
};

/* USB IN Endpoint Busy Flag */
volatile uint8_t  USBFS_Endp_Busy[ DEF_UEP_NUM ];


/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Initializes the usbotg clock configuration.
 *
 * @return  none
 */
static void usb_dev_clocks_init( void )
{
    if( SystemCoreClock == 96000000 )
    {
        /* RCC_CFGR0_USBPRE Div2, 96MHz/2=48MHz */
        RCC->CFGR0 = (RCC->CFGR0 & (~RCC_CFGR0_USBPRE)) | RCC_USBPRE_0;
    }
    else if( SystemCoreClock == 72000000 )
    {
        /* RCC_CFGR0_USBPRE Div1_5, 72MHz/1.5=48MHz */
        RCC->CFGR0 = (RCC->CFGR0 & (~RCC_CFGR0_USBPRE)) | RCC_USBPRE_1;
    }
    else if( SystemCoreClock == 48000000 )
    {
        /* RCC_CFGR0_USBPRE Div1, 48MHz/=48MHz */
        RCC->CFGR0 = (RCC->CFGR0 & (~RCC_CFGR0_USBPRE));
    }

    /* nablesthe HB peripheral clock */
    RCC->HBPCENR |= RCC_USBFSEN;
}

/*********************************************************************
 * @fn      usb_ep_conf
 *
 * @brief   
 *
 * @return  none
 */
void usb_ep_conf(uint8_t ep_idx)
{
    switch (ep_idx) 
    {
        case 0x01:
            USBFSD->UEP4_1_MOD = (USBFSD->UEP4_1_MOD & 0x0F) | USBFS_UEP1_RX_EN; break;
        case 0x81:
            USBFSD->UEP4_1_MOD = (USBFSD->UEP4_1_MOD & 0x0F) | USBFS_UEP1_TX_EN; break;
        case 0x02:
            USBFSD->UEP2_3_MOD = (USBFSD->UEP2_3_MOD & 0xF0) | USBFS_UEP2_RX_EN; break;
        case 0x82:
            USBFSD->UEP2_3_MOD = (USBFSD->UEP2_3_MOD & 0xF0) | USBFS_UEP2_TX_EN; break;
        case 0x03:
            USBFSD->UEP2_3_MOD = (USBFSD->UEP2_3_MOD & 0x0F) | USBFS_UEP3_RX_EN; break;
        case 0x83:
            USBFSD->UEP2_3_MOD = (USBFSD->UEP2_3_MOD & 0x0F) | USBFS_UEP3_TX_EN; break;
        case 0x04:
            USBFSD->UEP4_1_MOD = (USBFSD->UEP4_1_MOD & 0xF0) | USBFS_UEP4_RX_EN; break;
        case 0x84:
            USBFSD->UEP4_1_MOD = (USBFSD->UEP4_1_MOD & 0xF0) | USBFS_UEP4_TX_EN; break;
        case 0x05:
            USBFSD->UEP5_6_MOD = (USBFSD->UEP5_6_MOD & 0xF0) | USBFS_UEP5_RX_EN; break;   
        case 0x85:
            USBFSD->UEP5_6_MOD = (USBFSD->UEP5_6_MOD & 0xF0) | USBFS_UEP5_TX_EN; break;   
        case 0x06:
            USBFSD->UEP5_6_MOD = (USBFSD->UEP5_6_MOD & 0x0F) | USBFS_UEP6_RX_EN; break;     
        case 0x86:
            USBFSD->UEP5_6_MOD = (USBFSD->UEP5_6_MOD & 0x0F) | USBFS_UEP6_TX_EN; break;
        case 0x07:
            USBFSD->UEP7_MOD = (USBFSD->UEP7_MOD & 0xF0) | USBFS_UEP7_RX_EN; break;    
        case 0x87:
            USBFSD->UEP7_MOD = (USBFSD->UEP7_MOD & 0xF0) | USBFS_UEP7_TX_EN; break;             
        default: break;
    }

    if( ep_idx & USB_REQ_TYP_IN )
    {
        /* IN */
        ep_idx = ep_idx % DEF_UEP_NUM;
        USB_SET_TX_LEN(ep_idx,0);    
        USB_SET_TX_CTRL(ep_idx, USBFS_UEP_T_RES_NAK);
    }
    else
    {
        /* OUT */
        USB_SET_RX_CTRL(ep_idx, USBFS_UEP_R_RES_ACK);
    }
}

/*********************************************************************
 * @fn      usb_dev_endpoint_init
 *
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
__WEAK void usb_dev_endpoint_init(void)
{

}

/*********************************************************************
 * @fn      USBFS_Device_Init
 *
 * @brief   Initializes USB device.
 *
 * @return  none
 */
void usb_dev_init( uint8_t sta )
{
    usb_dev_clocks_init();
    if( sta )
    {
        USBFSH->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;

        soft_delay_us( 10 );

        USBFSH->BASE_CTRL = 0x00;
        USBFSD->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;
        USBFSD->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;

        USBFSD->UEP0_DMA = (uint32_t)USBFS_EP0_Buf;
        USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
        USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
        usb_dev_endpoint_init();

        /* Clear End-points Busy Status */
        for( uint8_t i=0; i<DEF_UEP_NUM; i++ )
        {
            USBFS_Endp_Busy[ i ] = 0;
        }

        USBFSD->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;

        //NVIC_EnableIRQ PreemptionPriority = 1 , UART2_IRQn SubPriority = 0
        NVIC->IPRIOR[(uint32_t)(USBFS_IRQn)] = ( 1 << 7 ) | ( 0 << 5 );
        NVIC->IENR[((uint32_t)(USBFS_IRQn) >> 5)] = (1 << ((uint32_t)(USBFS_IRQn) & 0x1F));
        NVIC_EnableIRQ(USBFS_IRQn);
    }
    else
    {
        USBFSH->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
        soft_delay_us( 10 );
        USBFSD->BASE_CTRL = 0x00;
        NVIC_DisableIRQ( USBFS_IRQn );
    }
}


/*********************************************************************
 * @fn      usb_dev_non_standard_requset
 *
 * @brief   usb non-standard request processing 
 *
 * @return  none
 */ 
__WEAK uint8_t usb_dev_non_standard_requset(void)
{
    uint8_t errflag = 0;
    if( USBFS_SetupReqType & USB_REQ_TYP_CLASS ) /* USB Class Request */
    {
        /* Class requests */
        switch( USBFS_SetupReqCode )
        {
            case CDC_GET_LINE_CODING:  /* IN , bWlength = 7 */
                break;
            case CDC_SET_LINE_CODING:  /* OUT , bWlength = 7 */
                break;
            case CDC_SET_LINE_CTLSTE: /* OUT , bWlength = 0 */
                break;
            case CDC_SEND_BREAK:
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
 * @fn      usb_dev_standard_requset
 *
 * @brief   usb standard request processing 
 *
 * @return  none
 */
static uint8_t usb_dev_standard_requset(void)
{
    uint8_t ep_index;
    uint8_t len = 0;
    uint8_t errflag = 0;

    switch( USBFS_SetupReqCode )
    {
        /* get device/configuration/string/report/... descriptors */
        case USB_GET_DESCRIPTOR:  /* bmRequestType = 0x80 , IN */
            switch( (uint8_t)( USBFS_SetupReqValue >> 8 ) )  
            {
                /* get usb device descriptor */
                case USB_DESCR_TYP_DEVICE:
                    pUSBFS_Descr = MyDevDescr;
                    len = DEF_USBD_DEVICE_DESC_LEN;
                    break;
                /* get usb configuration descriptor */
                case USB_DESCR_TYP_CONFIG:
                    pUSBFS_Descr = MyCfgDescr;
                    len = DEF_USBD_CONFIG_DESC_LEN;
                    break;
                /* get usb string descriptor */
                case USB_DESCR_TYP_STRING:
                    switch( (uint8_t)( USBFS_SetupReqValue & 0xFF ) )
                    {
                        /* Descriptor 0, Language descriptor */
                        case DEF_STRING_DESC_LANG:
                            pUSBFS_Descr = MyLangDescr;
                            len = DEF_USBD_LANG_DESC_LEN;
                            break;

                        /* Descriptor 1, Manufacturers String descriptor */
                        case DEF_STRING_DESC_MANU:
                            pUSBFS_Descr = MyManuInfo;
                            len = DEF_USBD_MANU_DESC_LEN;
                            break;

                        /* Descriptor 2, Product String descriptor */
                        case DEF_STRING_DESC_PROD:
                            pUSBFS_Descr = MyProdInfo;
                            len = DEF_USBD_PROD_DESC_LEN;
                            break;

                        /* Descriptor 3, Serial-number String descriptor */
                        case DEF_STRING_DESC_SERN:
                            pUSBFS_Descr = MySerNumInfo;
                            len = DEF_USBD_SN_DESC_LEN;
                            break;

                        default:
                            errflag = 0xFF;
                            break;
                    }
                    break;

                default :
                    errflag = 0xFF;
                    break;
            }

            /* Copy Descriptors to Endp0 DMA buffer */
            if( USBFS_SetupReqLen > len )
            {
                USBFS_SetupReqLen = len;
            }

            if( USBFS_SetupReqLen )
            {
                len = (USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
                pUSBFS_Descr += len;
            }
            break;
        /* Get usb configuration now set */
        case USB_GET_CONFIGURATION: /* bmRequestType = 0x80 , IN */
            /* bRequest = USB_GET_CONFIGURATION, wValue = 0, wIndex = 0, wLength = 1 */
            USBFS_EP0_Buf[0] = USBFS_DevConfig;
            if ( USBFS_SetupReqLen > 1 )
            {
                USBFS_SetupReqLen = 1;
            }
            break;
        /* Set usb configuration to use */
        case USB_SET_CONFIGURATION: /* bmRequestType = 0x00 , OUT */
            /* bRequest = USB_SET_CONFIGURATION, wValue = configvaule, wIndex = 0, wLength = 0 */
            USBFS_DevConfig = (uint8_t)( USBFS_SetupReqValue & 0xFF );
            USBFS_DevEnumStatus = 0x01;
            break;
        /* Set usb address */
        case USB_SET_ADDRESS: /* bmRequestType = 0x00 , OUT */
            /* bRequest = USB_SET_ADDRESS, wValue = USB Address, wIndex = 0, wLength = 0 */
            USBFS_DevAddr = (uint8_t)( USBFS_SetupReqValue & 0xFF );
            break;
        /* Clear or disable one usb feature */
        case USB_CLEAR_FEATURE: /* bmRequestType = 0x00~0x02 , OUT */
            /* bRequest = USB_CLEAR_FEATURE */
            if ( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
            {
                /* bmRequestType = 0x00, wValue = feature, wIndex = 0, wLength = 0 */
                /* clear one device feature */
                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                {
                    /* clear usb sleep status, device not prepare to sleep */
                    USBFS_DevSleepStatus &= ~0x01;
                }
            }
            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
            {
                /* bmRequestType = 0x02, wValue = feature, wIndex = endponit, wLength = 0 */
                /* Clear End-point Feature */
                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                {
                    ep_index = (uint8_t)( USBFS_SetupReqIndex & 0x0F );
                    if( USBFS_SetupReqIndex & USB_REQ_TYP_IN )
                    {
                        /* Set End-point  IN NAK */
                        USB_SET_TX_CTRL(ep_index,USBFS_UEP_T_RES_NAK);
                    }
                    else
                    {
                        /* Set End-point OUT ACK */
                        USB_SET_RX_CTRL(ep_index,USBFS_UEP_R_RES_ACK);
                    }
                }
                else
                {
                    errflag = 0xFF;
                }
            }
            else
            {
                errflag = 0xFF;
            }
            break;

        /* set or enable one usb feature */
        case USB_SET_FEATURE: /* bmRequestType = 0x00~0x02 , OUT */
            /* bRequest = USB_SET_FEATURE */
            if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
            {
                /* bmRequestType = 0x00, wValue = feature, wIndex = 0, wLength = 0 */
                /* Set Device Feature */
                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                {
                    if( MyCfgDescr[ 7 ] & 0x20 )
                    {
                        /* Set Wake-up flag, device prepare to sleep */
                        USBFS_DevSleepStatus |= 0x01;
                    }
                    else
                    {
                        errflag = 0xFF;
                    }
                }
                else
                {
                    errflag = 0xFF;
                }
            }
            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
            {
                /* bmRequestType = 0x02, wValue = feature, wIndex = endponit, wLength = 0 */
                /* Set End-point Feature */
                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                {
                    ep_index = (uint8_t)( USBFS_SetupReqIndex & 0x0F );
                    /* Set end-points status stall */
                    if( USBFS_SetupReqIndex & USB_REQ_TYP_IN )
                    {
                        /* Set End-point IN STALL */
                        USB_SET_TX_CTRL(ep_index,(USB_GET_TX_CTRL(ep_index) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL);
                    }
                    else
                    {
                        /* Set End-point OUT STALL */
                        USB_SET_RX_CTRL(ep_index,(USB_GET_RX_CTRL(ep_index) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL);
                    }
                }
                else
                {
                    errflag = 0xFF;
                }
            }
            else
            {
                errflag = 0xFF;
            }
            break;
        /* This request allows the host to select another setting for the specified interface  */
        case USB_GET_INTERFACE: /* bmRequestType = 0x80 , IN */
            /* bRequest = USB_GET_INTERFACE, wValue = 0, wIndex = interface, wLength = 1 */
            USBFS_EP0_Buf[0] = 0x00;
            if ( USBFS_SetupReqLen > 1 )
            {
                USBFS_SetupReqLen = 1;
            }
            break;
        case USB_SET_INTERFACE: /* bmRequestType = 0x00 , OUT */
            /* bRequest = USB_SET_INTERFACE, wValue = vaule, wIndex = interface, wLength = 0 */
            break;
        /* host get status of specified device/interface/end-points */
        case USB_GET_STATUS: /* bmRequestType = 0x80 , IN */
            /* bRequest = USB_GET_STATUS, wValue = 0, wIndex = 0, wLength = 2 */
            USBFS_EP0_Buf[ 0 ] = 0x00;
            USBFS_EP0_Buf[ 1 ] = 0x00;
            if ( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
            {
                if( USBFS_DevSleepStatus & 0x01 )
                {
                    USBFS_EP0_Buf[ 0 ] = 0x02;
                }
            }
            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
            {
                ep_index = (uint8_t)( USBFS_SetupReqIndex & 0x0F );
                if( USBFS_SetupReqIndex & USB_REQ_TYP_IN )
                {
                    if( ( USB_GET_TX_CTRL(ep_index) & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL )
                    {
                        USBFS_EP0_Buf[ 0 ] = 0x01;
                    }
                }
                else
                {
                    if( ( USB_GET_RX_CTRL(ep_index) & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL )
                    {
                        USBFS_EP0_Buf[ 0 ] = 0x01;
                    }
                }
            }
            else
            {
                errflag = 0xFF;
            }

            if( USBFS_SetupReqLen > 2 )
            {
                USBFS_SetupReqLen = 2;
            }

            break;

        default:
            errflag = 0xFF;
            break;
    }
    return errflag;
}

/*********************************************************************
 * @fn      usb_dev_setup_handler
 *
 * @brief   Setup stage processing 
 *
 * @return  none
 */
static uint8_t usb_dev_setup_handler(void)
{
    uint8_t len = 0;
    uint8_t  errflag = 0;
    ep0_send_zlp = 0 ;
    if ( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
    {
        if((USBFS_SetupReqLen == 0) || (USBFS_SetupReqType & USB_REQ_TYP_IN ))
        {
            /* usb non-standard request processing */
            errflag = usb_dev_non_standard_requset();
        }
    }
    else
    {
        /* usb standard request processing */
        errflag = usb_dev_standard_requset();
    }

    /* errflag = 0xFF means a request not support or some errors occurred, else correct */
    if( errflag == 0xff)
    {
        /* if one request not support, return stall */
        USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_STALL;
        USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG|USBFS_UEP_R_RES_STALL;
    }
    else
    {
        /* end-point 0 data Tx/Rx */
        if( USBFS_SetupReqType & USB_REQ_TYP_IN )
        {
            /* IN, tx */
            if(((USBFS_SetupReqLen % DEF_USBD_UEP0_SIZE) == 0 ) && (USBFS_SetupReqLen != 0))
            {
                ep0_send_zlp = 1;
            }

            len = (USBFS_SetupReqLen>DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
            USBFS_SetupReqLen -= len;
            USBFSD->UEP0_TX_LEN = len;
            USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
        }
        else 
        {
            /* OUT, rx */
            if( USBFS_SetupReqLen == 0 )
            {
                /* Rx completely , Send status to host */
                USBFSD->UEP0_TX_LEN  = 0; 
                USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
            }
            else
            {
                USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
            }
        }
    }
    return errflag;
}

/*********************************************************************
 * @fn      usb_dev_ep0_in_handler
 *
 * @brief   endpoint 0 in stage processing 
 *
 * @return  none
 */
static void usb_dev_ep0_in_handler(void)
{
    if( USBFS_SetupReqLen == 0 )
    {
        if( ep0_send_zlp )
        {
            /* Send zero length packet */
            ep0_send_zlp = 0 ;
            USBFSD->UEP0_TX_LEN = 0;
            USBFSD->UEP0_TX_CTRL ^= USBFS_UEP_T_TOG;
        }
        else
        {
            /* Satisfying three conditions will jump here.
            * 1. send status completely
            * 2. send zlp completely
            * 3. send last data completely.
            */
            /* if all data has sent completely, start reading out status, expcet DATA1 */
            USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
        }
    }
    else
    {
        uint8_t len;
        /* Copy Descriptors to Endp0 DMA buffer */
        len = USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
        memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
        USBFS_SetupReqLen -= len;
        pUSBFS_Descr += len;
        USBFSD->UEP0_TX_LEN = len;
        USBFSD->UEP0_TX_CTRL ^= USBFS_UEP_T_TOG;
        USBFSD->UEP0_TX_CTRL &= ~USBFS_UEP_T_RES_MASK;    
    }

    if(((USBFS_SetupReqType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) && (USBFS_SetupReqCode == USB_SET_ADDRESS))
    {
        /* Set USB Address */
        USBFSD->DEV_ADDR = (USBFSD->DEV_ADDR & USBFS_UDA_GP_BIT) | USBFS_DevAddr;
    }
}


/*********************************************************************
 * @fn      usbfs_dev_isr
 *
 * @brief   This function handles HD-FS exception.
 *
 * @return  none
 */
void usbfs_dev_isr(void)
{
    uint32_t ep_idx = 0, token;
    uint8_t intflag;
    intflag = USBFSD->INT_FG;
    if( intflag & USBFS_UIF_TRANSFER )
    {
        token = USBFSD->INT_ST & USBFS_UIS_TOKEN_MASK;
        ep_idx = USBFSD->INT_ST & USBFS_UIS_ENDP_MASK;
        switch (token)
        {
            /* Setup stage processing */
            case USBFS_UIS_TOKEN_SETUP:
                USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_NAK;
                USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG|USBFS_UEP_R_RES_NAK;
                /* Store All Setup Values */
                USBFS_SetupReqType  = pUSBFS_SetupReqPak->bRequestType;
                USBFS_SetupReqCode  = pUSBFS_SetupReqPak->bRequest;
                USBFS_SetupReqLen   = pUSBFS_SetupReqPak->wLength;
                USBFS_SetupReqValue = pUSBFS_SetupReqPak->wValue;
                USBFS_SetupReqIndex = pUSBFS_SetupReqPak->wIndex;
                usb_dev_setup_handler();
                break;
            /* data-in stage processing */
            case USBFS_UIS_TOKEN_IN:
                ep_idx = ep_idx % DEF_UEP_NUM;
                USB_SET_TX_CTRL(ep_idx, (USB_GET_TX_CTRL(ep_idx) & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK);
                if( ep_idx == 0  )
                {
                    /* end-point 0 data in interrupt */
                    usb_dev_ep0_in_handler();
                }
                else
                {
                    USB_SET_TX_CTRL(ep_idx, USB_GET_TX_CTRL(ep_idx)^USBFS_UEP_T_TOG);
                    if( EP_IN_Callback[ep_idx-1] != NULL )
                    {
                        EP_IN_Callback[ep_idx-1]();
                    }
                    USBFS_Endp_Busy[ep_idx] = 0;
                }
                break;
            /* data-out stage processing */
            case USBFS_UIS_TOKEN_OUT:
                if (intflag & USBFS_UIS_TOG_OK) 
                {
                    /* end-point 0 data out interrupt */
                    if(ep_idx == 0)
                    {
                        USBFS_SetupReqLen = USBFS_SetupReqLen - USBFSD->RX_LEN;
                        usb_dev_setup_handler();
                    }
                    else
                    {
                        USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_NAK);
                        if( EP_OUT_Callback[ep_idx-1] != NULL )
                        {
                            EP_OUT_Callback[ep_idx-1]();
                            USB_SET_RX_CTRL(ep_idx, USB_GET_RX_CTRL(ep_idx)^USBFS_UEP_R_TOG);
                            USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK);
                        }
                    }
                }
                break;
            default :
                break;
        }
        USBFSD->INT_FG = USBFS_UIF_TRANSFER;
    }
    else if( intflag & USBFS_UIF_BUS_RST )
    {
        /* usb reset interrupt processing */
        USBFSD->DEV_ADDR = 0;
        USBFSD->UEP0_DMA = (uint32_t)USBFS_EP0_Buf;
        USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
        USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
        usb_dev_endpoint_init();
        /* Clear End-points Busy Status */
        for( uint8_t i=0; i<DEF_UEP_NUM; i++ )
        {
            USBFS_Endp_Busy[ i ] = 0;
        }
        //UART2_ParaInit( 1 );
        USBFSD->INT_FG = USBFS_UIF_BUS_RST;
    }
    else if( intflag & USBFS_UIF_SUSPEND )
    {
        /* usb suspend interrupt processing */
        if ( USBFSD->MIS_ST & USBFS_UMS_SUSPEND )
        {
            USBFS_DevSleepStatus |= 0x02;
            if( USBFS_DevSleepStatus == 0x03 )
            {
                /* Handling usb sleep here */
            }
        }
        else
        {
            USBFS_DevSleepStatus &= ~0x02;
        }
        USBFSD->INT_FG = USBFS_UIF_SUSPEND;
    }
    else
    {
        /* other interrupts */
        USBFSD->INT_FG = intflag;
    }
}

/*********************************************************************
 * @fn      USBFS_Send_Resume
 *
 * @brief   USBFS device sends wake-up signal to host
 *
 * @return  none
 */
void usb_dev_send_resume(void)
{
    USBFSD->UDEV_CTRL ^= USBFS_UD_LOW_SPEED;
    soft_delay_ms( 8 );
    USBFSD->UDEV_CTRL ^= USBFS_UD_LOW_SPEED;
    soft_delay_ms( 1 );
}

/*********************************************************************
 * @fn      EP1_IN_Callback
 *
 * @brief   IN Endpoint 1 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP1_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP2_IN_Callback
 *
 * @brief   IN Endpoint 2 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP2_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP3_IN_Callback
 *
 * @brief   IN Endpoint 3 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP3_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP4_IN_Callback
 *
 * @brief   IN Endpoint 4 Callback Function
 *
 * @return  none
 */ 
void EP4_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP5_IN_Callback
 *
 * @brief   IN Endpoint 5 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP5_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP6_IN_Callback
 *
 * @brief   IN Endpoint 6 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP6_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP7_IN_Callback
 *
 * @brief   IN Endpoint 7 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP7_IN_Callback(void)
{

}

/*********************************************************************
 * @fn      EP1_OUT_Callback
 *
 * @brief   OUT Endpoint 1 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP1_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP2_OUT_Callback
 *
 * @brief   OUT Endpoint 2 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP2_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP3_OUT_Callback
 *
 * @brief   OUT Endpoint 3 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP3_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP4_OUT_Callback
 *
 * @brief   OUT Endpoint 4 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP4_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP5_OUT_Callback
 *
 * @brief   OUT Endpoint 5 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP5_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP6_OUT_Callback
 *
 * @brief   OUT Endpoint 6 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP6_OUT_Callback(void)
{

}

/*********************************************************************
 * @fn      EP7_OUT_Callback
 *
 * @brief   OUT Endpoint 7 Callback Function
 *
 * @return  none
 */ 
__WEAK void EP7_OUT_Callback(void)
{

}

