#include "comm_frames.h"
/* Communication frame structure
| frames | data length |        Data Load      | CheckSum |
| 1 Byte |    1 Byte   | 0-FRAME_MAX_LEN Bytes |  1 Byte  |
*/

const uint8_t crc8_maxim_lookuptable[256] = 
{
   /* 0     1     2     3     4     5     6     7     8     9     A    B      C     D    E      F */
    0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83, 0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41, /* 0 */
    0x9D, 0xC3, 0x21, 0x7F, 0xFC, 0xA2, 0x40, 0x1E, 0x5F, 0x01, 0xE3, 0xBD, 0x3E, 0x60, 0x82, 0xDC, /* 1 */
    0x23, 0x7D, 0x9F, 0xC1, 0x42, 0x1C, 0xFE, 0xA0, 0xE1, 0xBF, 0x5D, 0x03, 0x80, 0xDE, 0x3C, 0x62, /* 2 */
    0xBE, 0xE0, 0x02, 0x5C, 0xDF, 0x81, 0x63, 0x3D, 0x7C, 0x22, 0xC0, 0x9E, 0x1D, 0x43, 0xA1, 0xFF, /* 3 */
    0x46, 0x18, 0xFA, 0xA4, 0x27, 0x79, 0x9B, 0xC5, 0x84, 0xDA, 0x38, 0x66, 0xE5, 0xBB, 0x59, 0x07, /* 4 */
    0xDB, 0x85, 0x67, 0x39, 0xBA, 0xE4, 0x06, 0x58, 0x19, 0x47, 0xA5, 0xFB, 0x78, 0x26, 0xC4, 0x9A, /* 5 */
    0x65, 0x3B, 0xD9, 0x87, 0x04, 0x5A, 0xB8, 0xE6, 0xA7, 0xF9, 0x1B, 0x45, 0xC6, 0x98, 0x7A, 0x24, /* 6 */
    0xF8, 0xA6, 0x44, 0x1A, 0x99, 0xC7, 0x25, 0x7B, 0x3A, 0x64, 0x86, 0xD8, 0x5B, 0x05, 0xE7, 0xB9, /* 7 */
    0x8C, 0xD2, 0x30, 0x6E, 0xED, 0xB3, 0x51, 0x0F, 0x4E, 0x10, 0xF2, 0xAC, 0x2F, 0x71, 0x93, 0xCD, /* 8 */
    0x11, 0x4F, 0xAD, 0xF3, 0x70, 0x2E, 0xCC, 0x92, 0xD3, 0x8D, 0x6F, 0x31, 0xB2, 0xEC, 0x0E, 0x50, /* 9 */
    0xAF, 0xF1, 0x13, 0x4D, 0xCE, 0x90, 0x72, 0x2C, 0x6D, 0x33, 0xD1, 0x8F, 0x0C, 0x52, 0xB0, 0xEE, /* A */
    0x32, 0x6C, 0x8E, 0xD0, 0x53, 0x0D, 0xEF, 0xB1, 0xF0, 0xAE, 0x4C, 0x12, 0x91, 0xCF, 0x2D, 0x73, /* B */
    0xCA, 0x94, 0x76, 0x28, 0xAB, 0xF5, 0x17, 0x49, 0x08, 0x56, 0xB4, 0xEA, 0x69, 0x37, 0xD5, 0x8B, /* C */
    0x57, 0x09, 0xEB, 0xB5, 0x36, 0x68, 0x8A, 0xD4, 0x95, 0xCB, 0x29, 0x77, 0xF4, 0xAA, 0x48, 0x16, /* D */
    0xE9, 0xB7, 0x55, 0x0B, 0x88, 0xD6, 0x34, 0x6A, 0x2B, 0x75, 0x97, 0xC9, 0x4A, 0x14, 0xF6, 0xA8, /* E */
    0x74, 0x2A, 0xC8, 0x96, 0x15, 0x4B, 0xA9, 0xF7, 0xB6, 0xE8, 0x0A, 0x54, 0xD7, 0x89, 0x6B, 0x35  /* F */ 
   /* 0     1     2     3     4     5     6     7     8     9     A    B      C     D    E      F */     
};

static uint8_t comm_frames_crc8_maxim(uint8_t *data, uint8_t crc_in , uint8_t len)
{
    uint8_t crc = crc_in ;
    uint8_t i = 0 ;
    for ( i = 0 ; i < len ; i ++ )
    {
        crc = crc8_maxim_lookuptable[crc ^ data[i] ] ;
    }
    return crc ;
}

