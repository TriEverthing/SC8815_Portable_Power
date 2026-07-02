/********************************** (C) COPYRIGHT *******************************
* File Name          : PD_process.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/10/25
* Description        : This file provides all the PD firmware functions.
*********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32l103x_bsp.h"
#include "usbpd_sink.h"

#define USBPD_DEBUG_ON      1

#if USE_FreeRTOS
SemaphoreHandle_t UsbpdSem;
void USBPD_IRQHandler(void) __attribute__((interrupt()));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt()));
#else
void USBPD_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#endif

__attribute__ ((aligned(4))) uint8_t USBPD_Rx_Buf[ 34 ];                            /* PD receive buffer */
__attribute__ ((aligned(4))) uint8_t USBPD_Tx_Buf[ 34 ];                            /* PD send buffer */

/******************************************************************************/
//for Control Message 16bits
uint8_t PD_Ack_Buf[ 2 ];                                                            /* PD-ACK buffer */

uint8_t USBPD_Ms_Cnt = 0 ;
uint8_t USBPD_Ms_Dlt;                                                              /* System timer millisecond timing this interval value */

PD_CONTROL PD_Ctl;                                                                  /* PD Control Related Structures */

/* Data Message 2Byte Message Header + 8Data Object(4Byte) = 2 + 28 = 30  */
uint8_t  Adapter_SrcCap[ 30 ];                                                      /* SrcCap message from the adapter */

uint8_t  PDO_Len;

/* Fixed Supply Power Data Object
*  Byte0 : 
*  B07-B00 : Maximum Current[7:0] in 10mA units
*  Byte1 :
*  B15-B10 : Voltage[5:0] in 50mV units
*  B09-B08 : Maximum Current[9:8] in 10mA units
*  Byte2 :
*  B23 : EPR Capable , Set to ＆1 if EPR Capable
*  B22 : Reserved , 0
*  B21-B20 : Peak Current
*  B19-B16 : Voltage[9:6] in 50mV units
*  Byte3 :
*  B31-B30 : Fixed supply , 00-(Vmin=Vmax)
*  B29 : Dual-Role Power , Set to ＆1＊ for Dual-Role Power device.
*  B28 : USB Suspend Supported , Set to ＆1＊ if USB suspend is supported.
*  B27 : Unconstrained Power , Set to ＆1＊ if unconstrained power is available.
*  B26 : USB Communications Capable , Set to ＆1＊ if capable of USB Communications capable
*  B25 : Dual-Role Data , Set to ＆1＊ for Dual-Role Power device.
*  B24 : Unchunked Extended Messages Supported , Set to ＆1 if Unchunked Extended Messages are supported.
*/

/* Battery Supply PDO
*  Byte0 : 
*  B07-B00 : Maximum Allowable Power[7:0] in 10mA units
*  Byte1 : 
*  B15-B10 : Minimum Voltage[5:0] in 50mV units
*  B09-B08 : Maximum Allowable Power[9:8] in 10mA units
*  Byte2 : 
*  B23-B20 : Maximum Voltage[3:0] in 50mV units
*  B19-B16 : Minimum Voltage[9:6] in 50mV units
*  Byte3 :
*  B31-B30 : 01-Battery
*  B29-B24 : Maximum Voltage[5:0] in 50mV units
*/

/* Variable Supply (non-Battery) Power Data Object
*  Byte0 : 
*  B07-B00 : Maximum Current[7:0] in 10mA units
*  Byte1 : 
*  B15-B10 : Minimum Voltage[5:0] in 50mV units
*  B09-B08 : Maximum Current[9:8] in 10mA units
*  Byte2 : 
*  B23-B20 : Maximum Voltage[3:0] in 50mV units
*  B19-B16 : Minimum Voltage[9:6] in 50mV units
*  Byte3 :
*  B31-B30 : 10-Variable Supply (non-Battery)
*  B29-B24 : Maximum Voltage[5:0] in 50mV units
*/

/* SPR Programmable Power Supply APDO
*  Byte0 : 
*  B07 : Reserved
*  B06-B00 : Maximum Current[7:0] in 50mA units
*  Byte1 : 
*  B15-B08 : Minimum Voltage[7:0] in 100mV units
*  Byte2 : 
*  B23-B17 : Maximum Voltage[6:0] in 100mV units
*  B16 : Reserved
*  Byte3 :
*  B31-B30 : 11b 每 Augmented Power Data Object (APDO)
*  B29-B28 : 00-SPR PPS , 01-EPR AVS , 10-SPR AVS , 11-Reserved
*  B27 : PPS Power Limited
*  B26-B25 : Reserved
*  B24 : Maximum Voltage[7] in 100mV units
*/

/* SPR Adjustable Voltage Supply APDO
*  Byte0 : 
*  B07 : Reserved
*  B06-B00 : Maximum Current[7:0] 20V
*  Byte1 : 
*  B15-B10 : Maximum Current[5:0] 15V, For 9V 每 15V range: Maximum current in 10mA units equal to the Maximum Current field of the 15V Fixed Supply PDO
*  B09-B08 : Maximum Current[9:8] 20V
*  Byte2 : 
*  B23-B20 : Reserved
*  B19-B16 : Maximum Current[9:6] 15V, For 9V 每 15V range: Maximum current in 10mA units equal to the Maximum Current field of the 15V Fixed Supply PDO
*  Byte3 :
*  B31-B30 : 11b 每 Augmented Power Data Object (APDO)
*  B29-B28 : 00-SPR PPS , 01-EPR AVS , 10-SPR AVS , 11-Reserved
*  B27-B24 : Reserved
*/

