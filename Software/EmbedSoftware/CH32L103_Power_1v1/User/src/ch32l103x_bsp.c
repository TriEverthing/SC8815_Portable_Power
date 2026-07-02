/*
 * stm32h7xx_bsp.c
 *
 *  Created on: May 19, 2025
 *      Author: 30408
 */
#include "ch32l103x_bsp.h"
#include "usbd_cdc_data.h"

#if USE_FreeRTOS
//SemaphoreHandle_t UartTxMutex ;
#endif

/* private includes ----------------------------------------------------------*/

/* private typedef -----------------------------------------------------------*/

/* private define ------------------------------------------------------------*/

/* private macro -------------------------------------------------------------*/

/* private variables ---------------------------------------------------------*/
static __I uint8_t CH32_PBHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};
static __I uint8_t CH32_ADCPrescTable[4] = {2, 4, 6, 8};
/* at-start led resouce array */
GPIO_TypeDef *led_gpio_port[LED_NUM]        = { LED0_GPIO  };
const uint16_t led_gpio_pin[LED_NUM]        = { LED0_NUMS  };
const uint32_t led_gpio_crm_clk[LED_NUM]    = { LED0_GPIO_RCC_EN };
/* exported variables --------------------------------------------------------*/
/* delay variable */
__IO uint8_t  UsNumber = 0 ;
static uint16_t MsNumber = 0 ;
/* tick */
__IO uint32_t overTick = 0 ;
__IO uint32_t uwTick = 0 ;
uint16_t uwTickFreq = 1 ;  /* 1KHz */
/* initialzation variables */

/* private function prototypes --------------------------------------------*/

/* private user code ------------------------------------------------------*/

extern uint32_t SystemCoreClock ;  

#if UART_DMA_TX
#define UART_TXBUF_SIZE 128
static uint8_t uart_printf_buf1[UART_TXBUF_SIZE];
static uint8_t uart_printf_buf2[UART_TXBUF_SIZE];
static uint8_t * uart_printf_buf = uart_printf_buf1 ;
static uint16_t uart_tx_buf_ptr = 0 ;
//static uint8_t dma_tx_flag = 0 ;
#endif

/**
  * @brief  this function configures the source of the time base.
  * @param  none
  * @retval none
  */
void ch32_timebase_init(void)
{
    UsNumber = SystemCoreClock / 8000000 ;
    MsNumber = (uint16_t)UsNumber * 1000 ;
    /* system tick config */
    TICK_COUNT_VALUE = 0UL ;
    SysTick->CMP = 0x00 ;
    SysTick->CTLR &= ~ ( SysTick_CTRL_MODE_Msk | SysTick_CTRL_STCLK_Msk );
    SysTick->CTLR |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_STRE_Msk ;
}


/**
  * @brief  delay ms function
  * @param  none
  * @retval none
  */
void soft_delay_ms( uint32_t dms )
{
// #if USE_FreeRTOS
//   vTaskDelay( pdMS_TO_TICKS(dms) );
// #else
  volatile uint32_t mt1 , mt2 ;
  for( mt1 = 0 ; mt1 < dms ; mt1 ++ )
  {
    //96MHZ-4020
    for( mt2 = 0 ; mt2 < 4020 ; mt2 ++ ) //1ms
    {
      __NOP();
      __NOP();
    }
  }
// #endif
}

/**
  * @brief  delay us function
  * @param  none
  * @retval none
  */
void soft_delay_us( uint32_t dus )
{
  volatile uint32_t mt1 ;
  volatile uint32_t mt2 ;
  for( mt1 = 0 ; mt1 < dus ; mt1 ++ )
  {
    //480MHZ-15
    for( mt2 = 0 ; mt2 < 4 ; mt2 ++ ) //1us
    {
      __NOP();
    }
  }
}

/**
 * @brief  None.
 * @param  None.
 * @return None.
 */
