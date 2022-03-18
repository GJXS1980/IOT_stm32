#include "stm32f10x.h"  
#include "wifi.h"	    
#include "iic.h"       
#include "24c02.h" 		
#include "delay.h"	    
#include "usart1.h"	    
#include "led.h"        
#include "mqtt.h"       
#include "key.h"        
#include "beep.h"

char wifi_mode = 1;     //联网模式 
	
/*-------------------------------------------------*/
/*函数名：初始化WiFi的复位IO                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void WiFi_ResetIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		  
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		  
	RESET_IO(1);                                              
}
/*-------------------------------------------------*/
/*函数名：WiFi发送设置指令                         */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter=0;                                                   
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      
	WiFi_printf("%s\r\n",cmd);                  
	while(timeout--){                           
		Delay_Ms(100);                          
		if(strstr(WiFi_RX_BUF,"OK"))            
			break;       						
		u1_printf("%d ",timeout);               
	}
	u1_printf("\r\n");                          
	if(timeout<=0)return 1;                     
	else return 0;		         				
}
/*-------------------------------------------------*/
/*函数名：WiFi复位                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    
	Delay_Ms(500);                                  
	RESET_IO(1);
	//Delay_Ms(200); 
	//u1_printf("data:%s\r\n",WiFi_RX_BUF);	
	while(timeout--){  
		Delay_Ms(100);                              
		if(strstr(WiFi_RX_BUF,"ready"))             
			break;       						    
		u1_printf("%d ",timeout);                   
	}
	u1_printf("\r\n");                              
	if(timeout<=0)return 1;                         
	else return 0;		         				   
}
/*-------------------------------------------------*/
/*函数名：WiFi加入路由器指令                       */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                                                       
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); 
	while(timeout--){                               
		Delay_Ms(1000);                             
		//if(strstr(WiFi_RX_BUF,"WIFI GOT IPOK")) 
		if(strstr(WiFi_RX_BUF,"\n\r\nOK")) 
			break;       						    
		u1_printf("%d ",timeout);                   
	}
	u1_printf("\r\n");                              
	if(timeout<=0)return 1;                        
	return 0;                                       
}
/*-------------------------------------------------*/
/*函数名：连接TCP服务器，并进入透传模式            */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter=0;                                                       
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);            
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ServerIP,ServerPort);
	while(timeout--){                               
		Delay_Ms(100);                              	
		if(strstr(WiFi_RX_BUF ,"CONNECT"))          
			break;                                  
		if(strstr(WiFi_RX_BUF ,"CLOSED"))           
			return 1;                               
		if(strstr(WiFi_RX_BUF ,"ALREADY CONNECTED"))
			return 2;                               
		u1_printf("%d ",timeout);                     
	}
	u1_printf("\r\n");                        
	if(timeout<=0)return 3;                   
	else                                      
	{
		u1_printf("连接服务器成功，准备进入透传\r\n");  
		WiFi_RxCounter=0;                                                       
		memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);              
		WiFi_printf("AT+CIPSEND\r\n");                  
		while(timeout--){                               
			Delay_Ms(100);                              	
			if(strstr(WiFi_RX_BUF,"\r\nOK\r\n\r\n>"))   
				break;                          
			u1_printf("%d ",timeout);             
		}
		if(timeout<=0)return 4;                 	
	}
	return 0;	                                	
}
/*-------------------------------------------------*/
/*函数名：WiFi_Smartconfig                         */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{
	
	WiFi_RxCounter=0;                                                  
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          
	while(timeout--){                           
		Delay_Ms(1000);                         
		if(strstr(WiFi_RX_BUF,"connected"))     
			break;                                
		u1_printf("%d ",timeout);

			if(timeout%2)
				BEEP_ON;
			else
				BEEP_OFF;
	}	
	u1_printf("\r\n");                          
	if(timeout<=0)return 1;                     
	return 0;                                   
}
/*-------------------------------------------------*/
/*函数名：等待加入路由器                           */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               
		Delay_Ms(1000);                             
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       
			break;       						    
		u1_printf("%d ",timeout);                   
	}
	u1_printf("\r\n");                              
	if(timeout<=0)return 1;                         
	return 0;                                       
}
/*-------------------------------------------------*/
/*函数名：WiFi连接服务器                           */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	
	u1_printf("准备复位模块\r\n");                     
	if(WiFi_Reset(50))
	{                                
		u1_printf("复位失败，准备重启\r\n");           
		return 1;                                      
	}else u1_printf("复位成功\r\n");                   
	
	u1_printf("准备设置STA模式\r\n");                  
	if(WiFi_SendCmd("AT+CWMODE=1",50))
	{                
		u1_printf("设置STA模式失败，准备重启\r\n");    
		return 2;                                      
	}else u1_printf("设置STA模式成功\r\n");            
	
	if(wifi_mode==0)
	{                                       
		u1_printf("准备取消自动连接\r\n");                 
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",50))
		{            
			u1_printf("取消自动连接失败，准备重启\r\n");   
			return 3;                                      
		}else u1_printf("取消自动连接成功\r\n");           
				
		u1_printf("准备连接路由器\r\n");                   	
		if(WiFi_JoinAP(30))
		{                               
			u1_printf("连接路由器失败，准备重启\r\n");     
			return 4;                                      	
		}else u1_printf("连接路由器成功\r\n");             			
	}
	else
	{                                                 //如果联网模式=1：Smartconfig方式,用APP发送
		if(KEY2_IN_STA==0)
		{                                    
			u1_printf("准备设置自动连接\r\n");                 
			if(WiFi_SendCmd("AT+CWAUTOCONN=1",50))
		 {            
				u1_printf("设置自动连接失败，准备重启\r\n");   
				return 3;                                      
			}
			else 
				u1_printf("设置自动连接成功\r\n");           	
			
			u1_printf("准备开启Smartconfig\r\n");              
			if(WiFi_SendCmd("AT+CWSTARTSMART=3",50))
			{            
				u1_printf("开启Smartconfig失败，准备重启\r\n");
				return 4;                                      
			}else u1_printf("开启Smartconfig成功\r\n");        

			u1_printf("请使用APP软件传输密码\r\n");            
			if(WiFi_Smartconfig(60))
			{                          
				u1_printf("传输密码失败，准备重启\r\n");       
				return 5;                                      
			}else u1_printf("传输密码成功\r\n");               
			BEEP_OFF;									
			u1_printf("准备关闭Smartconfig\r\n");              
			if(WiFi_SendCmd("AT+CWSTOPSMART",50))
		  {             
				u1_printf("关闭Smartconfig失败，准备重启\r\n");
				return 6;                                      
			}else u1_printf("关闭Smartconfig成功\r\n");        
		}else
		{                                                 
			u1_printf("等待连接路由器\r\n");                   	
			if(WiFi_WaitAP(30)){                               
				u1_printf("连接路由器失败，准备重启\r\n");     
				return 7;                                      	
			}else u1_printf("连接路由器成功\r\n");             					
		}
	}
	
	u1_printf("准备设置透传\r\n");                    
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){              
		u1_printf("设置透传失败，准备重启\r\n");       
		return 8;                                      
	}else u1_printf("设置透传成功\r\n");               
	
	u1_printf("准备关闭多路连接\r\n");                 
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){               
		u1_printf("关闭多路连接失败，准备重启\r\n");   
		return 9;                                      
	}else u1_printf("关闭多路连接成功\r\n");           
	
	u1_printf("准备连接服务器\r\n");                   
	if(WiFi_Connect_Server(100)){                      
		u1_printf("连接服务器失败，准备重启\r\n");     
		return 10;                                     
	}else u1_printf("连接服务器成功\r\n");             	
	
	return 0;                                          
	
}