/* EPR Adjustable Voltage Supply APDO
*  Byte0 : 
*  B07-B00 : PDP in 1W increments
*  Byte1 : 
*  B15-B08 : Maximum Voltage[7:0] Maximum voltage in 100mV increments
*  Byte2 : 
*  B23-B17 : Maximum Voltage[6:0] Maximum voltage in 100mV increments
*  B16 : Reserved
*  Byte3 :
*  B31-B30 : 11b 每 Augmented Power Data Object (APDO)
*  B29-B28 : 00-SPR PPS , 01-EPR AVS , 10-SPR AVS , 11-Reserved
*  B27-B26 : Reserved
*  B25-B24 : Maximum Voltage[8:7] Maximum voltage in 100mV increments
*/

/* Source Capabilities Table , little-endian */
uint8_t SrcCap_5V3A_Tab[ 4 ] = 
{ 
    /* B07-B00 : Maximum Current[7:0] in 10mA units
    */
    0x2C , 
    /* B15-B10 : Voltage[5:0] in 50mV units
    *  B09-B08 : Maximum Current[9:8] in 10mA units
    */
    0x91 , 
    /* B23 : EPR Capable , Set to ＆1 if EPR Capable
    *  B22 : Reserved , 0
    *  B21-B20 : Peak Current
    *  B19-B16 : Voltage[9:6] in 50mV units
    */
    0x01 , 
    /* B31-B30 : Fixed supply , 00-(Vmin=Vmax) , 01-Battery , 10-Variable Supply (non-Battery) , 11-Augmented Power Data Object (APDO)
    *  B29 : Dual-Role Power , Set to ＆1＊ for Dual-Role Power device.
    *  B28 : USB Suspend Supported , Set to ＆1＊ if USB suspend is supported.
    *  B27 : Unconstrained Power , Set to ＆1＊ if unconstrained power is available.
    *  B26 : USB Communications Capable , Set to ＆1＊ if capable of USB Communications capable
    *  B25 : Dual-Role Data , Set to ＆1＊ for Dual-Role Power device.
    *  B24 : Unchunked Extended Messages Supported , Set to ＆1 if Unchunked Extended Messages are supported.
    */
    0x3E 
};
uint8_t SrcCap_5V2A_Tab[ 4 ] = { 0xC8, 0x90, 0x01, 0x3E };
/* Sink Capabilities Table */
uint8_t SinkCap_5V1A_Tab[ 4 ] = { 0x64, 0x90, 0x01, 0x36 };

/* PD3.0 */
uint8_t SrcCap_Ext_Tab[ 28 ] =
{
    0x18, 0x80, 0x63, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x07, 0x03,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03,
    0x00, 0x12, 0x00, 0x00,
};

