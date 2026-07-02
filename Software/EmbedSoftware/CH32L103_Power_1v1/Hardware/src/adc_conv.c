#include "ch32l103x_bsp.h"
#include "adc_conv.h"
#include "power_data.h"
#include <math.h>


/* CFG Keys */
#define ADC_CFG_KEY1        ((uint32_t)0x45670123)
#define ADC_CFG_KEY2        ((uint32_t)0xCDEF89AB)

//ADC Channel1
#define VIN_ADC              ADC1
#define VIN_ADC_RCC_EN       RCC_ADCEN
//ADC GPIO PA1
#define VIN_ADC_IO           GPIOA
#define VIN_ADC_IO_RCC_EN    RCC_IOPAEN
#define VIN_ADC_PIN_NUMS     1
#define VIN_ADC_PIN          (((uint16_t)0x0001) << VIN_ADC_PIN_NUMS )

/*
    // 电源输入
    PA1 VIN_ADC
*/
uint16_t adc_value[4];
int16_t Calibrattion_Val = 0;

int16_t Get_Adc_CalibrationValue(ADC_TypeDef *ADCx);


void adc_conv_init()
{
    /* enable GPIO clock */
    RCC->PB2PCENR |= VIN_ADC_IO_RCC_EN ;
    /* enable ADC clock */
    RCC->PB2PCENR |= VIN_ADC_RCC_EN ;
    /* enable DMA clock */
    RCC->HBPCENR |= RCC_DMAEN ;
    /* config ADC clock */
    RCC->CFGR0 &= ~RCC_ADC_PRE_ADJ ; //
    RCC->CFGR0 = ( RCC->CFGR0 & (~RCC_ADCPRE)) | RCC_ADCPRE_DIV6 ; //96MHz/6=16MHz

    /* config the GPIO as analog mode */
#if VIN_ADC_PIN_NUMS < 8 
    VIN_ADC_IO->CFGLR = ( VIN_ADC_IO->CFGLR  & ( ~( (uint32_t )(0x0000000F) << ( VIN_ADC_PIN_NUMS << 2 ) ) ) ) ;
#else
    VIN_ADC_IO->CFGHR = ( VIN_ADC_IO->CFGHR  & ( ~( (uint32_t )(0x0000000F) << ( ( VIN_ADC_PIN_NUMS - 8 )  << 2 ) ) ) ) ;
#endif

    /* DMA config */
    /* initialize DMA single data mode */
    //DIR:form peripheral to memory , peripheral inc disable 
    //circle mode , memroy inc enable , memory width 16bits , peripheral width 16bits , priority high
    DMA1_Channel1->CFGR = DMA_CFGR1_CIRC | DMA_CFGR1_MINC | DMA_CFGR1_MSIZE_0 | DMA_CFGR1_PSIZE_0 | DMA_CFGR1_PL_1 ;
    //DMA numbers
    DMA1_Channel1->CNTR = 3U ;
    //DMA peripheral address
    DMA1_Channel1->PADDR = (uint32_t)(&ADC1->RDATAR) ;
    //DMA memory address
    DMA1_Channel1->MADDR = (uint32_t)(&adc_value);
  
    /* enable DMA channel */
    DMA1_Channel1->CFGR |= DMA_CFGR1_EN ;

    Calibrattion_Val = Get_Adc_CalibrationValue(ADC1);
    
    /* ADC config */
    /* ADC continuous function enable */
    VIN_ADC->CTLR2 |= ADC_CONT ;
    /* ADC scan function enable */
    VIN_ADC->CTLR1 |= ADC_SCAN ;
    /* ADC data alignment config */
    VIN_ADC->CTLR2 &= ~ADC_ALIGN ;
    /* ADC channel length config */
    VIN_ADC->RSQR1 = ( VIN_ADC->RSQR1 & ( ~ ADC_L ) ) | ( 2 << 20 ) ;   //ADC1 , ADC16 for temp , ADC17 for VREFINT

    /* ADC regular channel config */
    VIN_ADC->RSQR3 = ( VIN_ADC->RSQR3 & ( ~ ADC_SQ1 ) ) | ( 1 << 0 ) ;      //ADC1
    VIN_ADC->RSQR3 = ( VIN_ADC->RSQR3 & ( ~ ADC_SQ2 ) ) | ( 16 << 5 ) ;     //ADC16
    VIN_ADC->RSQR3 = ( VIN_ADC->RSQR3 & ( ~ ADC_SQ3 ) ) | ( 17 << 10 ) ;    //ADC17
    /* sample period 55.5  , Tconv = Ts + 12.5×Tadcclk */
    VIN_ADC->CFG &= ~ADC_LP ;  //low power mode , smaple rate < 1Msa/s;
    VIN_ADC->SAMPTR2 = ( VIN_ADC->SAMPTR2 & ( ~ ADC_SMP1  ) ) | ( 5 << 3 ) ;  //ADC01 , 16MHz / ( 55.5 + 12.5 ) = 235KSa/s
    VIN_ADC->SAMPTR1 = ( VIN_ADC->SAMPTR1 & ( ~ ADC_SMP16 ) ) | ( 5 << 18 ) ; //ADC16 , 16MHz / ( 55.5 + 12.5 ) = 235KSa/s
    VIN_ADC->SAMPTR1 = ( VIN_ADC->SAMPTR1 & ( ~ ADC_SMP17 ) ) | ( 5 << 21 ) ; //ADC17 , 16MHz / ( 55.5 + 12.5 ) = 235KSa/s

    /* ADC trigger config */
    VIN_ADC->CTLR2 |= ADC_EXTSEL  ;

    /* Temperature Sensor and VREFINT Enable */
    /* enable ADC interface */
    VIN_ADC->CTLR2 |= ADC_ADON | ADC_TSVREFE ;
    soft_delay_ms(1U);

    VIN_ADC->CFG |= ADC_FIFO_EN ;
    /* ADC calibration and reset calibration */
    VIN_ADC->CTLR2 |= ADC_RSTCAL ;
    /* check the RSTCLB bit state */
    while( VIN_ADC->CTLR2 & ADC_RSTCAL );
    /* enable ADC calibration process */
    VIN_ADC->CTLR2 |= ADC_CAL ;
    /* check the CLB bit state */
    while( VIN_ADC->CTLR2 & ADC_CAL );

    /* ADC DMA function enable */
    VIN_ADC->CTLR2 |= ADC_DMA ;
    /* ADC software trigger enable */
    VIN_ADC->CTLR2 |= ADC_RSWSTART | ADC_EXTTRIG ;
}


