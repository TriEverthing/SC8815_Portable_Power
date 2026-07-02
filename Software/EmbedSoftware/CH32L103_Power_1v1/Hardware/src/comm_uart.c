
#include "ch32l103x_bsp.h"
#include "comm_uart.h"
#include "comm_powerctrl.h"
#include "comm_frames.h"

#if USE_FreeRTOS
SemaphoreHandle_t Uart_Comm_TxMutex ;
SemaphoreHandle_t Uart_Comm_RxSemCount ;
#endif


uint8_t comm_rx_buf[COMM_FRAMES_BUF_LEN];

static uint8_t uart_comm_initialized = 0 ;

void uart_comm_init(void)
{
    uint32_t  integerdivider = 0x00 , Fclocks ;
    CH32_ClocksTypeDef Clocks;

#if USE_FreeRTOS
    Uart_Comm_TxMutex = xSemaphoreCreateMutex();
    Uart_Comm_RxSemCount = xSemaphoreCreateCounting( 16 , 0 );
#endif

    //USART2 Clock Enable
    RCC->PB1PCENR |= RCC_USART2EN ;
    //GPIOA Clock Enable
    RCC->PB2PCENR |= RCC_IOPAEN ;
    //PA2 TX AF_PP OUT , 50MHz
    GPIOA->CFGLR = ( GPIOA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( 2 << 2 ) ) ) ) | ( (uint32_t )(0x0000000B) << ( 2 << 2 ) ) ;
    //PA3 RX AF_PP OUT , 50MHz
    GPIOA->CFGLR = ( GPIOA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( 3 << 2 ) ) ) ) | ( (uint32_t )(0x0000000B) << ( 3 << 2 ) ) ;

    CH32_GetClocksFreq( &Clocks );
    Fclocks = Clocks.PCLK1_Frequency ;
    if( USART2->CTLR1 & 0x8000 )
    {
        Fclocks = Fclocks << 1 ;
    }
    //8bits , no Parity , TX/RX enable
    USART2->CTLR1 &= ~( USART_CTLR1_M | USART_CTLR1_PCE | USART_CTLR1_PS | USART_CTLR1_RE | USART_CTLR1_TE );
    USART2->CTLR1 |= USART_CTLR1_TE | USART_CTLR1_RE ;
    //1 StopBits 
    USART2->CTLR2 &= ~USART_CTLR2_STOP ;
    //No HardFlow
    USART2->CTLR3 &= ~( USART_CTLR3_RTSE | USART_CTLR3_CTSE ) ;

    //Baudrate Setting
    integerdivider = Fclocks / 115200 ;
    USART2->BRR = (uint16_t)integerdivider ;
    USART2->CTLR1 |= USART_CTLR1_UE ;

#if UART_COMM_DMA_RX
    /* enable DMA clock */
    RCC->HBPCENR |= RCC_DMAEN ;
    /* RX DMA Enable */
	USART2->CTLR3 |= USART_CTLR3_DMAR ;
    //from Peripheral to Memory , Circle mode , Peripheral inc , Peripheral Size = 8bit , Memeory Size = 8bit
    DMA1_Channel6->CFGR = DMA_CFG6_MINC | DMA_CFG6_PL_1 | DMA_CFG6_CIRC ;
    //DMA memory address
    DMA1_Channel6->MADDR = (uint32_t)(comm_rx_buf) ;
    //DMA numbers
    DMA1_Channel6->CNTR = COMM_FRAMES_BUF_LEN ;
    //DMA peripheral address
    DMA1_Channel6->PADDR = ( uint32_t )( &( USART2->DATAR )) ;
    /* enable DMA channel */
    DMA1_Channel6->CFGR |= DMA_CFG6_EN ;
#endif

#if UART_COMM_DMA_TX
    /* enable DMA clock */
    RCC->HBPCENR |= RCC_DMAEN ;
    /* TX DMA Enable */
	USART2->CTLR3 |= USART_CTLR3_DMAT ;
    //from Memory to Peripheral , Signal mode , Peripheral inc , Peripheral Size = 8bit , Memeory Size = 8bit
    DMA1_Channel7->CFGR = DMA_CFG7_DIR | DMA_CFG7_MINC | DMA_CFG7_PL_1 ;
    //DMA numbers
    DMA1_Channel7->CNTR = 0U ;
    //DMA peripheral address
    DMA1_Channel7->PADDR = ( uint32_t )( &( USART2->DATAR )) ;
    //DMA memory address
    //DMA1_Channel7->MADDR = (uint32_t)(uart_printf_buf);
    /* enable DMA channel */
    //DMA1_Channel7->CFGR |= DMA_CFGR1_EN ;
