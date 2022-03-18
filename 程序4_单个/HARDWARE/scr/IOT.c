#include "stm32f10x.h" 
#include "IOT.h"       
#include "delay.h"
#include "iot_enable.h" 
#include "usart1.h" 

 
volatile u8 key = 0; 

void IOT_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
		          
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);                                
	
            		                            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4  ;  
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                            
	GPIO_Init(GPIOA, &GPIO_InitStructure);     
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	PA4_ON;  //关闭继电器
}


//PB9
void GPIO_EXTI(void)
{
		
		GPIO_InitTypeDef GPIO_InitStructure; 
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStruct;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
		GPIO_Init(GPIOB,&GPIO_InitStructure);

		EXTI_InitStructure.EXTI_Line = EXTI_Line9;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_Init(&EXTI_InitStructure);
			
		NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x03;
		NVIC_Init(&NVIC_InitStruct);	
	
}	

void EXTI9_5_IRQHandler(void)
{	
		if(EXTI_GetITStatus(EXTI_Line9) == RESET)
		{
					Delay_Ms(10);
					if((GPIOB->IDR&(1<<9)) == 0)
					{
							key = 1;	
					}	
		
				EXTI_ClearITPendingBit(EXTI_Line9);
		}



}	


void key_init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure; 

		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOB,ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  
	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                            
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOB,&GPIO_InitStructure);
	
		 PA4_ON; 		//关闭继电器

}	

u8 key_filtering(int cnt)
{
		int i = cnt;
		
		 while(cnt--)
		 {
				if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == 0 )
					i --;
				
				Delay_Ms(1);	
		 }

		 
		 if(i == 0)
			 return 1;
		 else
			 return 0;
		 
}

u8  scan_key(void)
{
		static u8 state = 1;
	
		u8 key_value=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
		//u1_printf("input1:%d\r\n",GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9));
		Delay_Ms(5);
		if(key_value == 0)
		{
				Delay_Ms(5);
				//key_value=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
				if( key_filtering(20) && state)
				{
					//u1_printf("input2:%d\r\n",key_value);
					state = 0;
					return 1;			
				}	
		
				
		}else state = 1;

		return 0;
}	


void IOT_AllOFF(void)
{
	PA1_OFF;
	PA4_ON;
	PA5_ON;
	PB8_ON;
}




