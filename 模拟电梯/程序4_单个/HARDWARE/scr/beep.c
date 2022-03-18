#include "beep.h"

void beep_init(void)
{
		RCC->APB2ENR |= (1<<3);
		
		GPIOB->CRL &= ~((u32)0XF<<28);
		GPIOB->CRL |= (3<<28);
		
		GPIOB->ODR &=~(1<<7);			//¹Ø±ÕBEEP
}	


