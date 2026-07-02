#ifndef __COMM_UART_H
#define __COMM_UART_H

/* includes -------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
/* private includes -----------------------------------------------------------*/

/* exported types -------------------------------------------------------------*/

/* exported constants ---------------------------------------------------------*/

/* exported macro -------------------------------------------------------------*/
#define UART_COMM_DMA_TX        1
#define UART_COMM_DMA_RX        1
/* exported functions ---------------------------------------------------------*/
void uart_comm_init(void);
void uart_comm_send_packet( uint8_t *u_packet , uint8_t length );
void comm_uart_recv_Task(void *pvParameters);

#endif