void DebugInit( void )
{
#if DEBUG_ON_OFF
#if DEBUG_TO_UART
  UartDebugInit( 115200 );
#else
  UsbDebugInit();
#endif


#if USE_EasyLogger
  //easylogger init
  elog_init();
  //Assert
  elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
  //Error
  elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
  //Waring
  elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
  //Information
  elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
  //Debug
  elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LINE | ELOG_FMT_FUNC | ELOG_FMT_TIME );
  //Verbose
  elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);
  //Start elog
  elog_start();
#endif
#endif
}


/*******************************************************************************
* Function Name  : uart_init
* Description    : Initialize the I/O serial port 1
* Input          : baud rates
* Return         : none
*******************************************************************************/
void UartDebugInit(uint32_t baudrate)
{
    uint32_t  integerdivider = 0x00 , Fclocks ;
    CH32_ClocksTypeDef Clocks ;

    uart_tx_buf_ptr = 0 ;

#if USE_FreeRTOS
    //UartTxMutex = xSemaphoreCreateMutex();
#endif

#if(DEBUG == DEBUG_UART1)
    //PA4 USART1_TX
    //USART1 Clock Enable
    RCC->PB2PCENR |= RCC_USART1EN ;
    //GPIOA Clock Enable
    RCC->PB2PCENR |= RCC_IOPAEN ;
    //AFIO Clock Enable
    RCC->PB2PCENR |= RCC_AFIOEN ;
    //USART1_RM PA4 , 010
    AFIO->PCFR1 &= ~AFIO_PCFR1_USART1_RM ;
    AFIO->PCFR2 = ( AFIO->PCFR2 & ( ~ AFIO_PCFR2_USART1_RM_H ) ) | AFIO_PCFR2_USART1_RM_H_0 ;
    //PA4 AF_PP OUT , 50MHz
    GPIOA->CFGLR = ( GPIOA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( 4 << 2 ) ) ) )  | ( (uint32_t )(0x0000000B) << ( 4 << 2 ) ) ;
#elif(DEBUG == DEBUG_UART2)
    //USART2 Clock Enable
    RCC->PB1PCENR |= RCC_USART2EN ;
    //GPIOA Clock Enable
    RCC->PB2PCENR |= RCC_IOPAEN ;
    //PA2 AF_PP OUT , 50MHz
    GPIOA->CFGLR = ( GPIOA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( 2 << 2 ) ) ) )  | ( (uint32_t )(0x0000000B) << ( 2 << 2 ) ) ;
#elif(DEBUG == DEBUG_UART3)
    //USART3 Clock Enable
    RCC->PB1PCENR |= RCC_USART3EN ;
    //GPIOB Clock Enable
    RCC->PB2PCENR |= RCC_IOPBEN ;
    //PB10 AF_PP OUT , 50MHz
    GPIOB->CFGHR = ( GPIOB->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( 10 - 8 ) << 2 ) ) ) )  | ( (uint32_t )(0x0000000B) << ( ( 10 - 8 ) << 2 ) ) ;
#endif

    CH32_GetClocksFreq( &Clocks );
#if(DEBUG == DEBUG_UART1)
    Fclocks = Clocks.PCLK2_Frequency ;
    if( USART1->CTLR1 & 0x8000 )
    {
        Fclocks = Fclocks << 1 ;
    }
#elif(DEBUG == DEBUG_UART2)
    Fclocks = Clocks.PCLK1_Frequency ;
    if( USART2->CTLR1 & 0x8000 )
    {
        Fclocks = Fclocks << 1 ;
    }
#elif(DEBUG == DEBUG_UART3)
    Fclocks = Clocks.PCLK1_Frequency ;
    if( USART3->CTLR1 & 0x8000 )
    {
        Fclocks = Fclocks << 1 ;
    }
#endif

