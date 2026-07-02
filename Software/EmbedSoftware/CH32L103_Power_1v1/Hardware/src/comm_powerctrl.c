/* 与电源底板进行通讯
高位在前  低位在后   例如：0x1234  低位0x34 高位0x12

1.模式设置  指令：0x01
|指令 |模式||电压|电流|
|0x01 |1字节|2字节|2字节|

模式: 0:恒压 1:恒流
电压:2字节 mv
电流:2字节 ma

2.保护设置  指令：0x02
|指令 |电压|电流|
|0x02 |2字节|2字节|
电压:2字节 mv ,输出最大电压值
电流:2字节 ma ,输出最大电流值

3.输出控制  指令：0x03
|指令 |控制|
|0x03 |1字节|
控制: 0:关闭 1:打开

5.设置匀速上报指令 指令:0x04
|指令 |上报间隔|
|0x03 |1字节|
上报间隔:ms 10-255

4.输出数据查询 指令：0x05
|指令 |
|0x05 |

返回数据:0x05
|指令|输出状态|模式|告警|电压|电流|输入电压|
|0x05|1字节  |1字节|1字节|2字节|2字节|

告警：0 无告警   1  过压  2 过流  3 其他故障
*/

/* includes -------------------------------------------------------------------*/
#include "ch32l103x_bsp.h"
//sc8815
#include "sc8815_user.h"
//ina226
#include "ina226_user.h"
//ch32 adc
#include "adc_conv.h"
//comm uart
#include "comm_uart.h"
#include "comm_powerctrl.h"
#include "power_data.h"
/* macro ----------------------------------------------------------------------*/
//FAN
#define PWRCTRL_FAN             GPIOB
#define PWRCTRL_FAN_RCC_EN      RCC_IOPBEN
#define PWRCTRL_FAN_NUM         8
#define PWRCTRL_FAN_PIN         (((uint16_t)0x0001) << PWRCTRL_FAN_NUM )

#if USE_FreeRTOS

#define USE_COMM_POWERDATA_TASK_STACK       0

/* FreeRTOS Stack Size , Unit Word */
#define COMM_UART_RX_TASK_STACK_SIZE        (512/4)   /* 512Byte */
#define COMM_POWERDATA_TASK_STACK_SIZE      (512/4)   /* 512Byte */
/* FreeRTOS Task Priorities */
#define COMM_UART_RX_TASK_PRIORITIES        (configMAX_PRIORITIES-2)
#define COMM_POWERDATA_TASK_PRIORITIES      (configMAX_PRIORITIES-3)
#endif

/* exported variables ---------------------------------------------------------*/
comm_frames_handle_t power_frames ;

static void comm_powerctrl_05cmd_ack( void )
{
    /*
    | Cmd_ID | Power Status | CV Mode |  Warning | Voltage | Current | Input Volagte| VBUS Voltage | VBUS Current |SC8815 interupt | NTC temp |
    |  0x05  |    1 Byte    | 1 Byte  |  1 Byte  | 2 Bytes | 2 Bytes |    2 Bytes   |    2 Bytes   |    2 Bytes   |    1 Byte      |  2 Byte  |
    */
    uint8_t data[17] = {0};
    data[0] = 0x05;
    data[1] = power_data.set_switch_status ;
    data[2] = power_data.set_cc_cv;
    data[3] = power_data.alarm ;

    //INA226 Part
    //vbus voltage , unit mV
    data[4] = (power_data.ina_read_voltage >> 8) & 0xff ;
    data[5] = power_data.ina_read_voltage & 0xff ;
    //vbus current , unit mA
    data[6] = (power_data.ina_read_current >> 8) & 0xff ;
    data[7] = power_data.ina_read_current & 0xff ;

    //CH32 Adc read input voltage , unit mV
    data[8] = ( power_data.input_voltage >> 8 ) & 0xff ;
    data[9] = power_data.input_voltage & 0xff ;

    //SC8815 Part
    //vbus voltage , unit mV
    data[10] = (power_data.sc8815_vbus_voltage >> 8) & 0xff;
    data[11] = power_data.sc8815_vbus_voltage & 0xff;
    //vbus current , unit mA
    data[12] = (power_data.sc8815_vbus_current >> 8) & 0xff;
    data[13] = power_data.sc8815_vbus_current & 0xff;
    //SC8815 interrupt
    data[14] = power_data.sc8815_interrupt;

    //NTC temperature
    data[15] = (power_data.ntc_temp >> 8) & 0xff ;
    data[16] = power_data.ntc_temp & 0xFF ;

    comm_frames_send_pack( &power_frames , data , 17 );
}

static void comm_powerctrl_unpack_callback(uint8_t *DataLoad , uint8_t length )
{
    uint8_t cmd = DataLoad[0];
    switch(cmd)
    {
        case POWERCTRL_VC_SET :
            power_data.set_cc_cv = DataLoad[1];
            power_data.set_voltage = ( DataLoad[2] << 8 ) | DataLoad[3] ;
            power_data.set_current = ( DataLoad[4] << 8 ) | DataLoad[5] ;
            sc8815_user_set(power_data.set_cc_cv, power_data.set_voltage, power_data.set_current);
            break;
        case POWERCTRL_PORTECT :
            power_data.set_protect_voltage = ( DataLoad[1] << 8 ) | DataLoad[2];
            power_data.set_protect_current = ( DataLoad[3] << 8 ) | DataLoad[4];
            sc8815_protect_set(power_data.set_protect_voltage, power_data.set_protect_current);
            break;
        case POWERCTRL_ON_OFF :
            power_data.set_switch_status = DataLoad[1];
            sc8815_user_power_on_off(power_data.set_switch_status);
            break;
        case POWERCTRL_DATA_RATE :
            power_data.report_interval = DataLoad[1];
            break;
        case POWERCTRL_READ_DATA :
            comm_powerctrl_05cmd_ack();
            break;

        default:
            break;
    }
};

