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
*  函数原型：void Init()
*  功    能: 对MAX7219进行初始化，调用任何其他函数之前必需先调用此函数
*************************************************************************/
void InitMAX7219(void) 
{
  	DATA_high;
		LOAD_high;
		CLK_high;
  	Write(REG_SCAN_LIMIT, 7);          	// 扫描8位数字
  	Write(REG_DECODE, 0);           		// 设置不译码方式
  	Write(REG_SHUTDOWN,1);                    // 结束停机方式，正常工作
  	Write(REG_DISPLAY_TEST, 0);                // 结束显示测试，正常工作
		Write(REG_INTENSITY, 3);        // 设置亮度
  	Clear();                           // 清零全部显示器
}
/*************************************************************************
*  函数原型：void Clear()
*  功    能：关闭全部显示器
**************************************************************************/
void Clear (void) 
{
  	unsigned char i;
  	for (i=1;i<=8;i++)
    Write(i, 0);                          // 关闭全部显示器
}
/*************************************************************************
*  函数原型：static void Write (unsigned char reg_add, unsigned char dat)
*  功    能：向指定的MAX7219内部寄存器写入数据
*  参    数: reg_add = MAX7219内部寄存器地址
           dat = 待写入的数据
**************************************************************************/
void Write (unsigned char reg_add, unsigned char dat) 
{
  	LOAD_low;                               // 使LOAD变低，启动串行数据发送
  	SendByte(reg_add);                 		// 发送MAX7219内部寄存器地址
	if(reg_add>8)
		SendByte(dat);                    		// 发送待写入的数据
	else
		SendByte((dat<<7)|(dat>>1));		//如发送的是显示数据，则根据硬件转换一下数据位
  	LOAD_high;                              // 使LOAD变高，串行数据结束
}
/*************************************************************************
*  函数原型：static void SendByte (unsigned char dat)
*  功    能：向MAX7219发送一个字节串行数据
*  参    数：dat = 待发送的数据
**************************************************************************/
void SendByte (unsigned char dat) 
{
  	unsigned char i;
  	for (i=0; i<8; i++) 
  	{    
    	CLK_low;                                   // 使CLK变低
    	if(dat&0x80)                      // 发送一位数据"1"或"0"
      		DATA_high;                                       
    	else                                             
      		DATA_low;   
		dat<<=1;
      	CLK_high;                                    // 使CLK变高
			
			Delay_Us(5);
	  }
}