uint8_t Status_Ext_Tab[ 8 ] =
{
    0x06, 0x80, 0x16, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

uint8_t usbpd_is_initialized = 0;

void usbpd_isr( uint8_t status )
{
    if(status & IF_RX_ACT)
    {
        if( ( status & MASK_PD_STAT ) == PD_RX_SOP0 )
        {
            if( USBPD->BMC_BYTE_CNT >= 6 )
            {
                /* If GOODCRC, do not answer and ignore this reception */
                if( ( USBPD->BMC_BYTE_CNT != 6 ) || ( ( USBPD_Rx_Buf[ 0 ] & 0x1F ) != DEF_TYPE_GOODCRC ) )
                {
                    soft_delay_us(30);                       /* Delay 30us, answer GoodCRC */
                    PD_Ack_Buf[ 0 ] = 0x41;
                    PD_Ack_Buf[ 1 ] = ( USBPD_Rx_Buf[ 1 ] & 0x0E ) | PD_Ctl.Flag.Bit.Auto_Ack_PRRole;
                    USBPD->CONFIG |= IE_TX_END ;
                    USBPD_Phy_SendPack( 0, PD_Ack_Buf, 2, UPD_SOP0 );
#if USBPD_DEBUG_ON
                    dbg_printf("Send GoodCRC\r\n");
#endif
                }
            }
        }
    }

    if( status & IF_TX_END )
    {
#if USBPD_DEBUG_ON
        dbg_printf("IRQ_Ticks=%d.\r\n",xTaskGetTickCount());
        dbg_printf("Packet send completion\r\n");
#endif
        /* Packet send completion interrupt (GoodCRC send completion interrupt only) */
        USBPD->PORT_CC1 &= ~CC_LVE;
        USBPD->PORT_CC2 &= ~CC_LVE;

        /* Interrupts are turned off and can be turned on after the main function has finished processing the data */
        NVIC_DisableIRQ(USBPD_IRQn);
        /* Packet received flag */
        PD_Ctl.Flag.Bit.Msg_Recvd = 1;                                          
    }

    if( status & IF_RX_RESET)
    {
        USBPD_Sink_Init( );
#if USBPD_DEBUG_ON
        dbg_printf("IF_RX_RESET\r\n");
#endif
    }
    if( status & BUF_ERR)
    {
#if USBPD_DEBUG_ON
        dbg_printf("BUFERR\r\n");
#endif
    }
}

uint8_t status_isr ;

/*********************************************************************
 * @fn      USBPD_IRQHandler
 *
 * @brief   This function handles USBPD interrupt.
 *
 * @return  none
 */
void USBPD_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE ;
    status_isr = USBPD->STATUS ;
    USBPD->STATUS = status_isr;
#if USE_FreeRTOS
    xSemaphoreGiveFromISR( UsbpdSem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
#else
    usbpd_isr(status_isr);
#endif
}

#if USE_FreeRTOS
void USBPD_ISR_Task(void *pvParameters)
{
    /* Initialize PD registers and states. */
    usbpd_init();

    while(1)
    {
        if(xSemaphoreTake(UsbpdSem, portMAX_DELAY) == pdTRUE) 
        {
            usbpd_isr(status_isr);
        }
    }
}
#endif

/*********************************************************************
 * @fn      TIM1_UP_IRQHandler
 *
 * @brief   This function handles TIM1 interrupt.
 *
 * @return  none
 */
void TIM1_UP_IRQHandler(void)
{
    USBPD_Ms_Cnt ++ ;
    TIM1->INTFR = ~TIM_UIF ;
}


/*********************************************************************
 * @fn      PD_SRC_Init
 *
 * @brief   This function uses to initialize SRC mode.
 *
 * @return  none
 */
void USBPD_Src_Init(void)
{
    PD_Ctl.Flag.Bit.PR_Role = 1;                                          /* SRC mode */
    PD_Ctl.Flag.Bit.Auto_Ack_PRRole = 1;                                  /* Default auto-responder role is SRC */
    USBPD->PORT_CC1 = CC_CMP_66 | CC_PU_330 ;                             //Comparators voltage 0.66V , pull up current 330uA
    USBPD->PORT_CC2 = CC_CMP_66 | CC_PU_330 ;
}

/*********************************************************************
 * @fn      PD_SINK_Init
 *
 * @brief   This function uses to initialize SNK mode.
 *
 * @return  none
 */
void USBPD_Sink_Init(void)
{
    PD_Ctl.Flag.Bit.PR_Role = 0;                                          /* SINK mode */
    PD_Ctl.Flag.Bit.Auto_Ack_PRRole = 0;                                  /* Default auto-responder role is SINK */
    // USBPD->PORT_CC1 = CC_CMP_66 | CC_PD ;   
    // USBPD->PORT_CC2 = CC_CMP_66 | CC_PD ;
    USBPD->PORT_CC1 = CC_CMP_66 ;   
    USBPD->PORT_CC2 = CC_CMP_66 ;
}

/*********************************************************************
 * @fn      PD_PHY_Reset
 *
 * @brief   This function uses to reset PD PHY.
 *
 * @return  none
 */
void USBPD_PHY_Reset(void)
{
    USBPD_Sink_Init();
    PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                                     /* PD disconnection detection is enabled by default */
    PD_Ctl.PD_State = STA_IDLE;                                           /* Set idle state */
    PD_Ctl.Flag.Bit.PD_Comm_Succ = 0;
}

/*********************************************************************
 * @fn      USBPD_Timer_Init
 *
 * @brief   USBPD_Timer_Init.
 *
 * @return  none
 */
void USBPD_Timer_Init(void)
{
    //PD Timer use TIM1
    //TIM1 clock enable
    RCC->PB2PCENR |= RCC_TIM1EN ;

    //Counter Mode Up , TIM_ClockDivision ,  DIV1 
    TIM1->CTLR1 = ( TIM1->CTLR1 & ( ~( TIM_DIR | TIM_CMS | TIM_CTLR1_CKD ))) ;
    //TIM Prescaler , 96MHz / DIV1 / 96 = 1MHz
    TIM1->PSC = 96 - 1 ;
    //TIM Period 1KHz , Automatic reload registers
    TIM1->ATRLR = 999 ;
    TIM1->RPTCR = 0x00 ;
    TIM1->SWEVGR = TIM_UG ;

    //Clear UIF Flag
    TIM1->INTFR = ~TIM_UIF ;

    //Enable interrupt
    //TIM1_UP_IRQn PreemptionPriority = 1 , UART2_IRQn SubPriority = 1
    NVIC->IPRIOR[(uint32_t)(TIM1_UP_IRQn)] = ( 1 << 7 ) | ( 1 << 5 );
    NVIC->IENR[((uint32_t)(TIM1_UP_IRQn) >> 5)] = (1 << ((uint32_t)(TIM1_UP_IRQn) & 0x1F));

    //Enables TIM update interrupts.
    TIM1->DMAINTENR |= TIM_UIF ;
    //Enables TIM peripheral.
    TIM1->CTLR1 |= TIM_CEN ;
}

/*********************************************************************
 * @fn      usbpd_init
 *
 * @brief   This function uses to initialize PD registers and states.
 *
 * @return  none
 */
void usbpd_init(void)
{
    if( usbpd_is_initialized == 0 )
    {
#if USE_FreeRTOS
        /* Creates a new binary semaphore, initialize vaule 0 */
        UsbpdSem = xSemaphoreCreateBinary();
#endif

        //GPIO_InitTypeDef GPIO_InitStructure = {0};
        //GPIOB clock enable , AFIO clock enable
        RCC->PB2PCENR |= RCC_IOPBEN | RCC_AFIOEN ;
        //USBPD clock enable
        RCC->HBPCENR |= RCC_USBPDEN ;
        //PB6,PB7 floating input
        GPIOB->CFGLR = ( GPIOB->CFGLR  & 0x00FFFFFF ) | ( GPIO_CFGLR_CNF6_0 | GPIO_CFGLR_CNF7_0 ) ;
        //USBPD high threshold input mode
        AFIO->CR |= USBPD_IN_HVT ;
        //USBPD DMA Enable
        USBPD->CONFIG = PD_DMA_EN | PD_FILT_ED ;
        //clear flag
        USBPD->STATUS = BUF_ERR | IF_RX_BIT | IF_RX_BYTE | IF_RX_ACT | IF_RX_RESET | IF_TX_END ;
        /* Initialize all variables */
        memset( &PD_Ctl.PD_State , 0x00 , sizeof( PD_CONTROL ) );
        Adapter_SrcCap[ 0 ] = 1 ;
        memcpy( &Adapter_SrcCap[ 1 ] , SrcCap_5V3A_Tab , 4 );

        USBPD_PHY_Reset();
        //Enable interrupt
        //USBPD_IRQn PreemptionPriority = 0 , UART2_IRQn SubPriority = 0
        NVIC->IPRIOR[(uint32_t)(USBPD_IRQn)] = ( 0 << 7 ) | ( 0 << 5 );
        NVIC->IENR[((uint32_t)(USBPD_IRQn) >> 5)] = (1 << ((uint32_t)(USBPD_IRQn) & 0x1F));
        //enter reception mode
        USBPD_Rx_Mode();

#if USE_FreeRTOS == 0
        USBPD_Timer_Init();
#endif
    }
    usbpd_is_initialized = 1;
}

/*********************************************************************
 * @fn      PD_Rx_Mode
 *
 * @brief   This function uses to enter reception mode.
 *
 * @return  none
 */
void USBPD_Rx_Mode( void )
{
    USBPD->CONFIG |= PD_ALL_CLR;
    USBPD->CONFIG &= ~PD_ALL_CLR;
    USBPD->CONFIG |= IE_RX_ACT | IE_RX_RESET | PD_DMA_EN ;
    USBPD->DMA = (uint32_t)(uint8_t *)USBPD_Rx_Buf;
    USBPD->CONTROL &= ~PD_TX_EN;
    USBPD->BMC_CLK_CNT = UPD_TMR_RX_96M ;
    USBPD->CONTROL |= BMC_START ;
    //Enabel USBPD_IRQn
    NVIC->IENR[((uint32_t)(USBPD_IRQn) >> 5)] = (1 << ((uint32_t)(USBPD_IRQn) & 0x1F));
}

/*********************************************************************
 * @fn      PD_Detect
 *
 * @brief   This function uses to detect CC connection.
 *
 * @return  0:No connection; 1:CC1 connection; 2:CC2 connection
 */
uint8_t USBPD_Detect(void)
{
    uint8_t  ret = 0;
    uint8_t  cmp_cc1 = 0;
    uint8_t  cmp_cc2 = 0;

    if(PD_Ctl.Flag.Bit.Connected)                                       /* Detect disconnection */
    {
        /* According to the usage scenario of PD SNK, whether
         * it is removed or not should be determined by detecting
         * the Vbus voltage, this code only shows the detection
         * and the subsequent communication flow. */
    }
    else                                                                /* Detect insertion */
    {
        USBPD->PORT_CC1 &= ~( CC_CE | PA_CC_AI );
        USBPD->PORT_CC1 |= CC_CMP_22 ; //0.22V
        soft_delay_us(2);
        if( USBPD->PORT_CC1 & PA_CC_AI )
        {
            cmp_cc1 |= bCC_CMP_22 ;
        }

        USBPD->PORT_CC2 &= ~( CC_CE | PA_CC_AI );
        USBPD->PORT_CC2 |= CC_CMP_22 ; //0.22V
        soft_delay_us(2);
        if( USBPD->PORT_CC2 & PA_CC_AI )
        {
            cmp_cc2 |= bCC_CMP_22;
        }

        if( (USBPD->PORT_CC1 & CC_PU_80 ) == 0 )
        {
            if ((cmp_cc1 & bCC_CMP_22) == bCC_CMP_22)
            {
                ret = 1;
            }
            if ((cmp_cc2 & bCC_CMP_22) == bCC_CMP_22)
            {
                if( ret )
                {
                    ret = 1;   /* Huawei A to C cable has two pull-up resistors */
                }
                else
                {
                    ret = 2;
                }
            }
        }
        else
        {
            /* SRC mode insertion detection */
        }
    }
    return( ret );
}

/*********************************************************************
 * @fn      PD_Det_Proc
 *
 * @brief   This function uses to process the return value of PD_Detect.
 *
 * @return  none
 */
void USBPD_Det_Proc(void)
{
    uint8_t  status;

    if( PD_Ctl.Flag.Bit.Connected )
    {
        /* PD is connected, detect its disconnection */

        /* According to the usage scenario of PD SNK, whether
         * it is removed or not should be determined by detecting
         * the Vbus voltage, this code only shows the detection
         * and the subsequent communication flow. */

    }
    else
    {
        /* PD disconnected, check connection */
        status = USBPD_Detect( );
        /* Determine connection status */
        if( status == 0 )
        {
            PD_Ctl.Det_Cnt = 0;
        }
        else
        {
            PD_Ctl.Det_Cnt++;
        }

        if( PD_Ctl.Det_Cnt >= 5 )
        {
            PD_Ctl.Det_Cnt = 0;
            PD_Ctl.Flag.Bit.Connected = 1;
            if( PD_Ctl.Flag.Bit.Stop_Det_Chk == 0 )
            {
                if( ((USBPD->PORT_CC1 & CC_PU_Mask)==0) || ((USBPD->PORT_CC2 & CC_PU_Mask)==0) )
                {
                    /* Select the corresponding PD channel */
                    if( status == 1 ) //CC1
                    {
                        USBPD->CONFIG &= ~CC_SEL;
                    }
                    else //CC2
                    {
                        USBPD->CONFIG |= CC_SEL;
                    }
                    PD_Ctl.PD_State = STA_SRC_CONNECT;
#if USBPD_DEBUG_ON
                    dbg_printf("CC%d SRC Connect\r\n",status);
#endif
                }

                PD_Ctl.PD_Comm_Timer = 0;
            }
        }
    }
}

/*********************************************************************
 * @fn      PD_Phy_SendPack
 *
 * @brief   This function uses to send PD data.
 *
 * @return  none
 */
void USBPD_Phy_SendPack( uint8_t mode, uint8_t *pbuf, uint8_t len, uint8_t sop )
{

    if ((USBPD->CONFIG & CC_SEL) == CC_SEL )
    {
        USBPD->PORT_CC2 |= CC_LVE; //use CC2 Pin
    }
    else
    {
        USBPD->PORT_CC1 |= CC_LVE; //use CC1 Pin
    }

    //BMC sample clocks counter
    USBPD->BMC_CLK_CNT = UPD_TMR_TX_96M;

    USBPD->DMA = (uint32_t)(uint8_t *)pbuf;

    USBPD->TX_SEL = sop;

    USBPD->BMC_TX_SZ = len;
    USBPD->CONTROL |= PD_TX_EN;
    USBPD->STATUS &= BMC_AUX_INVALID;

#if USE_FreeRTOS
    taskENTER_CRITICAL();
#endif

    USBPD->CONTROL |= BMC_START;

    /* Determine if you need to wait for the send to complete */
    if( mode )
    {
        /* Wait for the send to complete, this will definitely complete, no need to do a timeout */
        while( (USBPD->STATUS & IF_TX_END) == 0 );
        USBPD->STATUS |= IF_TX_END;

        if((USBPD->CONFIG & CC_SEL) == CC_SEL )
        {
            USBPD->PORT_CC2 &= ~CC_LVE;
        }
        else
        {
            USBPD->PORT_CC1 &= ~CC_LVE;
        }

        /* Switch to receive ready to receive GoodCRC */
        USBPD->CONFIG |=  PD_ALL_CLR ;
        USBPD->CONFIG &= ~( PD_ALL_CLR );
        USBPD->CONTROL &= ~ ( PD_TX_EN );
        USBPD->DMA = (uint32_t)(uint8_t *)USBPD_Rx_Buf;
        USBPD->BMC_CLK_CNT = UPD_TMR_RX_96M;
        USBPD->CONTROL |= BMC_START;
    }
    
#if USE_FreeRTOS
    taskEXIT_CRITICAL();
#endif
}

/*********************************************************************
 * @fn      PD_Load_Header
 *
 * @brief   This function uses to load pd header packets.
 *
 * @return  none
 */
void USBPD_Load_Header( uint8_t ex, uint8_t msg_type )
{
    /* Message Header
       BIT15:    Extended;
       BIT14-12: Number of Data Objects
       BIT11--9: Message ID
       BIT8:     PortPower Role/Cable Plug  0: SINK; 1: SOURCE
       BIT7---6: Revision, 00: V1.0; 01: V2.0; 10: V3.0;
       BIT5:     Port Data Role, 0: UFP; 1: DFP
       BIT4---0: Message Type
    */
    USBPD_Tx_Buf[ 0 ] = msg_type;
    if( PD_Ctl.Flag.Bit.PD_Role )
    {
        USBPD_Tx_Buf[ 0 ] |= 0x20;
    }
    if( PD_Ctl.Flag.Bit.PD_Version )
    {
        /* PD3.0 */
        USBPD_Tx_Buf[ 0 ] |= 0x80;
    }
    else
    {
        /* PD2.0 */
        USBPD_Tx_Buf[ 0 ] |= 0x40;
    }

    USBPD_Tx_Buf[ 1 ] = PD_Ctl.Msg_ID & 0x0E;
    if( PD_Ctl.Flag.Bit.PR_Role )
    {
        USBPD_Tx_Buf[ 1 ] |= 0x01;
    }
    if( ex )
    {
        USBPD_Tx_Buf[ 1 ] |= 0x80;
    }
}

/*********************************************************************
 * @fn      PD_Send_Handle
 *
 * @brief   This function uses to handle sending transactions.
 *
 * @return  0:success; 1:fail
 */
uint8_t USBPD_Send_Handle( uint8_t *pbuf, uint8_t len )
{
    uint8_t  pd_tx_trycnt;
    uint8_t  cnt;

    if( ( len % 4 ) != 0 )
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }
    if( len > 28 )
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }

    cnt = len >> 2;
    USBPD_Tx_Buf[ 1 ] |= ( cnt << 4 );
    for( cnt = 0; cnt != len; cnt++ )
    {
        USBPD_Tx_Buf[ 2 + cnt ] = pbuf[ cnt ];
    }

    pd_tx_trycnt = 4;
    while( --pd_tx_trycnt )                                                     /* Maximum 3 executions */
    {
        NVIC_DisableIRQ( USBPD_IRQn );
        USBPD_Phy_SendPack( 0x01, USBPD_Tx_Buf, ( len + 2 ), UPD_SOP0 );

        /* Set receive timeout 750US */
        cnt = 250;
        while( --cnt )
        {
            if( (USBPD->STATUS & IF_RX_ACT) == IF_RX_ACT)
            {
                USBPD->STATUS |= IF_RX_ACT;
                if( ( USBPD->BMC_BYTE_CNT == 6 ) && ( ( USBPD_Rx_Buf[ 0 ] & 0x1F ) == DEF_TYPE_GOODCRC ) )
                {
                    PD_Ctl.Msg_ID += 2;
                    break;
                }
            }
            soft_delay_us( 3 );
        }
        if( cnt !=0 )
        {
            break;
        }
    }

    /* Switch to receive mode */
    USBPD_Rx_Mode( );
    if( pd_tx_trycnt )
    {
        /* Send successful */
        return( DEF_PD_TX_OK );
    }
    else
    {
        /* Send failed */
        return( DEF_PD_TX_FAIL );
    }
}

