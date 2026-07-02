
//ESP32-IDF include files
#include <driver/gpio.h>
#include <driver/uart.h>
#include "esp_log.h"
//comm files
#include "comm_uart.h"
#include "comm_powerctrl.h"
#include "comm_frames.h"
//FreeRTOS include files
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#define COMM_UART_RXD_PIN   GPIO_NUM_9
#define COMM_UART_TXD_PIN   GPIO_NUM_10

// Setup UART buffered IO with event queue
const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;


uint8_t comm_rx_buf[COMM_FRAMES_BUF_LEN];


void uart_comm_init(void)
{
    uart_config_t uart_config = 
    {
        .baud_rate = 115200 ,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122 
    };
    // Configure UART parameters
    ESP_ERROR_CHECK( uart_param_config(UART_NUM_1, &uart_config) );
    // Set UART pins(TX , RX , RTS, CTS)
    ESP_ERROR_CHECK( uart_set_pin( UART_NUM_1 , COMM_UART_TXD_PIN , COMM_UART_RXD_PIN , UART_PIN_NO_CHANGE , UART_PIN_NO_CHANGE ) );

    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install( UART_NUM_1 , uart_buffer_size , uart_buffer_size , 10 , &uart_queue , 0 ));

}

void uart_comm_send_packet( uint8_t *u_packet , uint8_t length )
{
    uart_write_bytes( UART_NUM_1 , (const char*)u_packet, length );
    //ESP_ERROR_CHECK( uart_wait_tx_done( UART_NUM_1 , 100 ) ); // wait timeout is 100 RTOS ticks (TickType_t)
}

void comm_uart_recv_Task(void *pvParameters)
{  
    uart_event_t revc_event ;
    uint16_t len = 0 , rev_num = 0 ;
    uint16_t comm_rx_buf_read_ptr = 0 ;
    while( 1 )
    {
        if( xQueueReceive( uart_queue , (void *)&revc_event , portMAX_DELAY ) == pdTRUE )
        {
            switch (revc_event.type)
            {
                case UART_DATA :
                    //ESP_LOGI("UART","Received data.");
                    len = ( revc_event.size > COMM_FRAMES_BUF_LEN ) ? COMM_FRAMES_BUF_LEN : revc_event.size ; 
                    comm_rx_buf_read_ptr = 0 ;
                    uart_read_bytes( UART_NUM_1 , comm_rx_buf , len , portMAX_DELAY );
                    break;
                case UART_FIFO_OVF :
                    //ESP_LOGI("UART","FIFO overflow.");
                    uart_flush_input( UART_NUM_1 );
                    xQueueReset( uart_queue );
                case UART_BUFFER_FULL :
                    //ESP_LOGI("UART","Buffer full.");
                    uart_flush_input( UART_NUM_1 );
                    xQueueReset( uart_queue );
                default:
                    break;
            }

            while( len )
            {
                //dbg_printf("Recv_Task:%d.\r\n",len);
                rev_num = comm_frames_recv_cb( &power_frames , comm_rx_buf + comm_rx_buf_read_ptr , len );
                comm_rx_buf_read_ptr += rev_num ;
                comm_rx_buf_read_ptr %= COMM_FRAMES_BUF_LEN;
                len = len - rev_num ;
            }
        }
    }
}
