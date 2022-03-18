#ifndef _OLED_H_
#define _OLED_H_
#include "stm32f10x.h"

void OLED_Init(void);	//OLED��ʼ��
void OLED_Display_On(void);	//OLED������ʾ
void OLED_Display_Off(void);	//OLED�ر���ʾ
void OLED_SetContrast(uint8_t contrast);	//OLED�Աȶȵ���
void OLED_Update_Diagrams(void);	//OLED����ͼ��
void OLED_GlobalFill(uint8_t mode);//OLEDȫ�����
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);	//OLED����
void OLED_AreaFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);	//OLED�������
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);	//OLED��ʾ�ַ�
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);	//OLED��ʾn������
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);	//OLED��ʾ�ַ���
void OLED_TRUTH_LOGO(uint8_t mode);	//OLED��ʾTRUTH_LOGO
void OLED_ShowChinese(u8 x,u8 y,u8 no);
void OLED_ShowChineseString(u8 x0,u8 y0,u8 startnum,u8 endnum,u8 fontsize);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Clear(void);


//void OLED_ShowBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);	//OLED��ʾBMPͼƬ
#endif