/*********************************************************************
 * @fn      PDO_Request
 *
 * @brief   This function uses to Send the specified PDO.
 *
 * @return  none
 */
void USBPDO_Request( uint8_t pdo_index )
{
    uint16_t Current,Voltage;
    uint8_t  status;
    if ((pdo_index > PDO_Len) || (pdo_index == 0))
    {
        while(1)
        {
            #if USBPD_DEBUG_ON
            dbg_printf("pdo_index error!\r\n");
            #endif
            #if USE_FreeRTOS
            vTaskDelay(pdMS_TO_TICKS(500));
            #else
            soft_delay_ms( 500 );
            #endif
        }
    }
    else
    {
        memcpy( &USBPD_Rx_Buf[ 2 ], &Adapter_SrcCap[ 4*(pdo_index-1) + 1 ], 4 );
        USBPD_PDO_Analyse( 1, &USBPD_Rx_Buf[ 2 ], &Current, &Voltage );
#if USBPD_DEBUG_ON
        dbg_printf("Request: Current:%d mA , Voltage:%d mV\r\n",Current,Voltage);
#endif

        USBPD_Load_Header( 0x00, DEF_TYPE_REQUEST );
        USBPD_Rx_Buf[ 5 ] = 0x03;
        USBPD_Rx_Buf[ 5 ] |= pdo_index<<4;
        USBPD_Rx_Buf[ 3 ] = USBPD_Rx_Buf[ 3 ] & 0x03;
        USBPD_Rx_Buf[ 3 ] |= ( USBPD_Rx_Buf[ 2 ] << 2 );
        USBPD_Rx_Buf[ 4 ] = USBPD_Rx_Buf[ 3 ];
        USBPD_Rx_Buf[ 4 ] <<= 2;
        USBPD_Rx_Buf[ 4 ] = USBPD_Rx_Buf[ 4 ] & 0x0C;
        USBPD_Rx_Buf[ 4 ] |= ( USBPD_Rx_Buf[ 2 ] >> 6 );
    }
    status = USBPD_Send_Handle( &USBPD_Rx_Buf[ 2 ], 4 );

    if( status == DEF_PD_TX_OK )
    {
        PD_Ctl.PD_State = STA_RX_ACCEPT_WAIT;
    }
    else
    {
        PD_Ctl.PD_State = STA_TX_SOFTRST;
#if USBPD_DEBUG_ON
        dbg_printf("PD TX ERROR.\r\n");
#endif
    }
    PD_Ctl.PD_Comm_Timer = 0;
    PD_Ctl.Flag.Bit.PD_Comm_Succ = 1;
}

