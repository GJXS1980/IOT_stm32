#include "adc.h"

//PA6-6  PB0-8通道
void adc_init(void)
{

		RCC->APB2ENR |= (1<<2 | 1<<3 );	//PA PB
		GPIOA->CRL &= ~(15 << 24);  //模拟输入
		GPIOB->CRL &= ~(15<<0);
		
		RCC->APB2ENR |= (1<<9);		
		RCC->APB2RSTR |= (1<<9);		//复位ADC1
		RCC->APB2RSTR &= ~(1<<9);
		
		RCC->CFGR &= ~(3<<14);		//ADCPRE[1:0] 分频因子
		RCC->CFGR |= (2<<14);			//10：PCLK2 6分频后作为ADC时钟 72/6 = 12MHz
		
		ADC1->CR1 &= ~(15<<16);		
		ADC1->CR1 &= ~(1<<8);
		
		ADC1->CR2 &= ~(1<<1);
		ADC1->CR2 &= ~(7<<17);
		ADC1->CR2 |= (7<<17);			//软件控制转换
		ADC1->CR2 &= ~(1<<11);		//右对齐
		ADC1->CR2 |= (1<<20);
	
		ADC1->SQR1 &= ~(0XF<<20);	//设置转换规则为一个转换
		ADC1->SQR1 |= (1<<20);		//两个转换
		
		ADC1->SMPR2 &= ~(7<<18);	//通道6
		ADC1->SMPR2 |= (7<<18);		//111：239.5周期
		
		ADC1->SMPR2 &= ~(7<<24);		//通道8
		ADC1->SMPR2 |= (7<<24);
		
		ADC1->CR2 |= (1<<0 | 1<<3);	//启动转换
		while((ADC1->CR2)&(1<<3));	//硬件清零
		
		ADC1->CR2 |= (1<<2);		//启动校验
		while((ADC1->CR2)&(1<<2));
		
}

u16 Get_Adc(u8 ch)
{
		ADC1->SQR3 &= 0XFFFFFFE0;
		ADC1->SQR3 |=ch;
		ADC1->CR2  |= (1<<22);
		while(!(ADC1->SR&1<<1));
	
		return ADC1->DR;
}

u32 Get_result(u8 ch,u8 times)
{
	u8 i = times;
	u32 adc = 0;
	
	while(i--)
	{
			adc +=Get_Adc(ch);
	}
		adc =adc/times;
		
	return adc;
	//return (float)adc*(3.3/4096);

}