#endif

    //Enable interrupt
    //UART2_IRQn PreemptionPriority = 1 , UART2_IRQn SubPriority = 0
    NVIC->IPRIOR[(uint32_t)(USART2_IRQn)] = ( 1 << 7 ) | ( 0 << 5 );
    NVIC->IENR[((uint32_t)(USART2_IRQn) >> 5)] = (1 << ((uint32_t)(USART2_IRQn) & 0x1F));
    //DMA1_Channel6_IRQn PreemptionPriority = 1 , UART2_IRQn SubPriority = 1
    NVIC->IPRIOR[(uint32_t)(DMA1_Channel6_IRQn)] = ( 1 << 7 ) | ( 1 << 5 );
    NVIC->IENR[((uint32_t)(DMA1_Channel6_IRQn) >> 5)] = (1 << ((uint32_t)(DMA1_Channel6_IRQn) & 0x1F));

    //Enable DMA1 Channel6 for UART2_RX , full or half interrupt
    DMA1_Channel6->CFGR |= DMA_CFG6_HTIE | DMA_CFG6_TCIE ;
    //Enable UART2 bus IDLE interrupt
    USART2->CTLR1 &= ~( USART_CTLR1_PEIE | USART_CTLR1_TXEIE | USART_CTLR1_TCIE | USART_CTLR1_RXNEIE ) ;
    USART2->CTLR2 &= ~( USART_CTLR2_LBDIE ) ;
    USART2->CTLR3 &= ~( USART_CTLR3_EIE | USART_CTLR3_CTSIE) ;
    USART2->CTLR1 |= USART_CTLR1_IDLEIE ;

    uart_comm_initialized = 1 ;
}

void uart_comm_send_packet( uint8_t *u_packet , uint8_t length )
{
  if( uart_comm_initialized == 0 )
    return ;

#if USE_FreeRTOS
  xSemaphoreTake( Uart_Comm_TxMutex, portMAX_DELAY );
#endif

#if UART_COMM_DMA_TX
  DMA1_Channel7->CFGR &= ~DMA_CFG7_EN ;
  DMA1_Channel7->CNTR = length ;
  DMA1_Channel7->MADDR = ( uint32_t ) u_packet ;
  /* enable DMA channel */
  DMA1_Channel7->CFGR |= DMA_CFG7_EN ;
  while( ( DMA1->INTFR & DMA_TCIF7 ) == 0 ) ;
  DMA1->INTFCR |= DMA_CTCIF7 ;
#else
  for( uint16_t i = 0 ; i < len_t ; i++ )
  {
    while(( USART2->STATR & USART_STATR_TC ) == 0 );
    USART2->DATAR = ( uint8_t ) uart_printf_buf[i] ;
  }
#endif

#if USE_FreeRTOS
  xSemaphoreGive(Uart_Comm_TxMutex);
#endif    
}

void comm_uart_recv_Task(void *pvParameters)
{  
    uint16_t len = 0 , rev_num = 0 ;
    uint16_t comm_rx_dma_ptr = 0 , comm_rx_dma_last_ptr = 0 ;
    while( ( uart_comm_initialized == 0 ) || ( Uart_Comm_RxSemCount == NULL ) );
    while( 1 )
    {
        xSemaphoreTake( Uart_Comm_RxSemCount , portMAX_DELAY ); 
        // 1. [   R.....W   1             ] 
		// 2. [             1   R......W  ]
        // 3. [...W         1         R...]
		comm_rx_dma_ptr = COMM_FRAMES_BUF_LEN - DMA1_Channel6->CNTR ;
        len = ( comm_rx_dma_ptr + COMM_FRAMES_BUF_LEN - comm_rx_dma_last_ptr)% COMM_FRAMES_BUF_LEN;

        while( len )
        {
            //dbg_printf("Recv_Task:%d.\r\n",len);
            if( comm_rx_dma_ptr >= comm_rx_dma_last_ptr )
            {
                rev_num = len;
            }
            else
            {
                rev_num = COMM_FRAMES_BUF_LEN - comm_rx_dma_last_ptr;                
            }
            rev_num = comm_frames_recv_cb( &power_frames , comm_rx_buf + comm_rx_dma_last_ptr , rev_num );
            comm_rx_dma_last_ptr = ( comm_rx_dma_last_ptr + rev_num ) % COMM_FRAMES_BUF_LEN;
            len = len - rev_num ;
        }
    }
}

#if USE_FreeRTOS
void USART2_IRQHandler(void) __attribute__((interrupt()));
void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt()));
#else
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel6_IRQHandler(void) __attribute__((interrupt()));
#endif

void USART2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE ;
    
    if( ( USART2->STATR & USART_STATR_IDLE ) != 0 )
    {
        (void) USART2->DATAR;
        xSemaphoreGiveFromISR( Uart_Comm_RxSemCount , &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken == pdTRUE )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }    
    }
}

void DMA1_Channel6_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE ;
    if( ( DMA1->INTFR & DMA_HTIF6 ) || ( DMA1->INTFR & DMA_TCIF6 ))
	{
        DMA1->INTFCR |= DMA_CGIF6 | DMA_CTCIF6 | DMA_CHTIF6 | DMA_CTEIF6 ;
        xSemaphoreGiveFromISR( Uart_Comm_RxSemCount , &xHigherPriorityTaskWoken );
        if( xHigherPriorityTaskWoken == pdTRUE )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }    
	}
}