/*********************************************************************
 * @fn      PD_Save_Adapter_SrcCap
 *
 * @brief   This function uses to save the adapter SrcCap information.
 *
 * @return  none
 */
void USBPD_Save_Adapter_SrcCap(void)
{
    uint8_t  i, len;

    /* Calculate the number of NDO's (Number of Data Objects) in the Message Header */
    len = ( ( USBPD_Rx_Buf[ 1 ] >> 4 ) & 0x07 );

    /* Remove the PPS section */
    for( i = 0; i < len; i++ )
    {
        if( ( USBPD_Rx_Buf[ 2 + ( i << 2 ) + 3 ] & 0xC0 ) == 0xC0 )
        {
            break;
        }
    }

    PDO_Len = i;

    /* Modify SrcCap information */
    /* BIT31-30: Fixed Supply */
    /* BIT29:    Dual-Role Power */
    /* BIT28:    USB Suspend Power */
    /* BIT27:    Unconstrained Power */
    /* BIT26:    USB Communications */
    /* BIT25:    Dual-Role Data */
    /* BIT24:    Unchunked Extended Message */
    /* BIT23-20:  */
    /* BIT19-10: Voltage in 50mV units */
    /* BIT9-0:   Current in 10mA units */
    USBPD_Rx_Buf[ 5 ] = 0x3E;

    /* Save the adapter's SrcCap information */
    USBPD_Rx_Buf[ 1 ] &= 0x8F;
    USBPD_Rx_Buf[ 1 ] |= i << 4;
    Adapter_SrcCap[ 0 ] = i;
    memcpy( &Adapter_SrcCap[ 1 ], &USBPD_Rx_Buf[ 2 ], ( i << 2 ) );
}

