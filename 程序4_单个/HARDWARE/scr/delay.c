#include "stm32f10x.h"  

/*
@ 分频 只是影响定时器的速度，及记一次所需要的时间

	最大溢出时间： 则是定时器本身有关，也就是24位数的定时器
*/

void Delay_Init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

}

//最大延时时间 = 2的24次方-1 * 1/9 us;
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