#if(DEBUG == DEBUG_UART1)
    //UART1 Init
    //8bits , no Parity , OnlyTX enable
    USART1->CTLR1 &= ~( USART_CTLR1_M | USART_CTLR1_PCE | USART_CTLR1_PS | USART_CTLR1_RE | USART_CTLR1_TE );
    USART1->CTLR1 |= USART_CTLR1_TE ;
    //1 StopBits 
    USART1->CTLR2 &= ~USART_CTLR2_STOP ;
    //No HardFlow
    USART1->CTLR3 &= ~( USART_CTLR3_RTSE | USART_CTLR3_CTSE ) ;

    //Baudrate Setting
    integerdivider = Fclocks / baudrate ;
    USART1->BRR = (uint16_t)integerdivider ;
    //USART Enable
    USART1->CTLR1 |= USART_CTLR1_UE ;
#elif(DEBUG == DEBUG_UART2)
    //8bits , no Parity , OnlyTX enable
    USART2->CTLR1 &= ~( USART_CTLR1_M | USART_CTLR1_PCE | USART_CTLR1_PS | USART_CTLR1_RE | USART_CTLR1_TE );
    USART2->CTLR1 |= USART_CTLR1_TE ;
    //1 StopBits 
    USART2->CTLR2 &= ~USART_CTLR2_STOP ;
    //No HardFlow
    USART2->CTLR3 &= ~( USART_CTLR3_RTSE | USART_CTLR3_CTSE ) ;

    //Baudrate Setting
    integerdivider = Fclocks / baudrate ;
    USART2->BRR = (uint16_t)integerdivider ;
    USART2->CTLR1 |= USART_CTLR1_UE ;
#elif(DEBUG == DEBUG_UART3)
    //8bits , no Parity , OnlyTX enable
    USART3->CTLR1 &= ~( USART_CTLR1_M | USART_CTLR1_PCE | USART_CTLR1_PS | USART_CTLR1_RE | USART_CTLR1_TE );
    USART3->CTLR1 |= USART_CTLR1_TE ;
    //1 StopBits 
    USART3->CTLR2 &= ~USART_CTLR2_STOP ;
    //No HardFlow
    USART3->CTLR3 &= ~( USART_CTLR3_RTSE | USART_CTLR3_CTSE ) ;

    //Baudrate Setting
    integerdivider = Fclocks / baudrate ;
    USART3->BRR = (uint16_t)integerdivider ;
    USART3->CTLR1 |= USART_CTLR1_UE ;
#endif

#if UART_DMA_TX
    /* enable DMA clock */
    RCC->HBPCENR |= RCC_DMAEN ;
    /* TX DMA Enable */
	  USART1->CTLR3 |= USART_CTLR3_DMAT ;
#if(DEBUG == DEBUG_UART1)
    //DMA Channel4 USART1_TX
    //from Memory to Peripheral , Signal mode , Peripheral inc , Peripheral Size = 8bit , Memeory Size = 8bit
    DMA1_Channel4->CFGR = DMA_CFGR1_DIR | DMA_CFGR1_MINC | DMA_CFGR1_PL_1 ;
    //DMA numbers
    DMA1_Channel4->CNTR = 0U ;
    //DMA peripheral address
    DMA1_Channel4->PADDR = ( uint32_t )( &( USART1->DATAR )) ;
    //DMA memory address
    DMA1_Channel4->MADDR = (uint32_t)(uart_printf_buf) ;
    /* enable DMA channel */
    //DMA1_Channel4->CFGR |= DMA_CFGR1_EN ;
#elif(DEBUG == DEBUG_UART2)
    //DMA Channel4 USART2_TX
    //from Memory to Peripheral , Signal mode , Peripheral inc , Peripheral Size = 8bit , Memeory Size = 8bit
    DMA1_Channel7->CFGR = DMA_CFGR1_DIR | DMA_CFGR1_MINC | DMA_CFGR1_PL_1 ;
    //DMA numbers
    DMA1_Channel7->CNTR = 0U ;
    //DMA peripheral address
    DMA1_Channel7->PADDR = ( uint32_t )( &( USART2->DATAR )) ;
    //DMA memory address
    DMA1_Channel7->MADDR = (uint32_t)(uart_printf_buf);
    /* enable DMA channel */
    //DMA1_Channel7->CFGR |= DMA_CFGR1_EN ;
