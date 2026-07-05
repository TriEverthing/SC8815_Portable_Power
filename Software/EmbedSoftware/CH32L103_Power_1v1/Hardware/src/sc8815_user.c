#include "ch32l103x_bsp.h"
#include "SC8815.h"
#include "sc8815_user.h"
#include "sf_i2c.h"
#include "power_data.h"

#include "math.h"

/* Exported define -----------------------------------------------------------*/
//SCL
#define SC8815_SCL            GPIOD
#define SC8815_SCL_RCC_EN     RCC_IOPDEN
#define SC8815_SCL_NUM        0
#define SC8815_SCL_PIN        (((uint16_t)0x0001) << SC8815_SCL_NUM )
//SDA
#define SC8815_SDA            GPIOC
#define SC8815_SDA_RCC_EN     RCC_IOPCEN
#define SC8815_SDA_NUM        15
#define SC8815_SDA_PIN        (((uint16_t)0x0001) << SC8815_SDA_NUM )
//PSTOP
#define SC8815_PSTOP          GPIOD
#define SC8815_PSTOP_RCC_EN   RCC_IOPDEN
#define SC8815_PSTOP_NUM      1
#define SC8815_PSTOP_PIN      (((uint16_t)0x0001) << SC8815_PSTOP_NUM )
//CEN
#define SC8815_CEN            GPIOA
#define SC8815_CEN_RCC_EN     RCC_IOPAEN
#define SC8815_CEN_NUM        0
#define SC8815_CEN_PIN        (((uint16_t)0x0001) << SC8815_CEN_NUM )
//INT
#define SC8815_INT            GPIOC
#define SC8815_INT_RCC_EN     RCC_IOPCEN
#define SC8815_INT_NUM        14
#define SC8815_INT_PIN        (((uint16_t)0x0001) << SC8815_INT_NUM )
//DISC
#define SC8815_DISC           GPIOA
#define SC8815_DISC_RCC_EN    RCC_IOPAEN
#define SC8815_DISC_NUM       8
#define SC8815_DISC_PIN       (((uint16_t)0x0001) << SC8815_DISC_NUM )

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_set_scl_low( void )
{
    SC8815_SCL->BCR = SC8815_SCL_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_set_scl_high( void )
{
    SC8815_SCL->BSHR = SC8815_SCL_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_set_sda_low( void )
{
    SC8815_SDA->BCR = SC8815_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_set_sda_high( void )
{
    SC8815_SDA->BSHR = SC8815_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
uint8_t sc8815_read_sda( void )
{
   return ( ( SC8815_SDA->INDR & SC8815_SDA_PIN ) >> SC8815_SDA_NUM ) ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_sda_dir_input( void )
{
    SC8815_SDA->BSHR = SC8815_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void sc8815_sda_dir_output( void )
{
    SC8815_SDA->BSHR = SC8815_SDA_PIN ;
}


/* Exported types ------------------------------------------------------------*/
static struct sf_i2c_dev i2c0_dev = 
{
    .name               = "i2c0",
    .speed              = 4 , /*! speed:105Hz */
    .delay_us           = soft_delay_us ,
    .ops.sda_low        = sc8815_set_sda_low ,
    .ops.sda_high       = sc8815_set_sda_high ,
    .ops.scl_low        = sc8815_set_scl_low ,
    .ops.scl_high       = sc8815_set_scl_high ,
    .ops.sda_read_level = sc8815_read_sda ,
    .ops.sda_set_input  = sc8815_sda_dir_input ,
    .ops.sda_set_output = sc8815_sda_dir_output ,
};


void sc8815_i2c_init()
{
    //Enable GPIO Clocks
    RCC->PB2PCENR |= SC8815_SCL_RCC_EN | SC8815_SDA_RCC_EN | SC8815_PSTOP_RCC_EN | SC8815_CEN_RCC_EN | SC8815_INT_RCC_EN | SC8815_DISC_RCC_EN ;
    //AFIO
    RCC->PB2PCENR |= RCC_AFIOEN ;
    AFIO->PCFR1 |= AFIO_PCFR1_PD01_RM ;
    //PC14 PC15 as GPIO
    PWR->CTLR |= PWR_CTLR_DBP ;
    //LSE_OFF
    RCC->BDCTLR |= RCC_BDRST ;
   //PWR_BackupAccessCmd(ENABLE);
   //RCC_LSEConfig( RCC_LSE_OFF );
   //BKP_TamperPinCmd( DISABLE );

    //SC8815 SCL : push pull output
#if SC8815_SCL_NUM < 8 
    SC8815_SCL->CFGLR = ( SC8815_SCL->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_SCL_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( SC8815_SCL_NUM << 2 ) ) ;
#else
    SC8815_SCL->CFGHR = ( SC8815_SCL->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_SCL_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( SC8815_SCL_NUM -8 ) << 2 ) ) ;
#endif

    //SC8815 SDA : open drain output
#if SC8815_SDA_NUM < 8 
    SC8815_SDA->CFGLR = ( SC8815_SDA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_SDA_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000007) << ( SC8815_SDANUM << 2 ) ) ;
#else
    SC8815_SDA->CFGHR = ( SC8815_SDA->CFGHR & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_SDA_NUM - 8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000007) << ( ( SC8815_SDA_NUM - 8 ) << 2 ) ) ;
#endif

    //SC8815 INT : interrupt for sc8815 , pull up input
#if SC8815_INT_NUM < 8 
    SC8815_INT->CFGLR = ( SC8815_INT->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_INT_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000008) << ( SC8815_INT_NUM << 2 ) ) ;
    SC8815_INT->BSHR = SC8815_INT_PIN ;
#else
    SC8815_INT->CFGHR = ( SC8815_INT->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_INT_NUM - 8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000008) << ( ( SC8815_INT_NUM - 8 ) << 2 ) ) ;
    SC8815_INT->BSHR = SC8815_INT_PIN ;
#endif   
    PWR->CTLR &= ~PWR_CTLR_DBP ;
    //PWR_BackupAccessCmd(DISABLE);
    // GPIO_InitStructure.GPIO_Pin = SC8815_SDA_PIN ;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    // GPIO_Init(SC8815_SDA, &GPIO_InitStructure);
    sf_i2c_init(&i2c0_dev);

    //SC8815 PSTOP : push pull output
#if SC8815_PSTOP_NUM < 8 
    SC8815_PSTOP->CFGLR = ( SC8815_PSTOP->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_PSTOP_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( SC8815_PSTOP_NUM << 2 ) ) ;
#else
    SC8815_PSTOP->CFGHR = ( SC8815_PSTOP->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_PSTOP_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( SC8815_PSTOP_NUM -8 ) << 2 ) ) ;
#endif

    //SC8815 CEN : push pull output
#if SC8815_CEN_NUM < 8 
    SC8815_CEN->CFGLR = ( SC8815_CEN->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_CEN_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( SC8815_CEN_NUM << 2 ) ) ;
#else
    SC8815_CEN->CFGHR = ( SC8815_CEN->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_CEN_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( SC8815_CEN_NUM -8 ) << 2 ) ) ;