uint8_t comm_frames_recv_cb(comm_frames_handle_t *frames_handle , uint8_t *DataLoad , uint8_t length )
{
    if( frames_handle->rx_length ==  COMM_FRAMES_MAX_LEN )
    {
        return 0 ;
    }

    if ( frames_handle->rx_length + length > COMM_FRAMES_MAX_LEN )
    {
        length = COMM_FRAMES_MAX_LEN - frames_handle->rx_length ;
    }

    memcpy( &frames_handle->rx_buf[ frames_handle->rx_length ] , DataLoad , length );
    frames_handle->rx_length += length ;

    comm_frames_unpack( frames_handle );

    return length ;
}


void comm_frames_send_pack(comm_frames_handle_t *frames_handle , uint8_t *DataLoad , uint8_t length )
{
    uint8_t buffer[3] = {0};
    
    buffer[0] = COMM_FRAMES_HEAD ;
    buffer[1] = length ;
    buffer[2] = comm_frames_crc8_maxim( buffer , 0 , 2 );
    buffer[2] = comm_frames_crc8_maxim( DataLoad , buffer[2] , length );
    //send data & length
    frames_handle->comm_frames_send_cb( buffer , 2 );
    //send load
    frames_handle->comm_frames_send_cb( DataLoad , length );
    //send crc bits
    frames_handle->comm_frames_send_cb( buffer + 2 , 1 );
};


void comm_frames_unpack(comm_frames_handle_t *frames_handle)
{
    uint8_t crc = 0 ;
    uint8_t len = 0 ;
    uint8_t i = 0 ;

    do
    {
        switch( frames_handle->frames_status )
        {
            case COMM_FRAMES_STATUS_HEAD :
                for( i = 0 ; i < frames_handle->rx_length ; i ++ ) //Search data header
                {
                    if( frames_handle->rx_buf[i] == COMM_FRAMES_HEAD ) //Detected data header
                    {
                        frames_handle->frames_status = COMM_FRAMES_STATUS_LEN ;
                        if( i > 0 )
                        {
                            frames_handle->rx_length = frames_handle->rx_length - i;
                            memcpy( &frames_handle->rx_buf[0] , &frames_handle->rx_buf[i] , frames_handle->rx_length );
                            
                        }
                        //dbg_printf("Detected Header.\r\n");
                        break ;
                    }
                }

                if( frames_handle->frames_status == COMM_FRAMES_STATUS_HEAD  ) //No Data or No header , clear rx_length
                {
                    frames_handle->rx_length = 0 ;
                    break ;
                }
                
            case COMM_FRAMES_STATUS_LEN :

                if( frames_handle->rx_length >= 3 )
                {
                    if( frames_handle->rx_buf[1] > COMM_FRAMES_MAX_LEN )
                    {
                        //DataLength Error
                        frames_handle->rx_length -= 1 ;
                        memcpy( &frames_handle->rx_buf[0], &frames_handle->rx_buf[1], frames_handle->rx_length);
                        frames_handle->frames_status = COMM_FRAMES_STATUS_HEAD ;
                        break ;
                    }  

                    if( frames_handle->rx_length >= ( frames_handle->rx_buf[1] + 3 ) ) 
                    {
                        //dbg_printf("Detected DataLoad.\r\n");
                        frames_handle->frames_status = COMM_FRAMES_STATUS_CRC ;
                    }
                    else
                    {
                        //Need New Data
                        break ;
                    }          
                }
                else 
                {
                    //Need New Data
                    break ;
                }
            case COMM_FRAMES_STATUS_CRC :
                crc = comm_frames_crc8_maxim( &frames_handle->rx_buf[0], 0 , frames_handle->rx_buf[1] + 2 );
                if (crc != ( frames_handle->rx_buf[ frames_handle->rx_buf[1] + 2 ] ) )
                {
                    //dbg_printf("CRC Error:0x%02X.\r\n",crc);
                    //Data CRC Error
                    frames_handle->rx_length -= 1 ;
                    memcpy( &frames_handle->rx_buf[0], &frames_handle->rx_buf[1], frames_handle->rx_length);
                }
                else
                {
                    //Successfully received the data
                    len = frames_handle->rx_buf[1];
                    frames_handle->comm_frames_upack_cb(&frames_handle->rx_buf[2] , len );
                    frames_handle->rx_length -= (len + 3);
                    memcpy(&frames_handle->rx_buf[0], &frames_handle->rx_buf[len + 3], frames_handle->rx_length);
                }
                frames_handle->frames_status = COMM_FRAMES_STATUS_HEAD ;
                break;
            default:
                break;
        }
    }
    while( ( frames_handle->rx_length ) > 0  && ( frames_handle->frames_status == COMM_FRAMES_STATUS_HEAD ) ) ;

};

