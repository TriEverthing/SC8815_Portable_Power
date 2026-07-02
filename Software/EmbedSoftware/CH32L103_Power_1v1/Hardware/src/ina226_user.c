#include "ch32l103x_bsp.h"
#include "sf_i2c.h"
#include "ina226.h"
#include "ina226_user.h"
#include "power_data.h"


struct ina226_t ina226_obj;

/* Exported define -----------------------------------------------------------*/
//SCL
#define INA226_SCL            GPIOB
#define INA226_SCL_RCC_EN     RCC_IOPBEN
#define INA226_SCL_NUM        15
#define INA226_SCL_PIN        (((uint16_t)0x0001) << INA226_SCL_NUM )
//SDA
#define INA226_SDA            GPIOB
#define INA226_SDA_RCC_EN     RCC_IOPBEN
#define INA226_SDA_NUM        1
#define INA226_SDA_PIN        (((uint16_t)0x0001) << INA226_SDA_NUM )
//INT
#define INA226_ALT            GPIOB
#define INA226_ALT_RCC_EN     RCC_IOPBEN
#define INA226_ALT_NUM        0
#define INA226_ALT_PIN        (((uint16_t)0x0001) << INA226_ALT_NUM )


/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_set_scl_low( void )
{
    INA226_SCL->BCR = INA226_SCL_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_set_scl_high( void )
{
    INA226_SCL->BSHR = INA226_SCL_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_set_sda_low( void )
{
    INA226_SDA->BCR = INA226_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_set_sda_high( void )
{
    INA226_SDA->BSHR = INA226_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
uint8_t ina226_read_sda( void )
{
   return ( ( INA226_SDA->INDR & INA226_SDA_PIN ) >> INA226_SDA_NUM ) ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_sda_dir_input( void )
{
    INA226_SDA->BSHR = INA226_SDA_PIN ;
}

/**
 * @brief  Initialization i2c
 * @param  dev : Pointer to iic structure
 * @return none
 */
void ina226_sda_dir_output( void )
{
    INA226_SDA->BSHR = INA226_SDA_PIN ;
}


/* Exported types ------------------------------------------------------------*/
static struct sf_i2c_dev i2c1_dev = 
{
    .name               = "i2c1",
    .speed              = 4, /*! speed:105Hz */
    .delay_us           = soft_delay_us ,
    .ops.sda_low        = ina226_set_sda_low ,
    .ops.sda_high       = ina226_set_sda_high ,
    .ops.scl_low        = ina226_set_scl_low ,
    .ops.scl_high       = ina226_set_scl_high ,
    .ops.sda_read_level = ina226_read_sda ,
    .ops.sda_set_input  = ina226_sda_dir_input ,
    .ops.sda_set_output = ina226_sda_dir_output ,
};

void ina226_i2c_init()
{
    //ina226 SCL : push pull output
#if INA226_SCL_NUM < 8 
    INA226_SCL->CFGLR = ( INA226_SCL->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( INA226_SCL_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( INA226_SCL_NUM << 2 ) ) ;
#else
    INA226_SCL->CFGHR = ( INA226_SCL->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( INA226_SCL_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000003) << ( ( INA226_SCL_NUM -8 ) << 2 ) ) ;
#endif

    //ina226 SDA : open drain output
#if INA226_SDA_NUM < 8 
    INA226_SDA->CFGLR = ( INA226_SDA->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( INA226_SDA_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000007) << ( INA226_SDA_NUM << 2 ) ) ;
#else
    INA226_SDA->CFGHR = ( INA226_SDA->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( INA226_SDA_NUM - 8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000007) << ( ( INA226_SDA_NUM -8 ) << 2 ) ) ;
#endif
    sf_i2c_init(&i2c1_dev);

    //ina226 INT : interrupt for ina226 , pull up input
#if INA226_ALT_NUM < 8 
    INA226_ALT->CFGLR = ( INA226_ALT->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( INA226_ALT_NUM << 2 ) ) ) )  | ( (uint32_t )(0x00000008) << ( INA226_ALT_NUM << 2 ) ) ;
    INA226_ALT->BSHR = INA226_ALT_PIN ;
#else
    INA226_ALT->CFGHR = ( INA226_ALT->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( INA226_ALT_NUM -8 )  << 2 ) ) ) )  | ( (uint32_t )(0x00000008) << ( ( INA226_ALT_NUM -8 ) << 2 ) ) ;
    INA226_ALT->BSHR = INA226_ALT_PIN ;
#endif   
    
}


uint8_t ina226_write16(uint8_t addr, uint8_t reg, uint16_t data)
{
    uint8_t buf[3] ;
    buf[0] = reg ;
    buf[1] = data >> 8 ;
    buf[2] = data & 0xFF ;
    struct sf_i2c_msg msgs_t ;

    //target reg address
    msgs_t.addr = addr << 1 ;
    msgs_t.flags = SF_I2C_FLAG_WR ;
    msgs_t.buf = buf ;
    msgs_t.len = 3 ;

#if USE_FreeRTOS
    taskENTER_CRITICAL();
#endif

    sf_i2c_transfer( &i2c1_dev , &msgs_t , 1 );

#if USE_FreeRTOS
    taskEXIT_CRITICAL();
#endif
}

uint8_t ina226_read16(uint8_t addr, uint8_t reg, uint16_t *data)
{
    uint8_t buf[2];
    struct sf_i2c_msg msgs_t[2] ;

    //target reg address
    msgs_t[0].addr = addr << 1 ;
    msgs_t[0].flags = SF_I2C_FLAG_WR ;
    msgs_t[0].buf = &reg ;
    msgs_t[0].len = 1 ;
    //
    msgs_t[1].addr = addr << 1 ;
    msgs_t[1].flags = SF_I2C_FLAG_RD ;
    msgs_t[1].buf = buf ;
    msgs_t[1].len = 2 ;

#if USE_FreeRTOS
    taskENTER_CRITICAL();
#endif

    sf_i2c_transfer( &i2c1_dev , msgs_t , 2 );

#if USE_FreeRTOS
    taskEXIT_CRITICAL();
#endif

    *data = buf[0] << 8 | buf[1];
};

void ina226_user_int()
{
    ina226_i2c_init();
    ina226_obj.addr = 0x40;
    ina226_obj.write16 = ina226_write16 ;
    ina226_obj.read16 = ina226_read16 ;

    ina226_init(&ina226_obj);
    dbg_printf("ina226 def config 0x4127, read: 0x%04X.\r\n", ina226_config_read(&ina226_obj));

    ina226_config(&ina226_obj, INA226_AVERAGES_64, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    
    dbg_printf("ina226 set config: 0x%04X.\r\n", ina226_config_read(&ina226_obj));
    dbg_printf("ina226_get_manufacturer_id: 0x%04X.\r\n", ina226_get_manufacturer_id(&ina226_obj));

    // 05 READ
    dbg_printf("ina226_calibrate_read: 0x%04X.\r\n", ina226_calibrate_read(&ina226_obj));

    // 0.05欧姆采样电阻 12A 最大电流
    ina226_calibrate(&ina226_obj, 0.005055 , 12 );
    dbg_printf("ina226_calibrate_read: 0x%04X.\r\n", ina226_calibrate_read(&ina226_obj));

}

void ina226_data_update(void)
{
    //float shunt_voltage ;
    float bus_voltage , current , power ;

    //shunt_voltage = ina226_read_shunt_voltage(&ina226_obj);
    bus_voltage = ina226_read_bus_voltage(&ina226_obj);
    current = ina226_read_shunt_current(&ina226_obj);
    power = ina226_read_bus_power(&ina226_obj);

    power_data.ina_read_voltage =  (int32_t)(bus_voltage*1000);
    power_data.ina_read_current = (int32_t)(current*1000);
    power_data.ina_read_power = (int32_t)(power*1000);
}