#endif

    //SC8815 DISC : push pull output
#if SC8815_DISC_NUM < 8 
    SC8815_DISC->CFGLR = ( SC8815_DISC->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( SC8815_DISC_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( SC8815_DISC_NUM << 2 ) ) ;
#else
    SC8815_DISC->CFGHR = ( SC8815_DISC->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( SC8815_DISC_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( SC8815_DISC_NUM - 8 ) << 2 ) ) ;
#endif

    //PSTOP High
    SC8815_PSTOP->BSHR = SC8815_PSTOP_PIN ;
    //DISC Low
    SC8815_DISC->BCR = SC8815_DISC_PIN ;
}

// 使用此库需要提供以下外部函数,这是SC8815库需要使用的函数
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData) // 通过I2C向设备寄存器写一个字节
{
    // 写数据
    uint8_t data[2] = {RegAddress, ByteData};
    struct sf_i2c_msg msgs_t ;

    //target reg address
    msgs_t.addr = SlaveAddress << 1 ;
    msgs_t.flags = SF_I2C_FLAG_WR ;
    msgs_t.buf = data ;
    msgs_t.len = 2 ;

#if USE_FreeRTOS
    taskENTER_CRITICAL();
#endif

    sf_i2c_transfer( &i2c0_dev , &msgs_t , 1 );

#if USE_FreeRTOS
    taskEXIT_CRITICAL();
#endif
}

uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress) // 通过I2C从设备寄存器读一个字节
{
    uint8_t data;
    struct sf_i2c_msg msgs_t[2] ;

    //target reg address
    msgs_t[0].addr = SlaveAddress << 1 ;
    msgs_t[0].flags = SF_I2C_FLAG_WR ;
    msgs_t[0].buf = &RegAddress ;
    msgs_t[0].len = 1 ;
    //
    msgs_t[1].addr = SlaveAddress << 1 ;
    msgs_t[1].flags = SF_I2C_FLAG_RD ;
    msgs_t[1].buf = &data ;
    msgs_t[1].len = 1 ;

#if USE_FreeRTOS
    taskENTER_CRITICAL();
#endif

    sf_i2c_transfer( &i2c0_dev , msgs_t , 2 );

#if USE_FreeRTOS
    taskEXIT_CRITICAL();
#endif

    return data;
}

void SoftwareDelay(uint8_t ms) // 软件延时毫秒
{
    soft_delay_ms( ms );
}

// PSTOP 引脚
void SC8815_PSTOP_Set(uint8_t data)
{
    if( data )
        SC8815_PSTOP->BSHR = SC8815_PSTOP_PIN ;
    else
        SC8815_PSTOP->BCR = SC8815_PSTOP_PIN ;
}

// CE 引脚
void SC8815_CE_Set(uint8_t data)
{
    if( data )
        SC8815_CEN->BSHR = SC8815_CEN_PIN ;
    else
        SC8815_CEN->BCR = SC8815_CEN_PIN ;
}

// DISC_CTL
void SC8815_DISC_CTL_Set(uint8_t data)
{
    if( data && ( SC8815_PSTOP->OUTDR & SC8815_PSTOP_PIN ) )
        SC8815_DISC->BSHR = SC8815_DISC_PIN ;
    else
        SC8815_DISC->BCR = SC8815_DISC_PIN ;
}

SC8815_BatteryConfigTypeDef SC8815_BatteryConfigStruct = {0};
SC8815_HardwareInitTypeDef SC8815_HardwareInitStruct = {0};
SC8815_InterruptStatusTypeDef SC8815_InterruptMaskInitStruct = {0};

