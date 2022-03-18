#include "adc.h"

//PA6-6  PB0-8ͨ��
void adc_init(void)
{

		RCC->APB2ENR |= (1<<2 | 1<<3 );	//PA PB
		GPIOA->CRL &= ~(15 << 24);  //ģ������
		GPIOB->CRL &= ~(15<<0);
		
		RCC->APB2ENR |= (1<<9);		
		RCC->APB2RSTR |= (1<<9);		//��λADC1
		RCC->APB2RSTR &= ~(1<<9);
		
		RCC->CFGR &= ~(3<<14);		//ADCPRE[1:0] ��Ƶ����
		RCC->CFGR |= (2<<14);			//10��PCLK2 6��Ƶ����ΪADCʱ�� 72/6 = 12MHz
		
		ADC1->CR1 &= ~(15<<16);		
		ADC1->CR1 &= ~(1<<8);
		
		ADC1->CR2 &= ~(1<<1);
		ADC1->CR2 &= ~(7<<17);
		ADC1->CR2 |= (7<<17);			//�������ת��
		ADC1->CR2 &= ~(1<<11);		//�Ҷ���
		ADC1->CR2 |= (1<<20);
	
		ADC1->SQR1 &= ~(0XF<<20);	//����ת������Ϊһ��ת��
		ADC1->SQR1 |= (1<<20);		//����ת��
		
		ADC1->SMPR2 &= ~(7<<18);	//ͨ��6
		ADC1->SMPR2 |= (7<<18);		//111��239.5����
		
		ADC1->SMPR2 &= ~(7<<24);		//ͨ��8
		ADC1->SMPR2 |= (7<<24);
		
		ADC1->CR2 |= (1<<0 | 1<<3);	//����ת��
		while((ADC1->CR2)&(1<<3));	//Ӳ������
		
		ADC1->CR2 |= (1<<2);		//����У��
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