/*********************************************************************
 * @fn      PD_PDO_Analyse
 *
 * @brief   This function uses to analyse PDO's voltage and current.
 *
 * @return  none
 */
void USBPD_PDO_Analyse( uint8_t pdo_idx, uint8_t *srccap, uint16_t *current, uint16_t *voltage )
{
    uint32_t temp32;

    temp32 = srccap[ (  ( pdo_idx - 1 ) << 2 ) + 0 ] +
                        ( (uint32_t)srccap[ ( ( pdo_idx - 1 ) << 2 ) + 1 ] << 8 ) +
                        ( (uint32_t)srccap[ ( ( pdo_idx - 1 ) << 2 ) + 2 ] << 16 );

    /* Fixed Supply Power Data Object */
    /* Calculation of current values */
    if( current != NULL )
    {
        *current = ( temp32 & 0x000003FF ) * 10;
    }

    /* Calculation of voltage values */
    if( voltage != NULL )
    {
        temp32 = temp32 >> 10;
        *voltage = ( temp32 & 0x000003FF ) * 50;
    }
}

/*********************************************************************
 * @fn      PD_Main_Proc
 *
 * @brief   This function uses to process PD status.
 *
 * @return  none
 */
void USBPD_Main_Proc( )
{
    uint8_t status;
    uint8_t pd_header;
    uint8_t var , pdo_index ;
    uint16_t Current,Voltage;
    uint32_t Power_t , MaxPower = 0  ;

    /* Receive idle timer count */
    PD_Ctl.PD_BusIdle_Timer += USBPD_Ms_Dlt;

    /* Status analysis processing */
    switch( PD_Ctl.PD_State )
    {
        case STA_DISCONNECT: //https://github.com/openwch/ch32l103.git
            /* Status: Disconnected */
#if USBPD_DEBUG_ON
            dbg_printf("Disconnect\r\n");
#endif
            USBPD_PHY_Reset( );
            break;

        case STA_SRC_CONNECT:
            /* Status: SRC access */
            /* If SRC_CAP is received within 1S, reset operation is performed */
            PD_Ctl.PD_Comm_Timer += USBPD_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 999 )
            {
                /* Retry on exception (abort after 5 attempts) */
                PD_Ctl.Err_Op_Cnt++;
                if( PD_Ctl.Err_Op_Cnt > 5 )
                {
                    PD_Ctl.Err_Op_Cnt = 0;
                    PD_Ctl.PD_State = STA_IDLE;
                }
                else
                {
                    USBPD_PHY_Reset();
                }
            }
            break;

        case STA_RX_ACCEPT_WAIT:
            /* Status: waiting to receive ACCEPT */
        case STA_RX_PS_RDY_WAIT:
#if USBPD_DEBUG_ON
            dbg_printf("Status: waiting to receive ACCEPT or RDY.\r\n");
#endif
            /* Status: waiting to receive PS_RDY */
            PD_Ctl.PD_Comm_Timer += USBPD_Ms_Dlt;
            if( PD_Ctl.PD_Comm_Timer > 499 )
            {
                PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                         /* Enable connection detection*/
                PD_Ctl.PD_State = STA_TX_SOFTRST;
                PD_Ctl.PD_Comm_Timer = 0;
            }
            break;

        case STA_RX_PS_RDY:
#if USBPD_DEBUG_ON
            dbg_printf("Status: PS_RDY received.\r\n");
#endif
            /* Status: PS_RDY received */
            PD_Ctl.PD_State = STA_IDLE;
            if( PD_Ctl.PD_State == STA_RX_APD_PS_RDY_WAIT )
            {
                PD_Ctl.PD_State = STA_RX_APD_PS_RDY;
            }
            break;

        case STA_TX_SOFTRST:
#if USBPD_DEBUG_ON
            dbg_printf("Status: send software reset.\r\n");
#endif
            /* Status: send software reset */
            /* Send soft reset, if sent successfully, mode unchanged, count +1 for retry */
            USBPD_Load_Header( 0x00, DEF_TYPE_SOFT_RESET );
            status = USBPD_Send_Handle( NULL, 0 );
            if( status == DEF_PD_TX_OK )
            {
                /* current mode unchanged, jump to initial state of current mode, mode retry count, switch mode if exceeded */
                PD_Ctl.PD_State = STA_IDLE;
            }
            else
            {
                PD_Ctl.PD_State = STA_TX_HRST;
            }
            PD_Ctl.PD_Comm_Timer = 0;
            break;

        case STA_TX_HRST:
#if USBPD_DEBUG_ON
            dbg_printf("Status: Sending a hardware reset.\r\n");
#endif
            /* Status: Sending a hardware reset */
            /* Sending a hard reset */
            PD_Ctl.Flag.Bit.Stop_Det_Chk = 1;
            USBPD_Phy_SendPack( 0x01, NULL, 0, UPD_HARD_RESET );                   /* send HRST */
            USBPD_Rx_Mode( );                                                      /* switch to rx mode */
            PD_Ctl.PD_State = STA_IDLE;
            PD_Ctl.PD_Comm_Timer = 0;
            break;

        default:
            break;
    }

    /* Receive message processing */
    if( PD_Ctl.Flag.Bit.Msg_Recvd )
    {
        /* Adapter communication idle timing */
        PD_Ctl.Adapter_Idle_Cnt = 0x00;
        pd_header = USBPD_Rx_Buf[ 0 ] & 0x1F;
        switch( pd_header )
        {
            case DEF_TYPE_SRC_CAP:
#if USBPD_DEBUG_ON
                //dbg_printf("Ticks=%d.\r\n",xTaskGetTickCount());
#endif
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(5));
                #else
                soft_delay_ms( 5 );
                #endif
                PD_Ctl.Flag.Bit.Stop_Det_Chk = 0;                         /* Enable PD disconnection detection */

                USBPD_Save_Adapter_SrcCap();

                pdo_index = PDO_INDEX_1 ;
                /* Analysis of the voltage and current of each PDO group */
                for (var = 1; var <= PDO_Len; ++var)
                {
                    USBPD_PDO_Analyse( var, &USBPD_Rx_Buf[ 2 ], &Current, &Voltage );
                    Power_t = Current * Voltage / 1000 ;
                    if( Power_t > MaxPower )
                    {
                        pdo_index = var ;
                        MaxPower = Power_t ;
                    }
#if USBPD_DEBUG_ON
                    dbg_printf("PDO %d: Current:%d mA , Voltage:%d mV\r\n",var,Current,Voltage);
#endif
                }
#if USBPD_DEBUG_ON
                //dbg_printf("Ticks=%d.\r\n",xTaskGetTickCount());
#endif
                /* Different PDO's for different voltages and currents */
                /* Default application for the first group of PDO, 5V */
                //USBPDO_Request( PDO_INDEX_1 );  //5V,3A
                USBPDO_Request( pdo_index );      //Request Max Power
                break;

            case DEF_TYPE_ACCEPT:
                /* ACCEPT received */
                PD_Ctl.PD_State = STA_RX_PS_RDY_WAIT;
                PD_Ctl.PD_Comm_Timer = 0;
                break;

            case DEF_TYPE_PS_RDY:
                /* PS_RDY is received */
#if USBPD_DEBUG_ON
                dbg_printf("Success\r\n");
#endif
                PD_Ctl.PD_State = STA_RX_PS_RDY;
                break;

            case DEF_TYPE_WAIT:
                /* WAIT received, many requests may receive WAIT, need specific analysis */
                break;

            case DEF_TYPE_GET_SNK_CAP:
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(1));
                #else
                soft_delay_ms( 1 );
                #endif
                USBPD_Load_Header( 0x00, DEF_TYPE_SNK_CAP );
                USBPD_Send_Handle( SinkCap_5V1A_Tab, sizeof( SinkCap_5V1A_Tab ) );
                break;

            case DEF_TYPE_SOFT_RESET:
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(1));
                #else
                soft_delay_ms( 1 );
                #endif
                USBPD_Load_Header( 0x00, DEF_TYPE_ACCEPT );
                USBPD_Send_Handle( NULL, 0 );
                break;

            case DEF_TYPE_GET_SRC_CAP_EX:
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(1));
                #else
                soft_delay_ms( 1 );
                #endif
                USBPD_Load_Header( 0x01, DEF_TYPE_SRC_CAP );
                USBPD_Send_Handle( SrcCap_Ext_Tab, sizeof( SrcCap_Ext_Tab ) );
                break;

            case DEF_TYPE_GET_STATUS:
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(1));
                #else
                soft_delay_ms( 1 );
                #endif
                USBPD_Load_Header( 0x01, DEF_TYPE_GET_STATUS_R );
                USBPD_Send_Handle( Status_Ext_Tab, sizeof( Status_Ext_Tab ) );
                break;

            case DEF_TYPE_VCONN_SWAP:
                #if USE_FreeRTOS
                vTaskDelay(pdMS_TO_TICKS(1));
                #else
                soft_delay_ms( 1 );
                #endif
                USBPD_Load_Header( 0x00, DEF_TYPE_REJECT );
                USBPD_Send_Handle( NULL, 0 );
                break;

            case DEF_TYPE_VENDOR_DEFINED:
                /* VDM message handling */
                if( ( USBPD_Rx_Buf[ 2 ] & 0xC0 ) == 0 )
                {
                    /* REQ */
                    #if USE_FreeRTOS
                    vTaskDelay(pdMS_TO_TICKS(1));
                    #else
                    soft_delay_ms( 1 );
                    #endif

                    /* Data to be sent is cached to USBPD_Tx_Buf */
                    USBPD_Load_Header( 0x00, DEF_TYPE_VENDOR_DEFINED );

                    /* Return to NAK */
                    if( ( USBPD_Rx_Buf[ 3 ] & 0x60 ) == 0 )
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 0;
                    }
                    else
                    {
                        PD_Ctl.Flag.Bit.VDM_Version = 1;
                    }
                    USBPD_Rx_Buf[ 2 ] |= 0x80;
                    USBPD_Send_Handle( &USBPD_Rx_Buf[ 2 ], 4 );
                }
                break;

            default:
