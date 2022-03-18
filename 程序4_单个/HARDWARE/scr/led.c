#include "stm32f10x.h"  
#include "led.h"        

/*-------------------------------------------------*/
/*函数名：初始化LED函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
		
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);                                
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                                
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_12 ;                                             
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		                              
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;                                             
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		                              
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_10 | GPIO_Pin_12;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		                              
	
	LED_AllOn();   
	
}

/*-------------------------------------------------*/
/*函数名：点亮所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_AllOn(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}

/*-------------------------------------------------*/
/*函数名：熄灭所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_AllOff(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}



