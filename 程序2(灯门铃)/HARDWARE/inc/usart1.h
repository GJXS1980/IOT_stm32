#ifndef __USART1_H
#define __USART1_H

#include "stdio.h"      
#include "stdarg.h"		
#include "string.h"    

#define USART1_RX_ENABLE     0      //是否开启接收功能  1：开启  0：关闭
#define USART1_TXBUFF_SIZE   256    //定义串口1 发送缓冲区大小 256字节

#if  USART1_RX_ENABLE                          //如果使能接收功能
#define USART1_RXBUFF_SIZE   256               //定义串口1 接收缓冲区大小 256字节
extern char Usart1_RxCompleted ;               //外部声明，其他文件可以调用该变量
extern unsigned int Usart1_RxCounter;          //外部声明，其他文件可以调用该变量
extern char Usart1_RxBuff[USART1_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量
#endif

void Usart1_Init(unsigned int);     
void u1_printf(char*,...) ;         

#endif


