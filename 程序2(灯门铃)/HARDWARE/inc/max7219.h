#ifndef _MAX7219_H_
#define _MAX7219_H_
#include "stm32f10x.h"

/*********************** ����MAX7219 �ڲ��Ĵ��� **********************/
#define REG_DECODE        0x09            // ���뷽ʽ�Ĵ���
#define REG_INTENSITY     0x0a            // ���ȼĴ���
#define REG_SCAN_LIMIT    0x0b            // ɨ����޼Ĵ���
#define REG_SHUTDOWN      0x0c            // ͣ���Ĵ���
#define REG_DISPLAY_TEST  0x0f            // ��ʾ���ԼĴ���
#define INTENSITY_MIN     0x00            // ��С��ʾ����
#define INTENSITY_MAX     0x0f            // �����ʾ����
/*********************** ����MAX7219 �˿��ź� ************************/

#define	LOAD_high			GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define	LOAD_low	 		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define	CLK_high		 	GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define	CLK_low				GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define	DATA_high	    GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define	DATA_low			GPIO_ResetBits(GPIOB,GPIO_Pin_5)

/************************** ����ԭ��˵�� *****************************/
void Write (unsigned char, unsigned char);
void SendByte (unsigned char);
void Clear (void);
void InitMAX7219(void) ;
void MAX7219_INIT_GPIO(void);

#endif

