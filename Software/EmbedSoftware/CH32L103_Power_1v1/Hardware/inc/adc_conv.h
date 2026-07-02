#ifndef __ADC_CONV_H
#define __ADC_CONV_H

#include "stdio.h"
#include "stdint.h"

enum
{
    ADC_TYPE_TEMP,
    ADC_TYPE_VIN,
    ADC_TYPE_MAX
};

void adc_conv_init();

void ch32_adc_upadte(void);

#endif