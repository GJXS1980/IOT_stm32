#include "max7219.h"
#include "delay.h"

void MAX7219_INIT_GPIO(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;                     
		          
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);                                
	
            		                            
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6  ;  
	
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                            
		GPIO_Init(GPIOB, &GPIO_InitStructure);     
			
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_Init(GPIOA,&GPIO_InitStructure);

	
}	


/************************************************************************
*  ����ԭ�ͣ�void Init()
*  ��    ��: ��MAX7219���г�ʼ���������κ���������֮ǰ�����ȵ��ô˺���
*************************************************************************/
void InitMAX7219(void) 
{
  	DATA_high;
		LOAD_high;
		CLK_high;
  	Write(REG_SCAN_LIMIT, 7);          	// ɨ��8λ����
  	Write(REG_DECODE, 0);           		// ���ò����뷽ʽ
  	Write(REG_SHUTDOWN,1);                    // ����ͣ����ʽ����������
  	Write(REG_DISPLAY_TEST, 0);                // ������ʾ���ԣ���������
		Write(REG_INTENSITY, 3);        // ��������
  	Clear();                           // ����ȫ����ʾ��
}
/*************************************************************************
*  ����ԭ�ͣ�void Clear()
*  ��    �ܣ��ر�ȫ����ʾ��
**************************************************************************/
void Clear (void) 
{
  	unsigned char i;
  	for (i=1;i<=8;i++)
    Write(i, 0);                          // �ر�ȫ����ʾ��
}
/*************************************************************************
*  ����ԭ�ͣ�static void Write (unsigned char reg_add, unsigned char dat)
*  ��    �ܣ���ָ����MAX7219�ڲ��Ĵ���д������
*  ��    ��: reg_add = MAX7219�ڲ��Ĵ�����ַ
           dat = ��д�������
**************************************************************************/
void Write (unsigned char reg_add, unsigned char dat) 
{
  	LOAD_low;                               // ʹLOAD��ͣ������������ݷ���
  	SendByte(reg_add);                 		// ����MAX7219�ڲ��Ĵ�����ַ
	if(reg_add>8)
		SendByte(dat);                    		// ���ʹ�д�������
	else
		SendByte((dat<<7)|(dat>>1));		//�緢�͵�����ʾ���ݣ������Ӳ��ת��һ������λ
  	LOAD_high;                              // ʹLOAD��ߣ��������ݽ���
}
/*************************************************************************
*  ����ԭ�ͣ�static void SendByte (unsigned char dat)
*  ��    �ܣ���MAX7219����һ���ֽڴ�������
*  ��    ����dat = �����͵�����
**************************************************************************/
void SendByte (unsigned char dat) 
{
  	unsigned char i;
  	for (i=0; i<8; i++) 
  	{    
    	CLK_low;                                   // ʹCLK���
    	if(dat&0x80)                      // ����һλ����"1"��"0"
      		DATA_high;                                       
    	else                                             
      		DATA_low;   
		dat<<=1;
      	CLK_high;                                    // ʹCLK���
			
			Delay_Us(5);
	  }
}