uint16_t adc_conv_cal(uint8_t type)
{
    int16_t ret = 0 ;
    uint16_t vref_int = 0 ;

    vref_int = adc_value[2] + Calibrattion_Val ;

    switch(type)
    {
        case ADC_TYPE_TEMP: 
            ret =  adc_value[1] + Calibrattion_Val ;
            if( ret < 0 || adc_value[1] == 0 )
                ret =  0;
            else if( ret  > 4095 || adc_value[1] == 4095 )
                ret =  4095;
            //ret = (int16_t)( ret * 1200 / vref_int );
            ret = ( (int16_t)((( 1450 - ( ret * 1200 / vref_int ) ) / 4.2 + 25 )*10))  ;
            break;
        case ADC_TYPE_VIN:
            ret =  adc_value[0] + Calibrattion_Val ;
            if( ret < 0 || adc_value[0] == 0 )
                ret =  0;
            else if( ret  > 4095 || adc_value[0] == 4095 )
                ret =  4095;
            // 分压电阻 ra:20k  rb:2k
            ret = (int16_t)( ret * 1200 / vref_int ) * 11 ;
            break;
        default:
            break;
    }
    return ret;

}

void ch32_adc_upadte(void)
{
    //power_data.ntc_temp = ( int16_t )adc_conv_cal(ADC_TYPE_TEMP);
    power_data.input_voltage = adc_conv_cal(ADC_TYPE_VIN);
}

/*********************************************************************
 * @fn      Get_CalibrationValue
 *
 * @brief   Get ADCx Calibration Value.
 *
 * @param   ADCx - where x can be 1 to select the ADC peripheral.
 *
 * @return  CalibrationValue
 */
int16_t Get_Adc_CalibrationValue(ADC_TypeDef *ADCx)
{
    __IO uint8_t  i, j;
    uint16_t      buf[10];
    __IO uint16_t t;
    /* SoftWare Triger */
    ADCx->CTLR2 |= ( 7 << 17 ) ;
    /* ADC continuous function disable */
    /* ADC scan function disable */
    VIN_ADC->CTLR2 &= ~ ( ADC_CONT | ADC_SCAN ) ;
    /* ADC ON */
    ADCx->CTLR2 |= ADC_ADON ;

    /* ADC FIFO EN */
    FLASH->KEYR = ADC_CFG_KEY1;
    FLASH->KEYR = ADC_CFG_KEY2;
    FLASH->MODEKEYR = ADC_CFG_KEY1;
    FLASH->OBKEYR = ADC_CFG_KEY2;
    while((*(vu32*)0x40022034) & (1<<29));  // wait unlock
    *(vu32*)0x4002202C |= (1<<9);           //offset calibration
    (*(vu32*)0x40022034) |= (1<<29);        //lock
    while(((*(vu32*)0x40022034) & (1<<29)) == 0); //wait lock
    ADCx->CFG |= ADC_FIFO_EN ;
    /* ADC calibration and reset calibration */
    ADCx->CTLR2 |= ADC_RSTCAL ;
    /* check the RSTCLB bit state */
    while( ADCx->CTLR2 & ADC_RSTCAL );
    /* enable ADC calibration process */
    ADCx->CTLR2 |= ADC_CAL ;
    /* check the CLB bit state */
    while( ADCx->CTLR2 & ADC_CAL );

    /* ADC regular channel config */
    ADCx->RSQR3 = ( ADCx->RSQR3 & ( ~ ADC_SQ1 ) ) | ( 18 << 0 ) ;      //ADC18 for AVDD/2
    /* Sample period 55.5  , Tconv = Ts + 12.5×Tadcclk */
    ADCx->CFG &= ~ADC_LP ;  //low power mode , smaple rate < 1Msa/s;
    ADCx->SAMPTR1 = ( ADCx->SAMPTR1 & ( ~ ADC_SMP18 ) ) | ( 5 << 24 ) ; //ADC18 , 16MHz / ( 55.5 + 12.5 ) = 235KSa/s

    for(i = 0; i < 10; i++)
    {
        /* ADC software trigger enable */
        ADCx->CTLR2 |= ADC_RSWSTART | ADC_EXTTRIG ;
        while( ( ADCx->STATR & ADC_EOC ) == 0 );
        buf[i] = (uint16_t)ADCx->RDATAR ;
    }
    for(i = 0; i < 10; i++)
    {
        for(j = 0; j < 9; j++)
        {
            if(buf[j] > buf[j + 1])
            {
                t = buf[j];
                buf[j] = buf[j + 1];
                buf[j + 1] = t;
            }
        }
    }

    t = 0;
    for(i = 0; i < 6; i++)
    {
        t += buf[i + 2];
    }
    t = (t / 6) + ((t % 6) / 3);
    /* ADC OFF */
    ADCx->CTLR2 &= ~ADC_ADON ;
    return (int16_t)(2048 - (int16_t)t);
}

