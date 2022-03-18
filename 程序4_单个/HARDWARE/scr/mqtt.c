/*-------------------------------------------------*/
/*                                                 */
/*            实现MQTT协议功能的源文件             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    
#include "mqtt.h"         
#include "string.h"       
#include "stdio.h"        
#include "usart1.h"       
#include "utils_hmac.h"   
#include "wifi.h"         

unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];           
unsigned char *MQTT_RxDataInPtr;                            
unsigned char *MQTT_RxDataOutPtr;                         
unsigned char *MQTT_RxDataEndPtr;                          

unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];           
unsigned char *MQTT_TxDataInPtr;                            
unsigned char *MQTT_TxDataOutPtr;                           
unsigned char *MQTT_TxDataEndPtr;                           

unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];              
unsigned char *MQTT_CMDInPtr;                              
unsigned char *MQTT_CMDOutPtr;                             
unsigned char *MQTT_CMDEndPtr;                              

char ClientID[128];                                         
int  ClientID_len;                                          

char Username[128];                                        
int  Username_len;											 

char Passward[128];                                       
int  Passward_len;											

char ServerIP[128];                                        
int  ServerPort;                                           

int   Fixed_len;                       					   
int   Variable_len;                     				
int   Payload_len;                       					
unsigned char  temp_buff[TBUFF_UNIT];						

char Ping_flag;           
                         
char Connect_flag;       
char ConnectPack_flag;   
char SubcribePack_flag;  

/*----------------------------------------------------------*/
/*函数名：初始化接收,发送,命令数据的 缓冲区 以及各状态参数  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0];               
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr;             
  MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];        
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];               
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;             
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];     
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                    
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                    
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];             

  MQTT_ConectPack();                              
	MQTT_Subscribe(S_TOPIC_NAME,0);	                
	
	Ping_flag = ConnectPack_flag = SubcribePack_flag = 0; 
}
/*----------------------------------------------------------*/
/*函数名：阿里云初始化参数，得到客户端ID，用户名和密码      */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void AliIoT_Parameter_Init(void)
{	
	char temp[128];                                                      

	memset(ClientID,128,0);                                             
	sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME); 
	ClientID_len = strlen(ClientID);                                      
	
	memset(Username,128,0);                                               
	sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                    
	Username_len = strlen(Username);                                      
	
	memset(temp,128,0);                                                                     
	sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);      
	utils_hmac_sha1(temp,strlen(temp),Passward,DEVICESECRE,DEVICESECRE_LEN);                 
	Passward_len = strlen(Passward);                                                        
	
	memset(ServerIP,128,0);  
	sprintf(ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);               
	ServerPort = 1883;                                                                      
	
	u1_printf("服 务 器：%s:%d\r\n",ServerIP,ServerPort);
	u1_printf("客户端ID：%s\r\n",ClientID);              
	u1_printf("用 户 名：%s\r\n",Username);               
	u1_printf("密    码：%s\r\n",Passward);               
}

