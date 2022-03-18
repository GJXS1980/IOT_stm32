/*-------------------------------------------------*/
/*                                                 */
/*              操作Wifi功能的头文件               */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	    //包含需要的头文件

#define RESET_IO(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)x)  

#define WiFi_printf       u2_printf           
#define WiFi_RxCounter    Usart2_RxCounter    
#define WiFi_RX_BUF       Usart2_RxBuff       
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  

#define SSID   "HG310"                     		
#define PASS   "HG31063575."                

void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


#endif


