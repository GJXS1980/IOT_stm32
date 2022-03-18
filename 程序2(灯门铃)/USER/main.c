#include "stm32f10x.h" 
#include "main.h"      
#include "delay.h"     
#include "usart1.h"    
#include "usart2.h"    
#include "timer1.h"    
#include "timer3.h"    
#include "timer4.h"    
#include "wifi.h"	    	
#include "led.h"       
#include "mqtt.h"      
#include "key.h"       
#include <stdlib.h>
#include "cJSON.h"
#include "IOT.h"  
#include "pwm.h"
#include "beep.h"
#include "iot_enable.h"
#include "max7219.h"

uint8_t PWM={0};
uint8_t   old_data;
uint8_t   new_data;

void light_update(int state);

typedef enum IOT_sensor
{
		broadcast = 0,
		door_2,
		lighting_2,
		EXIT,
	
}IOT_sensor_t;

int  cJSON_analysis(char *data,char* target)
{

	cJSON *root =NULL;
	cJSON *item =NULL;
	
	u8 result;
	
	root=cJSON_Parse(data);
	if(!root) 
	{
    u1_printf("Error before: [%s]\n",cJSON_GetErrorPtr());
		return 0;
	}
	
	item=cJSON_GetObjectItem(root,"params");
	item=cJSON_GetObjectItem(item,target);	
	result = 	item->valueint;
	cJSON_Delete(root);
	
	return result;
}	

/*-------------------------------------------------*/
/*函数名：定时将数据发送往服务器        					  */
/*1.window 																				*/
/*2.lighting 																			*/
/*3.door 																					*/
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void IOT_State_Update(void)
{
	char *send = NULL;                   
	
	cJSON *root = cJSON_CreateObject();
	//int state = GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4);

#if Lighting_IOT_2	
	
	int state = GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1);
	
	cJSON_AddItemToObject(root,"lighting2",cJSON_CreateNumber(state));
	cJSON_AddItemToObject(root,"brightness2",cJSON_CreateNumber(PWM));

#endif 
	
		

#if Door_IOT_2	

	int state = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
	cJSON_AddItemToObject(root,"door2",cJSON_CreateNumber(state));
	
#endif 
	
	send=cJSON_PrintUnformatted(root);
	if(send != NULL)
	{
		u1_printf("发往服务器信息:%s\r\n",send);
		MQTT_PublishQs0(P_TOPIC_NAME,send,strlen(send));
	
	}
	
		cJSON_Delete(root);	
		cJSON_free(send);
}

void IOT_CONTORL(char *data)
{
	cJSON *root = NULL;
	cJSON *Item = NULL;
	u8  i = 0;
	
	root = cJSON_Parse(data);
	
	if(!root)
	{
	    u1_printf("Error before: [%s]\n",cJSON_GetErrorPtr());
			return;
	}	
	
	for(i=0;i<EXIT;i++)
	{
		
			switch(i)
			{
							 
#if Lighting_IOT_2				
				case lighting_2:
						 Item = cJSON_GetObjectItem(root,"lighting_2");
						 if(Item != NULL)
						 {
								Lighting_State(Item->valueint);
								light_update(Item->valueint);
								i = EXIT;
						 }
					break;			
#endif 
							 

#if Door_IOT_2							 
				case door_2:
						 Item = cJSON_GetObjectItem(root,"door_2");
						 if(Item != NULL)
						 {
								Door_State(Item->valueint);
									
								i = EXIT;
						 }
#endif							 
					case	broadcast:
						Item = cJSON_GetObjectItem(root,"broadcast");		//广播 所有模块上传状态信息
						{
								if(Item != NULL)
								{
										IOT_State_Update();  
										i = EXIT;
								}
						}
					
					break;
				default :
					break;	
			}
	
	}
	
		cJSON_Delete(root);
	
}