/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        
	Variable_len = 10;                                                    
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len;    
	Remaining_len = Variable_len + Payload_len;                           
	
	temp_buff[0]=0x10;                      
	do{                                    
		temp = Remaining_len%128;           
		Remaining_len = Remaining_len/128;  
		if(Remaining_len>0)               	
			temp |= 0x80;                       
		temp_buff[Fixed_len] = temp;         
		Fixed_len++;	                      
	}while(Remaining_len>0);               
	
	temp_buff[Fixed_len+0]=0x00;              
	temp_buff[Fixed_len+1]=0x04;   
	temp_buff[Fixed_len+2]=0x4D;	
	temp_buff[Fixed_len+3]=0x51;	
	temp_buff[Fixed_len+4]=0x54;	
	temp_buff[Fixed_len+5]=0x54;	
	temp_buff[Fixed_len+6]=0x04;	
	temp_buff[Fixed_len+7]=0xC2;
	temp_buff[Fixed_len+8]=0x00; 
	temp_buff[Fixed_len+9]=0x64;	
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  			    
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  			 
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 		
	/*     用户名        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				  		 
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				 		  
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);              
	/*      密码        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;			   
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;			   
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len); 

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);                  
}
/*----------------------------------------------------------*/
/*函数名：SUBSCRIBE订阅topic报文                            */
/*参  数：QoS：订阅等级                                     */
/*参  数：topic_name：订阅topic报文名称                     */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              
	Variable_len = 2;                          
	Payload_len = 2 + strlen(topic_name) + 1;   
	temp_buff[0]=0x82;                                                    
	temp_buff[1]=Variable_len + Payload_len;             
	temp_buff[2]=0x00;                                   
	temp_buff[3]=0x01;		                              
	temp_buff[4]=strlen(topic_name)/256;                 
	temp_buff[5]=strlen(topic_name)%256;		        
	memcpy(&temp_buff[6],topic_name,strlen(topic_name));  		
	temp_buff[6+strlen(topic_name)]=QoS;                 
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len); 
}
/*----------------------------------------------------------*/
/*函数名：PING报文，心跳包                                  */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0]=0xC0;                                 
	temp_buff[1]=0x00;              

	TxDataBuf_Deal(temp_buff, 2);  
}
/*----------------------------------------------------------*/
/*函数名：等级0 发布消息报文                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*参  数：data_len：数据长度                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(char *topic, char *data, int data_len)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                              
	Variable_len = 2 + strlen(topic);          
	Payload_len = data_len;                     
	Remaining_len = Variable_len + Payload_len;
	
	temp_buff[0]=0x30;                      	
	do{                                      
		temp = Remaining_len%128;            
		Remaining_len = Remaining_len/128;  
		if(Remaining_len>0)               	
			temp |= 0x80;                           
		temp_buff[Fixed_len] = temp;        
		Fixed_len++;	                    
	}while(Remaining_len>0);                
		             
	temp_buff[Fixed_len+0]=strlen(topic)/256;                     
	temp_buff[Fixed_len+1]=strlen(topic)%256;		               
	memcpy(&temp_buff[Fixed_len+2],topic,strlen(topic));          
	memcpy(&temp_buff[Fixed_len+2+strlen(topic)],data,data_len);   
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  
}
/*----------------------------------------------------------*/
/*函数名：处理服务器发来的等级0的推送                       */
/*参  数：redata：接收的数据                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           	
	int  pack_num;                         
  int  temp,temp_len;                  
  int  totle_len;                     
	int  topic_len;              	  	     
	int  cmd_len;                        
	int  cmd_loca;                        
	int  i;                              
	int  local,multiplier;
	unsigned char tempbuff[RBUFF_UNIT];	   
	unsigned char *data;                   
	re_len = redata[0]*256+redata[1];                               	
	data = &redata[2];                                             
	pack_num = temp_len = totle_len = temp = 0;                	  
	local = 1;
	multiplier = 1;
	do{
		pack_num++;                                     			
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                          	
		re_len -= (temp_len + local) ;                                    
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}while(re_len!=0);                                  			
	//u1_printf("接收到%d个服务器推送数据:\r\n",pack_num);			
	temp_len = totle_len = 0;                		            	
	local = 1;
	multiplier = 1;
	for(i=0;i<pack_num;i++){                                        
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);				
		topic_len = data[local+totle_len]*256+data[local+1+totle_len] + 2;    
		cmd_len = temp_len-topic_len;                               					
		cmd_loca = totle_len + local +  topic_len;                  					
		memcpy(tempbuff,&data[cmd_loca],cmd_len);                   					                 
		CMDBuf_Deal(tempbuff, cmd_len);                             					
		totle_len += (temp_len+local);                              					
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}	
}
/*----------------------------------------------------------*/
/*函数名：处理发送缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[2],data,size);      	
	MQTT_TxDataInPtr[0] = size/256;              
	MQTT_TxDataInPtr[1] = size%256;              
	MQTT_TxDataInPtr+=TBUFF_UNIT;                 
	if(MQTT_TxDataInPtr==MQTT_TxDataEndPtr)      
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];    
}
/*----------------------------------------------------------*/
/*函数名：处理命令缓冲区                                    */
/*参  数：data：数据                                        */
/*参  数：size：数据长度                                    */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2],data,size);      
	MQTT_CMDInPtr[0] = size/256;              
	MQTT_CMDInPtr[1] = size%256;              
	MQTT_CMDInPtr[size+2] = '\0';             
	MQTT_CMDInPtr+=CBUFF_UNIT;                 
	if(MQTT_CMDInPtr==MQTT_CMDEndPtr)         
		MQTT_CMDInPtr = MQTT_CMDBuf[0];       
}
