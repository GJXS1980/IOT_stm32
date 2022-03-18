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
u8 trashcan_state = 0;
u8 target_floor = 1;						//当前目标电梯
int target_index = 1;  					//当前索引
u8 PWM_STATUS = 5;

void light_update(int state);
void trashcan_update(int state);
void Gateway_State(int state);
void gateway_update(int state);
void Trashcan_stop(void);
void Trashcan_reset(void);

typedef enum IOT_sensor
{
		broadcast = 0,
		trashcan,
		door,
		lighting_3,
		trashcan_bar,
		lighting_bar,
		gateway,
		elevator,
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

#if Trashcan_IOT	
	
	cJSON_AddItemToObject(root,"trashcan_p",cJSON_CreateNumber(trashcan_state));
	cJSON_AddItemToObject(root,"pull_p",cJSON_CreateNumber(PWM));
	
#endif 
	
#if Lighting_IOT	
	
	int state = GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1);
	
	cJSON_AddItemToObject(root,"lighting3",cJSON_CreateNumber(state));
	cJSON_AddItemToObject(root,"brightness3",cJSON_CreateNumber(PWM));

#endif 
	
	
#if Gateway_IOT	
	
	int state = !GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4);
	
	cJSON_AddItemToObject(root,"gateway_p",cJSON_CreateNumber(state));
	cJSON_AddItemToObject(root,"target_floor_p",cJSON_CreateNumber(target_index));

#endif 	

#if Door_IOT	

	int state = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9);
	cJSON_AddItemToObject(root,"door_p",cJSON_CreateNumber(state));
	
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

#if Trashcan_IOT				
				case trashcan:
							 Item = cJSON_GetObjectItem(root,"trashcan");
							 if(Item != NULL)
							 { 
									Trashcan_State(Item->valueint);
							 }
							 Item = cJSON_GetObjectItem(root,"stop");
							 if(Item != NULL)
							 { 
									Trashcan_stop();
							 }
							 Item = cJSON_GetObjectItem(root,"reset");
							 if(Item != NULL)
							 { 
									Trashcan_reset();
							 }	
							i = EXIT;							 
					break;	
							 
#endif
							 
#if Lighting_IOT				
				case lighting_3:
						 Item = cJSON_GetObjectItem(root,"lighting_3");
						 if(Item != NULL)
						 {
								Lighting_State(Item->valueint);
								light_update(Item->valueint);
								i = EXIT;
						 }
					break;			
#endif 
		
#if Gateway_IOT				
				case gateway:
						 Item = cJSON_GetObjectItem(root,"gateway");
						 if(Item != NULL)
						 {
								Gateway_State(Item->valueint);
								gateway_update(Item->valueint);
								i = EXIT;
						 }
					
				case elevator:
							Item = cJSON_GetObjectItem(root,"target_floor");
						 if(Item != NULL)
						 {
								target_floor = Item->valueint;
								i = EXIT;
						 }
				
					break;			
#endif 						 

#if Door_IOT							 
				case door:
						 Item = cJSON_GetObjectItem(root,"door");
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

#if Gateway_IOT	

unsigned char  storey[]={
	0x00,0x00,0x0C,0x04,0x04,0x04,0x04,0x1E,0x00,/*"1",0*/
	0x00,0x3F,0x01,0x3F,0x20,0x20,0x3f,0x00,/*"2",1*/
	0x00,0x3F,0x01,0x1E,0x01,0x01,0x3F,0x00,/*"3",2*/
	0x00,0x24,0x24,0x24,0x3F,0x04,0x04,0x00,/*"4",3*/
	0x00,0x3E,0x20,0x3F,0x01,0x01,0x3F,0x00,/*"5",4*/
};

#endif

int main(void) 
{	
#if Gateway_IOT		
	int i,j;
#endif
	Delay_Init();                   
	Usart1_Init(115200);   
	
#if Gateway_IOT		
	MAX7219_INIT_GPIO();
	InitMAX7219();
	for(j=1;j<9;j++)
	{
			Write(j,storey[j]<<1);	
	}
	
	Delay_Ms(50);
	
#endif
		
	Usart2_Init(115200);                      
	TIM1_Init(500,7200);
	tim4_pwm_init(20000-1,72-1);	

#if Trashcan_IOT	
	TIM2_INIT(30000-1,36000 -1);		//15s
	
#endif

#if Door_IOT
	Delay_Ms(100);
	TIM2_INIT(200-1,36000 -1);		//	
#endif
	
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
	AliIoT_Parameter_Init();	    //初始化连接阿里云IoT平台MQTT服务器的参数	
	beep_init();

#if Door_IOT
	key_init();
#else
	IOT_Init();
#endif

	while(1)        
	{
#if 	Trashcan_IOT	
		
		if(scan_key() && PWM_STATUS > 0 )
		{
			PWM_STOP;
			TIM2->CNT = 0;
			PWM_STATUS--;
			TIM_Cmd(TIM2,DISABLE); 
			TIM_Cmd(TIM4,DISABLE);
			
		}
		else 
		{	
				PWM_STATUS = 5;
		}
#endif		
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
	//									IOT_State_Update(trashcan);    	//发布广播                     
 						
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
				
#if  Trashcan_IOT					
				if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"CurtainsSwitch\":1}"))
				{	                                                 
					Trashcan_State(1);
	
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"CurtainsSwitch\":0}"))
				{ 				
					Trashcan_State(0);
						
				}
				else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"Curtains_Reset\":1}"))
				{
						 Trashcan_reset();					
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"Curtains_Stop\":1}"))
				{
						Trashcan_stop();		
					
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"CurtainsControl"))
				{   
						
							PWM = cJSON_analysis((char*)&MQTT_CMDOutPtr[2],"CurtainsControl"); 
						
						  if(PWM<5)
							{
									PWM_CLOSE;
									while(PWM<5)
									{
										Delay_Ms(1000);
										PWM++;	
										u1_printf("开： %d S\r\n",PWM);
									}	
								
							}else if(PWM>5)
							{
								PWM_OPEN;			
								while(PWM>5)
								{
									  u1_printf("关：%d S\r\n",PWM);
										Delay_Ms(1000);
										PWM--;	
								}
							}
							PWM_STOP;
							u1_printf("PWM[0]=%d\r\n",PWM);
  							
				}