#elif(DEBUG == DEBUG_UART3)
    //DMA Channel2 USART3_TX
    //from Memory to Peripheral , Signal mode , Peripheral inc , Peripheral Size = 8bit , Memeory Size = 8bit
    DMA1_Channel2->CFGR = DMA_CFGR1_DIR | DMA_CFGR1_MINC | DMA_CFGR1_PL_1 ;
    //DMA numbers
    DMA1_Channel2->CNTR = 0U ;
    //DMA peripheral address
    DMA1_Channel2->PADDR = ( uint32_t )( &( USART3->DATAR )) ;
    //DMA memory address
    DMA1_Channel2->MADDR = (uint32_t)(uart_printf_buf);
    /* enable DMA channel */
    //DMA1_Channel2->CFGR |= DMA_CFGR1_EN ;
#endif
#endif
}

/*******************************************************************************
* Function Name  : usart_printf
* Description    : Handle the Read operation from the microSD card.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void uart_printf(const char* fmt,...)
{
	uint16_t len_t ;
#if UART_DMA_TX == 0
  uint8_t uart_printf_buf[256];
#endif
	va_list ap;

#if USE_FreeRTOS
  taskENTER_CRITICAL();
  //xSemaphoreTake(UartTxMutex, portMAX_DELAY);
#endif

	va_start(ap,fmt);
  len_t = vsnprintf((char*)uart_printf_buf+uart_tx_buf_ptr, UART_TXBUF_SIZE - uart_tx_buf_ptr, fmt,ap);
	va_end(ap);

  if( len_t > ( UART_TXBUF_SIZE - uart_tx_buf_ptr ) )
  {
    len_t = UART_TXBUF_SIZE - uart_tx_buf_ptr;
  }

#if UART_DMA_TX
#if(DEBUG == DEBUG_UART1)
  uart_tx_buf_ptr = uart_tx_buf_ptr + len_t;
  if( DMA1_Channel4->CNTR == 0 )
  {
    DMA1_Channel4->CFGR &= ~DMA_CFGR1_EN ;
    DMA1_Channel4->CNTR = uart_tx_buf_ptr ;
    DMA1_Channel4->MADDR = ( uint32_t ) uart_printf_buf ;
    /* enable DMA channel */
    DMA1_Channel4->CFGR |= DMA_CFGR1_EN ;

    if( uart_printf_buf == uart_printf_buf1 )
      uart_printf_buf = uart_printf_buf2 ;
    else
      uart_printf_buf = uart_printf_buf1 ;

    uart_tx_buf_ptr = 0;

    /* Wait DMA Finished */
    // while( ( DMA1->INTFR & DMA_TCIF4 ) == 0 );
    // DMA1->INTFCR |= DMA_CTCIF4 ;
  }