void sc8815_user_init()
{
    //uint8_t send[] = {0x00};
    sc8815_i2c_init();

    SoftwareDelay(10); // 必要的启动延时

    // 配置 SC8815 电池参数选项
    SC8815_BatteryConfigStruct.IRCOMP = SCBAT_IRCOMP_20mR;
    SC8815_BatteryConfigStruct.VBAT_SEL = SCBAT_VBAT_SEL_Internal;
    SC8815_BatteryConfigStruct.CSEL = SCBAT_CSEL_4S;
    SC8815_BatteryConfigStruct.VCELL = SCBAT_VCELL_4v50;
    SC8815_BatteryConfig(&SC8815_BatteryConfigStruct);

    // 配置 SC8815 硬件参数选项
    SC8815_HardwareInitStruct.IBAT_RATIO = SCHWI_IBAT_RATIO_12x;
    SC8815_HardwareInitStruct.VBAT_RATIO = SCHWI_VBAT_RATIO_12_5x;
    SC8815_HardwareInitStruct.IBUS_RATIO = SCHWI_IBUS_RATIO_6x;
    SC8815_HardwareInitStruct.VBUS_RATIO = SCHWI_VBUS_RATIO_12_5x;
    SC8815_HardwareInitStruct.VINREG_Ratio = SCHWI_VINREG_RATIO_100x;
    SC8815_HardwareInitStruct.SW_FREQ = SCHWI_FREQ_450KHz;
    SC8815_HardwareInitStruct.DeadTime = SCHWI_DT_20ns;
    SC8815_HardwareInitStruct.ICHAR = SCHWI_ICHAR_IBUS;
    SC8815_HardwareInitStruct.TRICKLE = SCHWI_TRICKLE_Disable;
    SC8815_HardwareInitStruct.TERM = SCHWI_TERM_Disable;
    SC8815_HardwareInitStruct.FB_Mode = SCHWI_FB_External;
    SC8815_HardwareInitStruct.TRICKLE_SET = SCHWI_TRICKLE_SET_70;
    SC8815_HardwareInitStruct.OVP = SCHWI_OVP_Disable;
    SC8815_HardwareInitStruct.DITHER = SCHWI_DITHER_Disable;
    SC8815_HardwareInitStruct.SLEW_SET = SCHWI_SLEW_1mV_us;
    SC8815_HardwareInitStruct.AD_SCAN = SCHWI_ADC_Enable;
    SC8815_HardwareInitStruct.ILIM_BW = SCHWI_ILIM_BW_5KHz;
    SC8815_HardwareInitStruct.LOOP = SCHWI_LOOP_Normal;
    SC8815_HardwareInitStruct.ShortFoldBack = SCHWI_SFB_Enable;
    SC8815_HardwareInitStruct.EOC = SCHWI_EOC_1_25;
    SC8815_HardwareInitStruct.PFM = SCHWI_PFM_Disable;
    SC8815_HardwareInit(&SC8815_HardwareInitStruct);

    // 配置 SC8815 中断屏蔽选项
    SC8815_InterruptMaskInitStruct.AC_OK = sENABLE;
    SC8815_InterruptMaskInitStruct.INDET = sENABLE;
    SC8815_InterruptMaskInitStruct.VBUS_SHORT = sENABLE;
    SC8815_InterruptMaskInitStruct.OTP = sENABLE;
    SC8815_InterruptMaskInitStruct.EOC = sENABLE;
    SC8815_ConfigInterruptMask(&SC8815_InterruptMaskInitStruct);
    /***现在可以设置 PSTOP 引脚为低, 启动 SC8815 功率转换****/
    SC8815_ADC_Enable();

    SC8815_VINREG_SetVoltage(3300);
    // SC8815_OTG_Disable();

    /*** 示例2, 设置为反向放电模式 ***/
    SC8815_SetBatteryCurrLimit(11000);
    SC8815_SetBusCurrentLimit(11000);
    SC8815_SetOutputVoltage(3300);
    SC8815_OTG_Enable();

    sc8815_user_power_on_off(0);
    //sc8815_user_power_on_off(1);  // 打开输出
    //SC8815_PSTOP_Set(1);
    //SoftwareDelay(10);
    //SC8815_SetBatteryCurrLimit(5000);
    SC8815_SetBusCurrentLimit(11000);

    #if 1
    // 读取芯片数据
    for (int i = 0; i < 0x19; i++)
    {
        uint8_t data = I2C_ReadRegByte(SC8815_ADDR, i);
        dbg_printf("sc8815 data:[%d] 0x%02X.\r\n", i , data );
    }
    #endif
}

#define NTC_BETA  3450.0    //should match the Beta Coefficient of the thermistor
#define NTC_R0    10.0      //unit kohm 
#define Rp_Vaule  10.0      //unit kohm
#define Rp_VCC    3.3

double Steinhart_Hart_B_NTC_Temp(uint16_t adc_in)
{
    double temp = adc_in / 1000.0 ;
    temp = Rp_Vaule * temp / ( 3.3 - temp ) ; //NTC Resisters
    temp = logf( temp / NTC_R0 ) / NTC_BETA + 1.0 / 298.15 ; 
    temp = 1 / temp - 273.15 ; //convert unit k to ℃
    return temp ;
}

void sc8815_user_set(uint8_t mode, uint16_t voltage, uint16_t current)
{
    dbg_printf("set %d %dmV %dmA.\r\n", mode, voltage, current);
    SC8815_SetOutputVoltage(voltage);
}

