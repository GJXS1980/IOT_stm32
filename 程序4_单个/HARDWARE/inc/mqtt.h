/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�ͷ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __MQTT_H
#define __MQTT_H
#include "iot_enable.h"

#define  R_NUM               5     //���ջ���������
#define  RBUFF_UNIT          300   //���ջ���������

#define  T_NUM               5     //���ͻ���������  
#define  TBUFF_UNIT          300   //���ͻ���������

#define  C_NUM               8     //�����������
#define  CBUFF_UNIT          300   //�����������

#define  MQTT_TxData(x)       u2_TxData(x)                                         //����2�������ݷ���

#if	Trashcan_IOT 	//	����

#define  PRODUCTKEY           "a1sJCAg60fL"                                        //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //��ƷID����
#define  DEVICENAME           "HG-Curtain"                                               //�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //�豸������
#define  DEVICESECRE          "b1ae8490110b75939068dba85ce67570"                   //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //�豸��Կ����
#define  S_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"   												//���ܹ㲥��Ϣ  
#define  P_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"    											//���͹㲥
#define  APP_TOPIC_NAME       "/sys/a1sJCAg60fL/HG-Curtain/thing/event/property/post"    //��Ҫ���������� 	

#endif   


#if Lighting_IOT	//	��

#define  PRODUCTKEY           "a1sJCAg60fL"                                        //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //��ƷID����
#define  DEVICENAME           "20220317001"                                               //�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //�豸������
#define  DEVICESECRE          "69eb966064526aa826f2889ef0f850d5"                   //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //�豸��Կ����
#define  S_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"   												//���ܹ㲥��Ϣ  
#define  P_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"    											//���͹㲥
#define  APP_TOPIC_NAME       "/sys/a1sJCAg60fL/20220317001/thing/event/property/post"    //��Ҫ���������� 	

#endif

#if Door_IOT	//	����

#define  PRODUCTKEY           "a1sJCAg60fL"                                        //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //��ƷID����
#define  DEVICENAME           "HG-door"                                            //�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //�豸������
#define  DEVICESECRE          "a547a4f3d4049af09ca936e9e8c12e0a"                   //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //�豸��Կ����
#define  S_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"   												//���ܹ㲥��Ϣ  
#define  P_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"    											//���͹㲥
#define  APP_TOPIC_NAME       "/sys/a1sJCAg60fL/HG-door/thing/event/property/post"    //��Ҫ���������� 	

#endif

#if Gateway_IOT			//	բ��

#define  PRODUCTKEY           "a1sJCAg60fL"                                        //��ƷID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                                   //��ƷID����
#define  DEVICENAME           "HG-Gateway"                                            //�豸��  
#define  DEVICENAME_LEN       strlen(DEVICENAME)                                   //�豸������
#define  DEVICESECRE          "7decbe7b9d80f27ab8130046caf52faa"                   //�豸��Կ   
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                                  //�豸��Կ����
#define  S_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"   												//���ܹ㲥��Ϣ  
#define  P_TOPIC_NAME         "/broadcast/a1sJCAg60fL/IOTmsg"    											//���͹㲥
#define  APP_TOPIC_NAME       "/sys/a1sJCAg60fL/HG-Gateway/thing/event/property/post"    //��Ҫ���������� 	


#endif 

extern unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];       //�ⲿ�������������ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_RxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_RxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_RxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];       //�ⲿ�������������ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
extern unsigned char *MQTT_TxDataInPtr;                        //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_TxDataOutPtr;                       //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_TxDataEndPtr;                       //�ⲿ����������ָ�򻺳���������λ��
extern unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];          //�ⲿ�����������������ݵĽ��ջ�����
extern unsigned char *MQTT_CMDInPtr;                           //�ⲿ����������ָ�򻺳���������ݵ�λ��
extern unsigned char *MQTT_CMDOutPtr;                          //�ⲿ����������ָ�򻺳�����ȡ���ݵ�λ��
extern unsigned char *MQTT_CMDEndPtr;                          //�ⲿ����������ָ�򻺳���������λ��

extern char ClientID[128];     //�ⲿ������������ſͻ���ID�Ļ�����
extern int  ClientID_len;      //�ⲿ������������ſͻ���ID�ĳ���
extern char Username[128];     //�ⲿ��������������û����Ļ�����
extern int  Username_len;	   //�ⲿ��������������û����ĳ���
extern char Passward[128];     //�ⲿ�����������������Ļ�����
extern int  Passward_len;	   //�ⲿ�����������������ĳ���
extern char ServerIP[128];     //�ⲿ������������ŷ�����IP��������
extern int  ServerPort;        //�ⲿ������������ŷ������Ķ˿ں�

extern char Ping_flag;         //�ⲿ����������ping����״̬      0������״̬���ȴ���ʱʱ�䵽������Ping����
                               //�ⲿ����������ping����״̬      1��Ping�����ѷ��ͣ����յ� �������ظ����ĵĺ� ��1��Ϊ0
extern char Connect_flag;      //�ⲿ����������ͬ����������״̬  0����û�����ӷ�����  1�������Ϸ�������
extern char ReConnect_flag;    //�ⲿ��������������������״̬    0�����ӻ�����  1�����ӶϿ�������
extern char ConnectPack_flag;  //�ⲿ����������CONNECT����״̬   1��CONNECT���ĳɹ�
extern char SubcribePack_flag; //�ⲿ�������������ı���״̬      1�����ı��ĳɹ�

void MQTT_Buff_Init(void);
void AliIoT_Parameter_Init(void);
void MQTT_ConectPack(void);
void MQTT_Subscribe(char *, int);
void MQTT_PingREQ(void);
void MQTT_PublishQs0(char *, char *, int);
void MQTT_DealPushdata_Qs0(unsigned char *);	
void TxDataBuf_Deal(unsigned char *, int);
void CMDBuf_Deal(unsigned char *, int);

#endif
