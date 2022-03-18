#include "stm32f10x.h"  

/*
@ ��Ƶ ֻ��Ӱ�춨ʱ�����ٶȣ�����һ������Ҫ��ʱ��

	������ʱ�䣺 ���Ƕ�ʱ�������йأ�Ҳ����24λ���Ķ�ʱ��
*/

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

}

//�����ʱʱ�� = 2��24�η�-1 * 1/9 us;
void Delay_Us(unsigned int us)
{		
	unsigned int temp;	                     
	
	SysTick->LOAD=us*9;                       
	                                          
	SysTick->VAL=0x00;                        
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  
	do{
		temp=SysTick->CTRL;                   	
	}while(temp&0x01&&!(temp&(1<<16)));       
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;   
}


void Delay_Ms(unsigned int ms)
{

	unsigned char i;      
	unsigned int temp;   
	
	if(ms<1800){                                   
		SysTick->LOAD=(unsigned int)ms*9*1000;             
		                                           
		SysTick->VAL=0x00;                         
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;   
		do{
			temp=SysTick->CTRL;                    
		}while(temp&0x01&&!(temp&(1<<16)));        
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;}   
	
	else{ 
		for(i=0;i<(ms/1800);i++){                     
			SysTick->LOAD=(unsigned int)1800*9*1000;          
													  
			SysTick->VAL=0x00;                        
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  
			do{
				temp=SysTick->CTRL;                   
			}while(temp&0x01&&!(temp&(1<<16)));       
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;}  
		
		SysTick->LOAD=(unsigned int)(ms%1800)*9*1000;   
		                                              
		SysTick->VAL =0x00;                           
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;        
		do{
			temp=SysTick->CTRL;                       
		}while(temp&0x01&&!(temp&(1<<16)));           
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;      
	}
} 
