#ifndef __COMM_FRAMES_H
#define __COMM_FRAMES_H

/* includes -------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
/* private includes -----------------------------------------------------------*/



/* exported constants ---------------------------------------------------------*/

/* exported macro -------------------------------------------------------------*/
#define COMM_FRAMES_BUF_LEN     128
#define COMM_FRAMES_MAX_LEN     32
#define COMM_FRAMES_HEAD        0xAA
/* exported types -------------------------------------------------------------*/
enum
{
    COMM_FRAMES_STATUS_HEAD = 0,
    COMM_FRAMES_STATUS_LEN ,
    COMM_FRAMES_STATUS_CRC
};

typedef struct 
{
    uint8_t rx_buf[COMM_FRAMES_BUF_LEN];
    uint8_t rx_length ;
    uint8_t frames_status ;
    /* comm_frames upack callback functions */
    void (*comm_frames_upack_cb)(uint8_t *data , uint8_t len);
    /* comm_frames transimit callback functions */
    void (*comm_frames_send_cb)(uint8_t *data , uint8_t len);
}comm_frames_handle_t;
/* exported functions ---------------------------------------------------------*/
void comm_frames_send_pack( comm_frames_handle_t *frames_handle , uint8_t *DataLoad , uint8_t length );
void comm_frames_unpack( comm_frames_handle_t *frames_handle );
uint8_t comm_frames_recv_cb(comm_frames_handle_t *frames_handle , uint8_t *DataLoad , uint8_t length );


#endif