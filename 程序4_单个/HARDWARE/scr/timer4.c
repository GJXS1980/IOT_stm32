#include "stm32f10x.h"  

char timer4_flag = 0;   //1:定时器4中断，有数据到

/*-------------------------------------------------*/
/*函数名：定时器4初始化                            */
/*参  数：arr：自动重装值   0~65535                */
/*参  数：psc：时钟预分频数 0~65535                */
/*返回值：无                                       */
/*说  明：定时时间：arr*psc*1000/72000000  单位ms  */
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