static uint8_t comm_power_initialized = 0 ;

void comm_powerctrl_init(void)
{
    power_frames.rx_length = 0 ;
    power_frames.frames_status = COMM_FRAMES_STATUS_HEAD ;
    power_frames.comm_frames_upack_cb = comm_powerctrl_unpack_callback ;
    power_frames.comm_frames_send_cb = uart_comm_send_packet ;

    //20ms
    power_data.report_interval = 20 ;
    //18000mV
    power_data.set_protect_voltage = 18000 ;
    //11000mA
    power_data.set_protect_current = 11000 ;

    //FAN LOW , fan off
    PWRCTRL_FAN->BCR = PWRCTRL_FAN_PIN ;
    //SC8815 FAN : push pull output
#if PWRCTRL_FAN_NUM < 8 
    PWRCTRL_FAN->CFGLR = ( PWRCTRL_FAN->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( PWRCTRL_FAN_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( PWRCTRL_FAN_NUM << 2 ) ) ;
#else
    PWRCTRL_FAN->CFGHR = ( PWRCTRL_FAN->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( PWRCTRL_FAN_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( PWRCTRL_FAN_NUM -8 ) << 2 ) ) ;
#endif

    //ch32 adc init
    adc_conv_init();

    //ina226 init
    ina226_user_int();

    //sc8815 init
    sc8815_user_init();

    //uart init
    uart_comm_init();

    comm_power_initialized = 1 ;
};

void comm_powerctrl_fan(uint8_t Fan_Newstate )
{
    if( Fan_Newstate )
        PWRCTRL_FAN->BSHR = PWRCTRL_FAN_PIN ;   //FAN High , Fan on
    else
        PWRCTRL_FAN->BCR = PWRCTRL_FAN_PIN ;    //FAN Low , Fan off
}

void comm_powerctrl_Task(void *pvParameters)
{
    uint8_t fan_status = 0 ;
    uint32_t power_tick = 0 ;
    TickType_t xLastWakeTime ;

    //comm power control init
    comm_powerctrl_init();
    
    /* Power Control task */
    xTaskCreate((TaskFunction_t )comm_uart_recv_Task ,
                        (const char*    )"Rev_Task",
                        (uint16_t       )COMM_UART_RX_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )COMM_UART_RX_TASK_PRIORITIES,  //task pro level
                        (TaskHandle_t*  )NULL);
    
#if USE_COMM_POWERDATA_TASK_STACK
    /* Power Data Upadte Task */
    xTaskCreate((TaskFunction_t )Power_Data_Task ,
                        (const char*    )"Upadte_Task",
                        (uint16_t       )COMM_POWERDATA_TASK_STACK_SIZE,  //stack size
                        (void*          )NULL ,
                        (UBaseType_t    )COMM_POWERDATA_TASK_PRIORITIES,     //task pro level
                        (TaskHandle_t*  )NULL);
#endif
    xLastWakeTime = xTaskGetTickCount() ;
    while(1)
    {        
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20) );
        //comm_frames_unpack( &power_frames );
 #if ( USE_COMM_POWERDATA_TASK_STACK == 0 )
        ina226_data_update();
        
        sc8815_data_upadte();

        ch32_adc_upadte();

        if( power_tick % 50 == 0 )
        {
            dbg_printf("[output]: voltage=%d mV, current=%d mA, power=%d W.\r\n", power_data.ina_read_voltage, power_data.ina_read_current, power_data.ina_read_power);
            dbg_printf("[temperature]: %d.%d, adc_vin=%d mV.\r\n",power_data.ntc_temp/10, power_data.ntc_temp%10, power_data.input_voltage);
        }

        if( power_data.ntc_temp > 650 && fan_status == 0 )
        {
            comm_powerctrl_fan( 1 );
            fan_status = 1 ;
        }
        else if( power_data.ntc_temp < 550 && fan_status == 1 )
        {
            comm_powerctrl_fan( 0 );
            fan_status = 0 ;
        }
#endif

        if( power_data.report_interval != 0 )
        {
            if( power_tick % power_data.report_interval == 0 )
            {
                comm_powerctrl_05cmd_ack();
            }
        }
        power_tick ++ ;
    }
}

void Power_Data_Task(void *pvParameters)
{
    uint8_t fan_status = 0 ;
    TickType_t xLastWakeTime ;

    while( comm_power_initialized == 0 );

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {        
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20) );
        
        ina226_data_update();
        
        sc8815_data_upadte();

        ch32_adc_upadte();

        if( power_data.ntc_temp > 650 && fan_status == 0 )
        {
            comm_powerctrl_fan( 1 );
            fan_status = 1 ;
        }
        else if( power_data.ntc_temp < 550 && fan_status == 1 )
        {
            comm_powerctrl_fan( 0 );
            fan_status = 0 ;
        }
    }
}

void Power_Protect_Task(void *pvParameters)
{
    TickType_t xLastWakeTime ;
    while( comm_power_initialized == 0 );
    xLastWakeTime = xTaskGetTickCount() ;
    while(1)
    {        
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20) );
    }
}