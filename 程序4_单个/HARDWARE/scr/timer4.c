#include "stm32f10x.h"  

char timer4_flag = 0;   //1:��ʱ��4�жϣ������ݵ�

/*-------------------------------------------------*/
/*����������ʱ��4��ʼ��                            */
/*��  ����arr���Զ���װֵ   0~65535                */
/*��  ����psc��ʱ��Ԥ��Ƶ�� 0~65535                */
/*����ֵ����                                       */
/*˵  ������ʱʱ�䣺arr*psc*1000/72000000  ��λms  */
/*-------------------------------------------------*/
void TIM4_Init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;            
	NVIC_InitTypeDef NVIC_InitStructure;                          
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);           	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	              
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;                  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;     
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);            
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);                    
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                          
	TIM_Cmd(TIM4,DISABLE);                                                                  
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn;                 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;       
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;              
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;                 
	NVIC_Init(&NVIC_InitStructure);                               
}


void TIM4_IRQHandler(void)
{

}