#if USBPD_DEBUG_ON
                dbg_printf("Unsupported Command\r\n");
#endif
                break;
        }

        /* Message has been processed, interrupt reception is turned on again */
        USBPD_Rx_Mode( );
        PD_Ctl.Flag.Bit.Msg_Recvd = 0;                                    /* Clear the received flag */
        PD_Ctl.PD_BusIdle_Timer = 0;                                      /* Idle time cleared */
    }
}


#if USE_FreeRTOS
void USBPD_Task(void *pvParameters)
{
    TickType_t xTickCount = 0, xTickCount_Last = 0 ;

    /* Initialize PD registers and states. */
    usbpd_init();

    while( 1 )
    {
        xTickCount = xTaskGetTickCount();
        USBPD_Ms_Dlt = ( ( uint8_t )( xTickCount - xTickCount_Last ) ) * portTICK_RATE_MS ;
        xTickCount_Last = xTickCount ;

        PD_Ctl.Det_Timer += USBPD_Ms_Dlt;
        if( PD_Ctl.Det_Timer > 4 )
        {
            PD_Ctl.Det_Timer = 0 ;
            USBPD_Det_Proc();
        }
        USBPD_Main_Proc();
    }
}
#else
void usbpd_loop( void )
{
    static uint8_t USBPD_Ms_Cnt_Last;                                                         /* System timer millisecond timing final value */
    TIM1->DMAINTENR &= ~TIM_UIF ;
    /* Get the calculated timing interval value */
    USBPD_Ms_Dlt = USBPD_Ms_Cnt - USBPD_Ms_Cnt_Last ;
    USBPD_Ms_Cnt_Last = USBPD_Ms_Cnt ;
    TIM1->DMAINTENR |= TIM_UIF ;

    PD_Ctl.Det_Timer += USBPD_Ms_Dlt;
    if( PD_Ctl.Det_Timer > 4 )
    {
        PD_Ctl.Det_Timer = 0 ;
        USBPD_Det_Proc();
    }
    USBPD_Main_Proc();
}
#endif