#ifndef __BEEP_H_
#define __BEEP_H_
#include "stm32f10x.h"

#define BEEP_ON 	(GPIOB->ODR |= (1<<7))
#define BEEP_OFF 	(GPIOB->ODR &=~(1<<7))	

void beep_init(void);




#endif
















