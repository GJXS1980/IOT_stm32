#include "pwm.h"
#include "usart1.h" 
#include "IOT.h"
#include "iot_enable.h"
#include "beep.h"

extern uint8_t   new_data;

void tim4_pwm_init(unsigned short int arr, unsigned short int psc)
{

	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStructrue;
	GPIO_InitTypeDef 					GPIO_InitStructrue;
	TIM_OCInitTypeDef 				TIM_OCInitStructrue;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE);
	
	GPIO_InitStructrue.GPIO_Pin    = GPIO_Pin_8;
	GPIO_InitStructrue.GPIO_Mode   = GPIO_Mode_AF_PP;
	GPIO_InitStructrue.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructrue);
	
	TIM_TimeBaseInitStructrue.TIM_Prescaler = psc;  
	TIM_TimeBaseInitStructrue.TIM_Period =arr ; 	
	TIM_TimeBaseInitStructrue.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructrue.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructrue);
	
	TIM_OCInitStructrue.TIM_OCMode = TIM_OCMode_PWM1 ;
	TIM_OCInitStructrue.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructrue.TIM_OCPolarity = TIM_OCNPolarity_High;
	TIM_OC3Init(TIM4,&TIM_OCInitStructrue);
	
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);
	TIM_Cmd(TIM4,ENABLE);
	
	TIM4->CCR3 = 0;
	
}


void TIM2_INIT(unsigned short int arr, unsigned short int psc)																			//30S
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;            //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                          //����һ�������жϵı���
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //�����ж��������飺��2�� �������ȼ���0 1 2 3 �����ȼ���0 1 2 3		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);           //ʹ��TIM3ʱ��	
	TIM_DeInit(TIM2);                                             //��ʱ��3�Ĵ����ָ�Ĭ��ֵ	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	         		 	//�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		       	    //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);            //����TIM2
	
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);                    //�������жϱ�־λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);                      //ʹ��TIM3����ж�    
	TIM_Cmd(TIM2,DISABLE);                                         //��TIM2                          
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                 //����TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;       //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                               //�����ж�
}


void TIM2_IRQHandler(void)
{
	
#if Door_IOT	
	static u8 on = 0;

#endif
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
#if Trashcan_IOT		
//		u1_printf("CNT3=%d\r\n",TIM2->CNT);
		PWM_STOP;
		TIM_Cmd(TIM2,DISABLE);
#endif
		
#if Door_IOT
		
		if(on)
	  {	
			BEEP_ON;
		}
		else
		{
			BEEP_OFF;
			new_data = 0;	
			TIM_Cmd(TIM2,DISABLE);
		}	
		
#endif
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
	}

}