int main(void) 
{	
	Delay_Init();                   
	Usart1_Init(115200);   
	Usart2_Init(115200);                      
	TIM1_Init(500,7200);
	tim4_pwm_init(20000-1,72-1);	

#if Door_IOT_2
	TIM2_INIT(200-1,36000 -1);		
#endif
	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
	AliIoT_Parameter_Init();	   	  //初始化连接阿里云IoT平台MQTT服务器的参数	
	beep_init();

#if Door_IOT_2
	key_init();
#else
	IOT_Init();
#endif

	while(1)        
	{	
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1)
		{     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){              
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					//u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);  
					MQTT_TxData(MQTT_TxDataOutPtr);                       
					MQTT_TxDataOutPtr += TBUFF_UNIT;                     
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)            
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];           
				} 				
			}
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){ 					
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                         
								    ConnectPack_flag = 1;                                        
									break;                                                       
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");    
									Connect_flag = 0;                                            
									break;                                                       
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n");
									Connect_flag = 0;                                            
									break;                                                       
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");        
									Connect_flag = 0;                                            
									break;                                                       
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");  
									Connect_flag = 0;                                            		
									break;                                                       
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");             
									Connect_flag = 0;                                            		
									break;                                                       
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");            
									Connect_flag = 0;                                            	
									break;                                                       
					}				
				}			
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("订阅成功\r\n");      
							        SubcribePack_flag = 1;           
											Ping_flag = 0;                       
											TIM3_ENABLE_30S();                                   
 						
									break;                                                     
						default   : u1_printf("订阅失败，准备重启\r\n"); 
									Connect_flag = 0;                     
									break;                                							
					}					
				}
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING报文回复\r\n"); 
					if(Ping_flag==1){             
						 Ping_flag = 0;    				  
					}else if(Ping_flag>1){ 				
						Ping_flag = 0;     				  
						TIM3_ENABLE_30S(); 				  
					}				
				}			
				else if((MQTT_RxDataOutPtr[2]==0x30))	//服务器等级0推送
				{ 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
								
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];                       
			}
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr)
			{  
				u1_printf("来自服务器信息:%s\r\n",&MQTT_CMDOutPtr[2]);
				
#if  Lighting_IOT_2					
				if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"LightingSwitch\":1}"))
				{ 

					Lighting_State(1);
						
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"LightingSwitch\":0}"))
				{ 

					Lighting_State(0);
						
				}
#endif					
	
#if  Door_IOT_2				
			if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"DoorSwitch\":1}"))
				{ 
					
					Door_State(1);  
					
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"DoorSwitch\":0}"))
				{ 

					Door_State(0);  
				
				}
				
#endif				
				
				else		//以上为手机APP端控制信息，以下来自来自PC的广播控制信息
				{
							IOT_CONTORL((char*)&MQTT_CMDOutPtr[2]);	//解析JSON文件
				}
					
					MQTT_CMDOutPtr += CBUFF_UNIT;                             	//指针下移
					if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //指针归位到缓冲区开头			
		  }
			
			
#if 	Door_IOT_2		
				if(scan_key()==1)			//指触发一次
				{
						Door_State(1);
				}
				
				if(new_data != old_data)
				{
				
						Door_State(new_data);
				}	
#endif 
		}
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("需要连接服务器\r\n");                
			TIM_Cmd(TIM1,DISABLE);                         
			TIM_Cmd(TIM3,DISABLE);                         
			WiFi_RxCounter=0;                               
			memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);        
			if(WiFi_Connect_IoTServer()==0)
			{   			     
				u1_printf("建立TCP连接成功\r\n");            
				Connect_flag = 1;                            
				WiFi_RxCounter=0;                            
				memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);      
				MQTT_Buff_Init();    
				BEEP_ON;
				Delay_Ms(500);
				BEEP_OFF;
				
				}	
			
			}
	}
}

void Door_State(int state)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
	
	if(state>0)
	{	
		
		char *send = NULL;
		int state = 1 ;
		cJSON *root = cJSON_CreateObject();
		
		BEEP_ON;
		TIM_Cmd(TIM2,ENABLE); 
		
		cJSON_AddItemToObject(root,"door2",cJSON_CreateNumber(state));
		send=cJSON_PrintUnformatted(root);
		
		if(send != NULL)
		{
			u1_printf("发往服务器信息:%s\r\n",send);
			MQTT_PublishQs0(P_TOPIC_NAME,send,strlen(send));
		
		}
		
		cJSON_Delete(root);	
		cJSON_free(send);
		
	}
	else
	{	
		BEEP_OFF;
	}
	
	old_data = state;
	new_data = old_data;
	if(new_data) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302311\",\"params\":{\"DoorSwitch\":1},\"version\":\"1.0.0\"}");  
	else         sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302311\",\"params\":{\"DoorSwitch\":0},\"version\":\"1.0.0\"}");  
	
	MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
	
}

void Lighting_State(int state)
{

	state>0?PA1_ON:PA1_OFF;
	light_update(state);  	 //添加数据，发布给服务器		
}


void light_update(int state)
{
	char temp[TBUFF_UNIT];                   
	
	if(state) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302320\",\"params\":{\"LightingSwitch\":1},\"version\":\"1.0.0\"}");  
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302320\",\"params\":{\"LightingSwitch\":0},\"version\":\"1.0.0\"}");  
	MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));  

}