void sc8815_protect_set(uint16_t voltage, uint16_t current)
{
    SC8815_SetBatteryCurrLimit(current);
    SC8815_SetBusCurrentLimit(current);
};

// bit0 PSTOP 引脚, 1:打开 0:关闭
// bit1 DISC_CTL 引脚, 1:打开 0:关闭
void sc8815_user_power_on_off(uint8_t data)
{
    dbg_printf("power on off %d\r\n", data);
    uint8_t pstop = 0;
    uint8_t disc_ctl = 0;

    pstop = ( data & 0x01 ) ? 0 : 1;
    disc_ctl = ( data & 0x01 ) ? 0 : 1;

    if( pstop )
    {
        SC8815_PSTOP_Set(pstop);
        SoftwareDelay(10);
        SC8815_DISC_CTL_Set(disc_ctl);
    }
    else 
    {
        SC8815_DISC_CTL_Set(disc_ctl);
        SoftwareDelay(10);
        SC8815_PSTOP_Set(pstop);
    }
}

uint16_t VbusVolt = 0;
uint16_t VbusCurr = 0;
uint16_t BattVolt = 0;
uint16_t BattCurr = 0;
int16_t NtcResistor = 0;

void sc8815_data_upadte( void )
{
    //uint8_t tmp;
    //tmp = I2C_ReadRegByte(SC8815_ADDR, SCREG_STATUS);     //读取状态寄存器

    // SC8815中断引脚
    if ( ( SC8815_INT->INDR & SC8815_INT_PIN ) != 0)
    {
        SC8815_ReadInterrupStatus(&SC8815_InterruptMaskInitStruct); // MCU 收到 SC8815 中断后调用此函数读 SC8815 中断 (读中断状态后将清除中断状态位)

        if (SC8815_InterruptMaskInitStruct.AC_OK == 1) // 检查 AC_OK 中断是否触发
        {
            // dbg_printf("AC_OK\n");
            // AC_OK 中断处理代码
            power_data.sc8815_interrupt |= INTERR_ACIN;
        }
        else if (SC8815_InterruptMaskInitStruct.EOC == 1)
        {
            //dbg_printf("EOC\n");
            // EOC 中断处理代码
            power_data.sc8815_interrupt |= INTERR_EOC;
        }
        else if (SC8815_InterruptMaskInitStruct.INDET == 1)
        {
            //dbg_printf("INDET\n");
            // INDET 中断处理代码
            power_data.sc8815_interrupt |= INTERR_INDET;
        }
        else if (SC8815_InterruptMaskInitStruct.VBUS_SHORT == 1)
        {
            //dbg_printf("VBUS_SHORT\n");
            // VBUS_SHORT 中断处理代码
            power_data.sc8815_interrupt |= INTERR_VBUS_SHORT;
            sc8815_user_power_on_off(0);
        }
        else if (SC8815_InterruptMaskInitStruct.OTP == 1)
        {
            //dbg_printf("OTP\n");
            // OTP 中断处理代码
            power_data.sc8815_interrupt |= INTERR_OTP;
            sc8815_user_power_on_off(0);
        }
        else
        {
            /* code */
        }
    }
    VbusVolt = SC8815_Read_VBUS_Voltage();
    VbusCurr = SC8815_Read_VBUS_Current();
    BattVolt = SC8815_Read_BATT_Voltage();
    BattCurr = SC8815_Read_BATT_Current();
    //NtcResistor = SC8815_Read_ADIN_Voltage();
    NtcResistor = ( int16_t )( Steinhart_Hart_B_NTC_Temp( SC8815_Read_ADIN_Voltage())* 10);

    power_data.sc8815_vbus_voltage = VbusVolt;
    power_data.sc8815_vbus_current = VbusCurr;
    power_data.sc8815_vbat_voltage = BattVolt;
    power_data.sc8815_vbat_current = BattCurr;
    power_data.ntc_temp = NtcResistor ;

    //dbg_printf("SC8815 VbusVolt:%d mV, VbusCurr:%d mA, BattVolt:%d mV, BattCurr:%d mA.\r\n", VbusVolt, VbusCurr, BattVolt, BattCurr );
}