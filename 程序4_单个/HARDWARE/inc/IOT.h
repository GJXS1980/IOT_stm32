
#ifndef __IOT_H
#define __IOT_H

#define 	PB8_ON			GPIO_SetBits(GPIOB, GPIO_Pin_8)  
#define 	PB8_OFF			GPIO_ResetBits(GPIOB, GPIO_Pin_8)  

#define 	PB9_ON			GPIO_SetBits(GPIOB, GPIO_Pin_9)  
#define 	PB9_OFF			GPIO_ResetBits(GPIOB, GPIO_Pin_9)  

#define 	PA1_ON			GPIO_SetBits(GPIOA, GPIO_Pin_1)  
#define 	PA1_OFF			GPIO_ResetBits(GPIOA, GPIO_Pin_1)  

#define 	PA4_ON			GPIO_SetBits(GPIOA, GPIO_Pin_4)  
#define 	PA4_OFF			GPIO_ResetBits(GPIOA, GPIO_Pin_4)  

#define 	PA5_ON			GPIO_SetBits(GPIOA, GPIO_Pin_5)  
#define 	PA5_OFF			GPIO_ResetBits(GPIOA, GPIO_Pin_5)  


#define PB8_READ_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8) 
#define PB9_READ_STA   GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_9) 

#define PA4_READ_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4) 
#define PA5_READ_STA   GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) 

 
#define 	PWM_OPEN				TIM4->CCR3 = 1985  //1985
#define 	PWM_CLOSE				TIM4->CCR3 = 1000  //
#define 	PWM_STOP				TIM4->CCR3 = 1480
#define 	PWM_BACK				TIM4->CCR3 = 1985

void IOT_Init(void);
void IOT_AllOFF(void);

extern volatile u8 key;
void key_init(void);
void GPIO_EXTI(void);
void key_init(void);
u8  scan_key(void);

#endif


