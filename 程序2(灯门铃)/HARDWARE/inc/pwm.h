
#ifndef _PWM_H
#define _PWM_H
#include "stm32f10x.h"

void tim4_pwm_init(unsigned short int arr, unsigned short int psc);
void TIM2_INIT(unsigned short int arr, unsigned short int psc);
#endif
