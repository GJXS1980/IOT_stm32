#ifndef _MAX7219_H_
#define _MAX7219_H_
#include "stm32f10x.h"

/*********************** 定义MAX7219 内部寄存器 **********************/
#define REG_DECODE        0x09            // 译码方式寄存器
#define REG_INTENSITY     0x0a            // 亮度寄存器
#define REG_SCAN_LIMIT    0x0b            // 扫描界限寄存器
#define REG_SHUTDOWN      0x0c            // 停机寄存器
#define REG_DISPLAY_TEST  0x0f            // 显示测试寄存器
#define INTENSITY_MIN     0x00            // 最小显示亮度
#define INTENSITY_MAX     0x0f            // 最大显示亮度
/*********************** 定义MAX7219 端口信号 ************************/

#define	LOAD_high			GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define	LOAD_low	 		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define	CLK_high		 	GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define	CLK_low				GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define	DATA_high	    GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define	DATA_low			GPIO_ResetBits(GPIOB,GPIO_Pin_5)

/************************** 函数原型说明 *****************************/
void Write (unsigned char, unsigned char);
void SendByte (unsigned char);
void Clear (void);
void InitMAX7219(void) ;
void MAX7219_INIT_GPIO(void);

#endif