#elif(DEBUG == DEBUG_UART2)
  DMA1_Channel7->CFGR &= ~DMA_CFGR1_EN ;
  DMA1_Channel7->CNTR = len_t ;
  //DMA1_Channel7->MADDR = ( uint32_t ) uart_printf_buf ;
  /* enable DMA channel */
  DMA1_Channel7->CFGR |= DMA_CFGR1_EN ;
  while( ( DMA1->INTFR & DMA_TCIF7 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF7 ;
#elif(DEBUG == DEBUG_UART3)
  DMA1_Channel2->CFGR &= ~DMA_CFGR1_EN ;
  DMA1_Channel2->CNTR = len_t ;
  //DMA1_Channel2->MADDR = ( uint32_t ) uart_printf_buf ;
  /* enable DMA channel */
  DMA1_Channel2->CFGR |= DMA_CFGR1_EN ;
  while( ( DMA1->INTFR & DMA_TCIF2 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF2 ;
#endif

#else
  for( uint16_t i = 0 ; i < len_t ; i++ )
	{
#if(DEBUG == DEBUG_UART1)
		while( ( USART1->STATR & USART_STATR_TC ) == 0 );
		USART1->DATAR = (uint8_t) uart_printf_buf[i];
#elif(DEBUG == DEBUG_UART2)
    while(( USART2->STATR & USART_STATR_TC ) == 0 );
    USART2->DATAR = ( uint8_t ) uart_printf_buf[i] ;
#elif(DEBUG == DEBUG_UART3)
    while(( USART3->STATR & USART_STATR_TC ) == 0 );
    USART3->DATAR = ( uint8_t ) uart_printf_buf[i] ;
#endif
  }
#endif

#if USE_FreeRTOS
  taskEXIT_CRITICAL();
  //xSemaphoreGive(UartTxMutex);
#endif
}

#if 0
/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length
 *
 * @return  size: Data length
 */
__attribute__((used))
int _write(int fd, char *buf, int size)
{

#if USE_FreeRTOS
  taskENTER_CRITICAL();
  //xSemaphoreTake( UartTxMutex , portMAX_DELAY );
#endif

#if UART_DMA_TX
#if(DEBUG == DEBUG_UART1)
  DMA1_Channel4->CFGR &= ~DMA_CFGR1_EN ;
  DMA1_Channel4->CNTR = size ;
  DMA1_Channel4->MADDR = ( uint32_t ) buf ;
  /* enable DMA channel */
  DMA1_Channel4->CFGR |= DMA_CFGR1_EN ;
  /* Wait DMA Finished */
  while( ( DMA1->INTFR & DMA_TCIF4 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF4 ;
#elif(DEBUG == DEBUG_UART2)
  DMA1_Channel7->CFGR &= ~DMA_CFGR1_EN ;
  DMA1_Channel7->CNTR = size ;
  DMA1_Channel7->MADDR = ( uint32_t ) buf ;
  /* enable DMA channel */
  DMA1_Channel7->CFGR |= DMA_CFGR1_EN ;
  while( ( DMA1->INTFR & DMA_TCIF7 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF7 ;
#elif(DEBUG == DEBUG_UART3)
  DMA1_Channel2->CFGR &= ~DMA_CFGR1_EN ;
  DMA1_Channel2->CNTR = size ;
  DMA1_Channel2->MADDR = ( uint32_t ) buf ;
  /* enable DMA channel */
  DMA1_Channel2->CFGR |= DMA_CFGR1_EN ;
  while( ( DMA1->INTFR & DMA_TCIF2 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF2 ;
#endif
#else
  for(uint16_t i = 0; i < size; i++)
  {
#if(DEBUG == DEBUG_UART1)
    while(( USART1->STATR & USART_STATR_TC ) == 0 );
    USART1->DATAR = ( uint8_t ) *buf++ ;
#elif(DEBUG == DEBUG_UART2)
    while(( USART2->STATR & USART_STATR_TC ) == 0 );
    USART2->DATAR = ( uint8_t ) *buf++ ;
#elif(DEBUG == DEBUG_UART3)
    while(( USART3->STATR & USART_STATR_TC ) == 0 );
    USART3->DATAR = ( uint8_t ) *buf++ ;
#endif
  }
#endif
#if USE_FreeRTOS
  taskEXIT_CRITICAL();
  //xSemaphoreGive(UartTxMutex);
#endif
  return size;
}
#endif


#if EN_USART1_RX   //

u8 USART_RX_BUF[USART_REC_LEN];     //

u16 USART_RX_STA=0;       //
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : UART1 interrupts the service function
* Input          : none
* Return         : none
*******************************************************************************/
void USART1_IRQHandler(void)
{
  u8 res;
  if(USART1->SR&(1<<5))//
  {
    res=USART1->DR;
    if((USART_RX_STA&0x8000)==0)//
    {
      if(USART_RX_STA&0x4000)//
      {
        if(res!=0x0a)USART_RX_STA=0;//
        else USART_RX_STA|=0x8000;  //
      }else //
      {
        if(res==0x0d)USART_RX_STA|=0x4000;
        else
        {
          USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
          USART_RX_STA++;
          if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//
        }
      }
    }
  }

}
#endif

/*********************************************************************
 * @fn      RCC_GetClocksFreq
 *
 * @brief   The result of this function could be not correct when using
 *        fractional value for HSE crystal.
 *
 * @param   RCC_Clocks - pointer to a RCC_ClocksTypeDef structure which will hold
 *        the clocks frequencies.
 *
 * @return  none
 */
void CH32_GetClocksFreq(CH32_ClocksTypeDef* RCC_Clocks)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0, presc = 0;

    tmp = RCC->CFGR0 & RCC_SWS ;

    switch (tmp)
    {
      case 0x00: //HSI
        if(RCC->CTLR & RCC_HSILP )
        {
          RCC_Clocks->SYSCLK_Frequency = HSI_LP_VALUE;
        }
        else
        {
          RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
        }
        break;

      case 0x04: //HSE
        RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
        break;

      case 0x08: //PLL
        pllmull = RCC->CFGR0 & RCC_PLLMULL ;
        pllsource = RCC->CFGR0 & RCC_PLLSRC ;

        pllmull = ( pllmull >> 18) + 2;

        if(pllmull == 17) pllmull = 18;


        if (pllsource == 0x00)
        {
          //HSI
          if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE)
          {
            RCC_Clocks->SYSCLK_Frequency = (HSI_VALUE) * pllmull;
          }
          else
          {
            RCC_Clocks->SYSCLK_Frequency = (HSI_VALUE >>1) * pllmull;
          }
        }
        else
        {
          //HSE
          if ((RCC->CFGR0 & RCC_PLLXTPRE ) != 0 )
          {
            RCC_Clocks->SYSCLK_Frequency = (HSE_VALUE >> 1) * pllmull;
          }
          else
          {
            RCC_Clocks->SYSCLK_Frequency = HSE_VALUE * pllmull;

          }
        }
        break;
      default:
        RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
        break;
    }
    /* AHB clock prescaler */
    tmp = RCC->CFGR0 & RCC_HPRE ;
    tmp = tmp >> 4;
    presc = CH32_PBHBPrescTable[tmp];
    RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;
    /* APB1 clock prescaler */
    tmp = RCC->CFGR0 & RCC_PPRE1 ;
    tmp = tmp >> 8;
    presc = CH32_PBHBPrescTable[tmp];
    RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
    /* APB2 clock prescaler */
    tmp = RCC->CFGR0 & RCC_PPRE2 ;
    tmp = tmp >> 11;
    presc = CH32_PBHBPrescTable[tmp];
    RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

    if(RCC->CFGR0 & RCC_ADC_PRE_ADJ )
    {
      RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->HCLK_Frequency;
    }
    else
    {
      tmp = RCC->CFGR0 & RCC_ADCPRE_DIV8 ;
      tmp = tmp >> 14;
      presc = CH32_ADCPrescTable[tmp];
      RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->PCLK2_Frequency / presc;
    }
}

/**
  * @brief  configure led gpio
  * @param  led: specifies the led to be configured.
  * @retval none
  */
void led_init(led_type led)
{
    uint32_t Shift_Mask  = 0x00 ;
	/* enable the led clock */
    RCC->PB2PCENR |= led_gpio_crm_clk[led] ;

    if( led_gpio_pin[led] < 8 )
    {
        Shift_Mask = led_gpio_pin[led] << 2 ;
	    /* configure the led gpio */
        led_gpio_port[led]->CFGLR &= ~( ((uint32_t)0x0000000F) << Shift_Mask ) ;
        led_gpio_port[led]->CFGLR |=  ( ((uint32_t)0x00000003) << Shift_Mask ) ;
    }
    else 
    {
        Shift_Mask = ( led_gpio_pin[led] - 8 ) << 2 ;
    	/* configure the led gpio */
        led_gpio_port[led]->CFGHR &= ~( ((uint32_t)0x0000000F) << Shift_Mask ) ;
        led_gpio_port[led]->CFGHR |=  ( ((uint32_t)0x00000003) << Shift_Mask ) ;
    } 
    led_on( led );
}

/**
  * @brief  turns selected led on.
  * @param  led: specifies the led to be set on.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void led_on(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
    led_gpio_port[led]->BSHR = (( uint32_t)0x00000001) << led_gpio_pin[led] ;
}

/**
  * @brief  turns selected led off.
  * @param  led: specifies the led to be set off.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void led_off(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
    led_gpio_port[led]->BCR = (( uint32_t)0x00000001) << led_gpio_pin[led] ;
}

/**
  * @brief  turns selected led toggle.
  * @param  led: specifies the led to be set off.
  *   this parameter can be one of following parameters:
  *     @arg LED2
  *     @arg LED3
  *     @arg LED4
  * @retval none
  */
void led_toggle(led_type led)
{
  if(led > (LED_NUM - 1))
    return;
  if(led_gpio_pin[led])
	led_gpio_port[led]->OUTDR ^= (( uint32_t)0x00000001) << led_gpio_pin[led]  ;
}

/**
  * @brief  initialize delay function
  * @param  none
  * @retval none
  */
void delay_init()
{
  UsNumber = SystemCoreClock / 8000000 ;
  MsNumber = (uint16_t)UsNumber * 1000 ;
  SysTick->CTLR &= ~ ( SysTick_CTRL_MODE_Msk | SysTick_CTRL_STCLK_Msk );
  SysTick->CTLR |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_STRE_Msk ;
}


/**
  * @brief  inserts a delay time.
  * @param  nus: specifies the delay time length, in microsecond.
  * @retval none
  */
void delay_us(uint32_t nus)
{
  uint32_t i;

  SysTick->SR &= ~(1 << 0) ;
  i = (uint32_t)nus * UsNumber ;

  SysTick->CMP = i;
  SysTick->CTLR |= (1 << 4);
  SysTick->CTLR |= (1 << 5) | (1 << 0);

  while((SysTick->SR & (1 << 0)) != (1 << 0));
  SysTick->CTLR &= ~(1 << 0) ;
}


/**
  * @brief  inserts a delay time.
  * @param  nms: specifies the delay time length, in milliseconds.
  * @retval none
  */
void delay_ms(uint32_t nms)
{
  uint32_t i;

  SysTick->SR &= ~(1 << 0);
  i = (uint32_t)nms * MsNumber ;

  SysTick->CMP = i;
  SysTick->CTLR |= (1 << 4);
  SysTick->CTLR |= (1 << 5) | (1 << 0);

  while((SysTick->SR & (1 << 0)) != (1 << 0));
  SysTick->CTLR &= ~(1 << 0);
}

/**
  * @brief  inserts a delay time.
  * @param  sec: specifies the delay time, in seconds.
  * @retval none
  */
void delay_xms(uint32_t nms)
{
	uint32_t i;
	for( i = 0 ; i < nms ; i ++ )
	{
		delay_us( 1000 ) ;
	}
}

#if 0
/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
__attribute__((used))
void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}
#endif
/**
  * @brief  usb board init.
  * @param  none
  * @retval none
  */
void usbd_low_level_init(void)
{
    //Config USB Clocks for 96MHz , RCC_USBCLKSource_PLLCLK_Div2:96MHz/2=48MHz
    RCC->CFGR0 &= ~( RCC_USBPRE_0 | RCC_USBPRE_1 );
    RCC->CFGR0 |= RCC_USBPRE_0;
    //Enbale USB Clock
	  RCC->HBPCENR |= RCC_USBFSEN;
    //Enable interrupt
    //USBFS_IRQn PreemptionPriority = 0 , USBFS_IRQn SubPriority = 0
    NVIC->IPRIOR[(uint32_t)(USBFS_IRQn)] = ( 0 << 7 ) | ( 0 << 5 );
    NVIC->IENR[((uint32_t)(USBFS_IRQn) >> 5)] = (1 << ((uint32_t)(USBFS_IRQn) & 0x1F));
}
