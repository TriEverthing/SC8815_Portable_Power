/* includes -------------------------------------------------------------------*/
#include <driver/gpio.h>
#include <driver/uart.h>
#include "esp_log.h"
//comm uart
#include "comm_uart.h"
#include "comm_powerctrl.h"
#include "power_data.h"
//FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
//lvgl
#include "lvgl.h"
//nxp guider
#include <gui_guider.h>
#include <events_init.h>

extern lv_ui guider_ui ;
extern SemaphoreHandle_t xGuiSemaphore ;
/* exported variables ---------------------------------------------------------*/
comm_frames_handle_t power_frames ;
SemaphoreHandle_t PowerDataSemaphore ;

static void comm_powerctrl_unpack_callback(uint8_t *DataLoad , uint8_t length )
{
/*
| Cmd_ID | Power Status | CV Mode |  Warning | Voltage | Current | Input Volagte| VBUS Voltage | VBUS Current |SC8815 interupt | NTC temp |
|  0x05  |    1 Byte    | 1 Byte  |  1 Byte  | 2 Bytes | 2 Bytes |    2 Bytes   |    2 Bytes   |    2 Bytes   |    1 Byte      |  1 Byte  |
*/
    if( DataLoad[0] == POWERCTRL_READ_DATA )
    {

        power_data.set_switch_status = DataLoad[1];
        power_data.set_cc_cv = DataLoad[2];
        power_data.alarm = DataLoad[3];

        //INA226 Part
        //vbus voltage , unit mV
        power_data.ina_read_voltage = ( DataLoad[4] << 8 ) | DataLoad[5] ;
        //vbus current , unit mA
        power_data.ina_read_current = ( DataLoad[6] << 8 ) | DataLoad[7] ;
        power_data.ina_read_power = ( uint16_t )( power_data.ina_read_current / 10000.0 * power_data.ina_read_voltage ) ;
        //CH32 Adc read input voltage , unit mV
        power_data.input_voltage = ( DataLoad[8] << 8 )  | DataLoad[9] ;

        //SC8815 Part
        //vbus voltage , unit mV
        power_data.sc8815_vbus_voltage = ( DataLoad[10] << 8 ) | DataLoad[11] ;
        //vbus current , unit mA
        power_data.sc8815_vbus_current = ( DataLoad[12] << 8 ) | DataLoad[13] ;
        //SC8815 interrupt
        power_data.sc8815_interrupt = DataLoad[14];

        //NTC temperature
        power_data.ntc_temp =  ( DataLoad[15] << 8 ) | DataLoad[16]  ;

        xSemaphoreGive( PowerDataSemaphore );
    }
};

void comm_powerctrl_init(void)
{
    PowerDataSemaphore = xSemaphoreCreateCounting( 10 , 0 );

    power_frames.rx_length = 0 ;
    power_frames.frames_status = COMM_FRAMES_STATUS_HEAD ;
    power_frames.comm_frames_upack_cb = comm_powerctrl_unpack_callback ;
    power_frames.comm_frames_send_cb = uart_comm_send_packet ;

    //20ms
    power_data.report_interval = 20 ;
    //32000mV
    power_data.set_protect_voltage = 32000 ;
    //12000mA
    power_data.set_protect_current = 12000 ;

    //3300mV
    power_data.set_voltage = 3300 ;
    //10000mA
    power_data.set_current = 10000 ;

    //uart init
    uart_comm_init();
};


void comm_powerctrl_Task(void *pvParameters)
{
    char number_str[16];
    while(1)
    {        
        if( xSemaphoreTake( PowerDataSemaphore , portMAX_DELAY ) == pdTRUE )
        {
            if( xGuiSemaphore != NULL )
            {
                if ( xSemaphoreTake(xGuiSemaphore, portMAX_DELAY) == pdTRUE ) 
                {
                    snprintf( number_str , 7 , "%2.4f" , power_data.ina_read_voltage / 1000.0 );
                    lv_label_set_text( guider_ui.ST7789V3_OutputVlotage_Lable , number_str );

                    snprintf( number_str , 7 , "%2.4f" , power_data.ina_read_current / 1000.0 );
                    lv_label_set_text( guider_ui.ST7789V3_OutputCurrent_Lable , number_str );

                    snprintf( number_str , 7 , "%2.4f" , power_data.ina_read_power / 100.0 );
                    lv_label_set_text( guider_ui.ST7789V3_OutputPower_Lable , number_str );

                    snprintf( number_str , 16 , "VIN:%2.2fV" , power_data.input_voltage / 1000.0 );
                    lv_label_set_text( guider_ui.ST7789V3_InputVoltage_Lable , number_str );

                    snprintf( number_str , 16 , "NTC:%3.1fC" , power_data.ntc_temp / 10.0 );
                    lv_label_set_text( guider_ui.ST7789V3_Temp_Lable , number_str );

                    xSemaphoreGive(xGuiSemaphore);
                }
            }
        }
    }
}


void comm_powerctrl_set_mode(uint8_t mode, uint16_t voltage, uint16_t current)
{
    uint8_t data[6] = {0};
    data[0] = 0x01;
    data[1] = mode;
    data[2] = (voltage >> 8) & 0xff;
    data[3] = voltage & 0xff;
    data[4] = (current >> 8) & 0xff;
    data[5] = current & 0xff;

    comm_frames_send_pack(&power_frames, data, 6);
};

void comm_powerctrl_set_protect(uint16_t voltage, uint16_t current)
{
    uint8_t data[5] = {0};
    data[0] = 0x02;
    data[1] = (voltage >> 8) & 0xff;
    data[2] = voltage & 0xff;
    data[3] = (current >> 8) & 0xff;
    data[4] = current & 0xff;

    comm_frames_send_pack(&power_frames, data, 5);
};

// bit0 PSTOP 引脚, 1:打开 0:关闭
void comm_powerctrl_set_output(uint8_t output)
{
    uint8_t send_val = 0 ;
    if( output == 1 )
    {
        send_val = 0x03;  //
        /* code */
    }
    
    uint8_t data[2] = {0};
    data[0] = 0x03;
    data[1] = send_val;

    comm_frames_send_pack(&power_frames, data, 2);
};


void comm_powerctrl_set_interval(uint8_t interval)
{
    uint8_t data[2] = {0} ;
    data[0] = 0x04 ;
    data[1] = interval ;
    comm_frames_send_pack(&power_frames, data, 2);
};


void comm_powerctrl_get_data( void )
{
    uint8_t data[1] = {0};
    data[0] = 0x05;
    comm_frames_send_pack( &power_frames , data , 1 );
};


