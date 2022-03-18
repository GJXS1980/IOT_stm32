#include "stm32f10x.h"  
#include "led.h"        

/*-------------------------------------------------*/
/*����������ʼ��LED����                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     //����һ������GPIO�ı���
		
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);                                //ʹ��GPIOA�˿�ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                                //ʹ��GPIOB�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_12 ;                                             //׼������PA5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              //���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		                              //����PA5
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;                                             //׼������PA5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              //���������ʽ
	GPIO_Init(GPIOC, &GPIO_InitStructure);            		                              //����PA5
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1  | GPIO_Pin_10 | GPIO_Pin_12;                //׼������PB1 PB10 PB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     //IO����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                              //���������ʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);            		                              //����PB1 PB10 PB11
	
	LED_AllOn();   //����LEDϨ��
	
}

/*-------------------------------------------------*/
/*����������������LED                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_AllOn(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}

/*-------------------------------------------------*/
/*��������Ϩ������LED                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LED_AllOff(void)
{
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}



