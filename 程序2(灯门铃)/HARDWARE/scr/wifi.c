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

char wifi_mode = 1;     //����ģʽ 0��SSID������д�ڳ�����   1��Smartconfig��ʽ��APP����
	
/*-------------------------------------------------*/
/*����������ʼ��WiFi�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
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
/*��������WiFi��������ָ��                         */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
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
	if(timeout<=0)return 1;                     //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         				
}
/*-------------------------------------------------*/
/*��������WiFi��λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
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
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter=0;                                                       
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);          
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,PASS); 
	while(timeout--){                               
		Delay_Ms(1000);                             
		//if(strstr(WiFi_RX_BUF,"WIFI GOT IPOK")) //������յ�WIFI GOT IP��ʾ�ɹ�
		if(strstr(WiFi_RX_BUF,"\n\r\nOK")) //������յ�WIFI GOT IP��ʾ�ɹ�����������ԭ�������ʽ��		WIFI GOT IP\n\r\nOK���ϰ汾�����ʽ��WIFI GOT IP\r\n\r\nOK��Ϊ��ͨ�ã��˴���������
			break;       						    //��������whileѭ��
		u1_printf("%d ",timeout);                   
	}
	u1_printf("\r\n");                              
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       
}
/*-------------------------------------------------*/
/*������������TCP��������������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
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
		if(strstr(WiFi_RX_BUF ,"CLOSED"))           //������ܵ�CLOSED��ʾ������δ����
			return 1;                               //������δ��������1
		if(strstr(WiFi_RX_BUF ,"ALREADY CONNECTED"))//������ܵ�ALREADY CONNECTED�Ѿ���������
			return 2;                               //�Ѿ��������ӷ���2
		u1_printf("%d ",timeout);                   //����������ڵĳ�ʱʱ��  
	}
	u1_printf("\r\n");                        
	if(timeout<=0)return 3;                   
	else                                      
	{
		u1_printf("���ӷ������ɹ���׼������͸��\r\n");  
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
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
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
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               
		Delay_Ms(1000);                             
		if(strstr(WiFi_RX_BUF,"WIFI GOT IP"))       //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						    
		u1_printf("%d ",timeout);                   
	}
	u1_printf("\r\n");                              
	if(timeout<=0)return 1;                         //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       
}
/*-------------------------------------------------*/
/*��������WiFi���ӷ�����                           */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	
	u1_printf("׼����λģ��\r\n");                     
	if(WiFi_Reset(50))
	{                                
		u1_printf("��λʧ�ܣ�׼������\r\n");           
		return 1;                                      
	}else u1_printf("��λ�ɹ�\r\n");                   
	
	u1_printf("׼������STAģʽ\r\n");                  
	if(WiFi_SendCmd("AT+CWMODE=1",50))
	{                //����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
		u1_printf("����STAģʽʧ�ܣ�׼������\r\n");    
		return 2;                                      
	}else u1_printf("����STAģʽ�ɹ�\r\n");            
	
	if(wifi_mode==0)
	{                                      //�������ģʽ=0��SSID������д�ڳ����� 
		u1_printf("׼��ȡ���Զ�����\r\n");                 
		if(WiFi_SendCmd("AT+CWAUTOCONN=0",50))
		{            
			u1_printf("ȡ���Զ�����ʧ�ܣ�׼������\r\n");   
			return 3;                                      
		}else u1_printf("ȡ���Զ����ӳɹ�\r\n");           
				
		u1_printf("׼������·����\r\n");                   	
		if(WiFi_JoinAP(30))
		{                               //����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
			u1_printf("����·����ʧ�ܣ�׼������\r\n");     
			return 4;                                      	
		}else u1_printf("����·�����ɹ�\r\n");             			
	}
	else
	{                                                 //�������ģʽ=1��Smartconfig��ʽ,��APP����
		if(KEY2_IN_STA==0)
		{                                    
			u1_printf("׼�������Զ�����\r\n");                 
			if(WiFi_SendCmd("AT+CWAUTOCONN=1",50))
		 {            //�����Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u1_printf("�����Զ�����ʧ�ܣ�׼������\r\n");   
				return 3;                                      
			}
			else 
				u1_printf("�����Զ����ӳɹ�\r\n");           	
			
			u1_printf("׼������Smartconfig\r\n");              
			if(WiFi_SendCmd("AT+CWSTARTSMART=3",50))
			{            //����Smartconfig��100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u1_printf("����Smartconfigʧ�ܣ�׼������\r\n");
				return 4;                                      
			}else u1_printf("����Smartconfig�ɹ�\r\n");        

			u1_printf("��ʹ��APP�����������\r\n");            
			if(WiFi_Smartconfig(60))
			{                          //APP����������룬1s��ʱ��λ���ܼ�60s��ʱʱ��
				u1_printf("��������ʧ�ܣ�׼������\r\n");       
				return 5;                                      
			}else u1_printf("��������ɹ�\r\n");               
			BEEP_OFF;									//�رշ�������ʾ����
			u1_printf("׼���ر�Smartconfig\r\n");              
			if(WiFi_SendCmd("AT+CWSTOPSMART",50))
		  {             //�ر�Smartconfig��100ms��ʱ��λ���ܼ�5s��ʱʱ��
				u1_printf("�ر�Smartconfigʧ�ܣ�׼������\r\n");
				return 6;                                      
			}else u1_printf("�ر�Smartconfig�ɹ�\r\n");        
		}else
		{                                                 //��֮����ʱK2��û�а���
			u1_printf("�ȴ�����·����\r\n");                   //������ʾ����	
			if(WiFi_WaitAP(30)){                               //�ȴ�����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
				u1_printf("����·����ʧ�ܣ�׼������\r\n");     
				return 7;                                      	
			}else u1_printf("����·�����ɹ�\r\n");             					
		}
	}
	
	u1_printf("׼������͸��\r\n");                    
	if(WiFi_SendCmd("AT+CIPMODE=1",50)){              
		u1_printf("����͸��ʧ�ܣ�׼������\r\n");       
		return 8;                                      
	}else u1_printf("����͸���ɹ�\r\n");               
	
	u1_printf("׼���رն�·����\r\n");                 
	if(WiFi_SendCmd("AT+CIPMUX=0",50)){               
		u1_printf("�رն�·����ʧ�ܣ�׼������\r\n");   
		return 9;                                      
	}else u1_printf("�رն�·���ӳɹ�\r\n");           
	
	u1_printf("׼�����ӷ�����\r\n");                   
	if(WiFi_Connect_Server(100)){                      
		u1_printf("���ӷ�����ʧ�ܣ�׼������\r\n");     
		return 10;                                     
	}else u1_printf("���ӷ������ɹ�\r\n");             	
	
	return 0;                                          
	
}
