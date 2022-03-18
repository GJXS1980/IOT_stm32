#ifndef __ADC_H
#define __ADC_H_H
#include "stm32f10x.h"

void adc_init(void);
u16 Get_Adc(u8 ch);
u32 Get_result(u8 ch,u8 times);

#endif
