#endif		

#if  Lighting_IOT					
				if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"LightingSwitch\":1}"))
				{ 

					Lighting_State(1);
						
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"LightingSwitch\":0}"))
				{ 

					Lighting_State(0);
						
				}
#endif					
	
#if  Door_IOT				
			if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch\":1}"))
				{ 
					
					Door_State(1);  
					
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch\":0}"))
				{ 

					Door_State(0);  
				
				}
				
#endif				


#if Gateway_IOT

			if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"Gateway_SW\":1}"))
				{ 
					
					Gateway_State(1);  
					
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"Gateway_SW\":0}"))
				{ 

					Gateway_State(0);  
				
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
#if Gateway_IOT	
			if(target_floor > target_index)
			{
					for(i=(target_index-1)*8;i<sizeof(storey)-8;i++)  //上
					{
							for(j=1;j<9;j++)
							{
								Write(j,storey[j+i]<<1);	
							}
							
							Delay_Ms(200);
							
							if(i%8==0)
							{		
									target_index++;			
									Delay_Ms(1000);
									if(target_index > target_floor)
									{
											target_index = target_floor;
											//target_floor = 5; 到底目标点 开门
											break;
									}	
										
							}
					}
			
			}	
			
			 if(target_floor < target_index)   //下
			{
						i= 8*(target_index-1);
						for(;i>-1;i--)
						{
								for(j=1;j<9;j++)
								{
									Write(j,storey[j+i]<<1);	
								}
								
								Delay_Ms(200);
								
								if(i%8==0)
								{	
										Delay_Ms(1000);
										target_index--;
										
										if(target_index < target_floor)
										{
											target_index=target_floor;
											//target_floor = 5; 到底目标点 开门
											break;
										}	
											
								}
						}
			}	
					
#endif

#if 	Door_IOT		
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
				
#if Trashcan_IOT	
				PWM_CLOSE;
#endif
			}						
		}
	}
}
/*-------------------------------------------------*/
/*函数名：判断开关1状态，并发布给服务器            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Trashcan_State(int state)
{
	
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
	
	
	if(state>0 && (trashcan_state==0))
	{
		TIM_Cmd(TIM4,ENABLE);
		PWM_OPEN;
		u1_printf("open\r\n");
		trashcan_state = 1;
		
	}
	else if((state==0) && trashcan_state)
	{	
		TIM_Cmd(TIM4,ENABLE);
		PWM_CLOSE;
		u1_printf("close\r\n");
		trashcan_state = 0;
	} else return;
	
//	u1_printf("CNT1=%d\r\n",TIM2->CNT);
//	TIM2->CNT = (30000 - 1) - TIM2->CNT ;	
//	u1_printf("CNT2=%d\r\n",TIM2->CNT);
		TIM2->CNT = 0;
		TIM_Cmd(TIM2,ENABLE);                                         //开TIM2  
	
	if(state) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"CurtainsSwitch\":1},\"version\":\"1.0.0\"}");  
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"CurtainsSwitch\":0},\"version\":\"1.0.0\"}");  
	MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}

void Trashcan_stop(void)
{
		char temp[TBUFF_UNIT]; 
		TIM_Cmd(TIM4,ENABLE);

		PWM_STOP;
		TIM2->CNT = 0;
		TIM_Cmd(TIM2,DISABLE);  
	
    sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"Curtains_Stop\":0},\"version\":\"1.0.0\"}");  
	  MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}	

void Trashcan_reset(void)
{
	 char temp[TBUFF_UNIT]; 
	  PWM_STATUS = 5;
		TIM_Cmd(TIM4,ENABLE);
		PWM_CLOSE;
	
	  sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"Curtains_Reset\":0},\"version\":\"1.0.0\"}");  
	  MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
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
		
		cJSON_AddItemToObject(root,"door_p",cJSON_CreateNumber(state));
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
	char temp[TBUFF_UNIT];   //定义一个临时缓冲区
	
	if(state) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302320\",\"params\":{\"LightingSwitch\":1},\"version\":\"1.0.0\"}");  
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302320\",\"params\":{\"LightingSwitch\":0},\"version\":\"1.0.0\"}");  
	MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));  

}

void Gateway_State(int state)
{

	state>0?PA4_OFF:PA4_ON;
	gateway_update(state);	 	                  //添加数据，发布给服务器
}

void gateway_update(int state)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
	
	if(state) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302370\",\"params\":{\"Gateway_SW\":1},\"version\":\"1.0.0\"}");  
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302370\",\"params\":{\"Gateway_SW\":0},\"version\":\"1.0.0\"}");  
	MQTT_PublishQs0(APP_TOPIC_NAME,temp,strlen(temp));
}



