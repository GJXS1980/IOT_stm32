#include "oled.h"
#include "delay.h"
#include "oledfont.h"
#include "iic.h"

uint8_t OLED_Diagrams[8][128];

//OLED_Update_Diagrams
//OLED����ͼ��
void OLED_Update_Diagrams(void)
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8ҳforѭ��
	{
		OLED_IIC_WriteCmd(0xb0+page);	//8ҳ��ַ��0~7
		OLED_IIC_WriteCmd(0x00);	//����8λ��ʼ�е�ַ�ĵ���λ
		OLED_IIC_WriteCmd(0x10);	//����8λ��ʼ�е�ַ�ĸ���λ
		for(row = 0; row < 128; row++)	//128��forѭ��
			OLED_IIC_WriteDat(OLED_Diagrams[page][row]);	//����OLEDͼ��
	}
}

//OLED_Init
//OLED��ʼ��
void OLED_Init(void)
{
	Delay_Us(200);					//��ʱ
	OLED_IIC_WriteCmd(0xAE);	//�ر���ʾ
	OLED_IIC_WriteCmd(0x20);	//�����ڴ�Ѱַģʽ������
	OLED_IIC_WriteCmd(0x00);	//ˮƽѰַ��ʽ
	OLED_IIC_WriteCmd(0xC8);	//����COM���ɨ�跽��,����ӳ��ģʽ����COM[N-1]ɨ�赽COM0,����N�Ƕ�·���ñȡ�
	OLED_IIC_WriteCmd(0x40);	//������ʾ��ʼ��
	OLED_IIC_WriteCmd(0x81);	//�Աȶ�����������
	OLED_IIC_WriteCmd(0xFF);	//�Աȶ�����ֵ(Ĭ�����)
	OLED_IIC_WriteCmd(0xA1);	//���ö���ӳ��,�е�ַ127ӳ�䵽SEG0
	OLED_IIC_WriteCmd(0xA6);	//����������ʾ
	OLED_IIC_WriteCmd(0xA8);	//���ö�·���ñ�������
	OLED_IIC_WriteCmd(0x3F);	//����·���ñ�����ΪN+1 MUX	N=111111b,��63d��64MUX
	OLED_IIC_WriteCmd(0xA4);	//�ָ���RAM������ʾ,�����ѭRAM����
	OLED_IIC_WriteCmd(0xD3);	//������ʾƫ��������
	OLED_IIC_WriteCmd(0x00);	//ƫ��Ϊ0
	OLED_IIC_WriteCmd(0xD5);	//������ʾ��ʱ��/����Ƶ��������
	OLED_IIC_WriteCmd(0xF0);	//����Ƶ��Ϊ���1111b,��ƵΪ0000b,����Ƶ
	OLED_IIC_WriteCmd(0xD9);	//����Ԥ�������������
	OLED_IIC_WriteCmd(0xF1);	//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_IIC_WriteCmd(0xDA);	//����COM����Ӳ������������
	OLED_IIC_WriteCmd(0x12);	//����COM��/������ӳ��,˳��COM��������
	OLED_IIC_WriteCmd(0x8D);	//��ɱ�����������
	OLED_IIC_WriteCmd(0x14);	//��ɱ�����ֵ-������ɱ�
	OLED_IIC_WriteCmd(0xAF);	//������ʾ
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_IIC_WriteCmd(0xb0+y);
	OLED_IIC_WriteCmd(((x&0xf0)>>4)|0x10);
	OLED_IIC_WriteCmd((x&0x0f)); 
}  

//OLED_Display_On
//OLED������ʾ
void OLED_Display_On(void)
{
	OLED_IIC_WriteCmd(0x8D);	//��ɱ�����������
	OLED_IIC_WriteCmd(0x14);	//��ɱ�����ֵ-������ɱ�
	OLED_IIC_WriteCmd(0xAF);	//������ʾ
}

//OLED_Display_Off
//OLED�ر���ʾ
void OLED_Display_Off(void)
{
	OLED_IIC_WriteCmd(0x8D);	//��ɱ�����������
	OLED_IIC_WriteCmd(0x10);	//��ɱ�����ֵ-�رյ�ɱ�
	OLED_IIC_WriteCmd(0xAE);	//�ر���ʾ
}

//OLED_SetContrast
//OLED�Աȶȵ���
//contrast:�Աȶ�����_1~256
void OLED_SetContrast(uint8_t contrast)
{
	OLED_IIC_WriteCmd(0x81);	//�Աȶ�����������
	OLED_IIC_WriteCmd(contrast);//�Աȶ�����ֵ	
}

//oled_DrawPoint
//OLED����
//x:��������_0~127
//y:��������_0~63
//mode:1->���		0->���
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
	uint8_t page, temp, line;
	if(x>127 || y>63)	return;	//������Χ���
	page = y/8;	//�õ�����ҳ��
	temp = y%8;	//�õ�����ҳ���ڵڼ���
	line = 1<<temp;	//ת��Ϊʮ������
	mode ? (OLED_Diagrams[page][x] |= line) : (OLED_Diagrams[page][x] &= ~line);	////modeΪ1��д1����֮д0
}

//OLED_GlobalFill
//OLEDȫ�����
//mode:1->ȫ������	0->ȫ��Ϩ��
void OLED_GlobalFill(uint8_t mode)
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8ҳforѭ��
		for(row = 0; row < 128; row++)	//128��forѭ��
			mode ? (OLED_Diagrams[page][row]=0xFF) : (OLED_Diagrams[page][row]=0x00);	//modeΪ1��д1����֮д0
	OLED_Update_Diagrams();	//OLED����ͼ��
}

//OLED_AreaFill
//OLED�������
//x1,y1:��һ������
//x2,y2:�ڶ�������
//mode:1->���		0->���
//note:x1<=x2, y1<=y2
void OLED_AreaFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
	uint8_t x, y;
	for(x = x1; x <= x2; x++)	//x��Χ
		for(y = y1; y <= y2; y++)	//y��Χ
			OLED_DrawPoint(x, y, mode);	//����
	OLED_Update_Diagrams();	//OLED����ͼ��
}

//OLED_ShowChar
//OLED��ʾ�ַ�
//x,y:x,y����
//size:ѡ������ 6*8(8)/6*12(12)/8*16(16)/12*24(24)
//mode:1->������ʾ	0->������ʾ
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	if(size == 8)	csize = 6;	//�����6*8���壬ֱ��ǿ��ת��
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
	for(t=0;t<csize;t++)
	{   
		if(size==8)temp=asc2_0806[chr][t]; 	 	//����0806����
		else if(size==12)temp=asc2_1206[chr][t]; 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
      for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)	OLED_DrawPoint(x,y,mode);
			else	OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }
}

//mypow
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//OLED_ShowNum
//OLED��ʾn������
//x,y :�������	 
//len :���ֵ�λ��
//size:ѡ������ 6*8(8)/6*12(12)/8*16(16)/12*24(24)
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;	
	if(size == 8)	size = 12;
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

//OLED_ShowString
//OLED��ʾ�ַ���
//x,y:�������  
//size:ѡ������ 6*8(8)/6*12(12)/8*16(16)/12*24(24) 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
   uint8_t size0;
	if(size == 8)	size0 = 12;
	else size0 = size;
	while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
	{       
		if(x>(128-(size/2)))
		{
			x=0;
			y+=size;
		}
		if(y>(64-size))
		{
			y=x=0;
			OLED_GlobalFill(0);
		}
		OLED_ShowChar(x,y,*p,size,1);	 
		x+=size0/2;
		p++;
    }  
	
	OLED_Update_Diagrams();		//����ͼ��
}

/******Private******/
//OLED_TRUTH_LOGO
//OLED��ʾTRUTH_LOGO
//mode:1->���		0->���
void OLED_TRUTH_LOGO(uint8_t mode)
{
	uint8_t x, y, t1, temp, j;
	mode ? OLED_GlobalFill(0) : OLED_GlobalFill(1);
	for(y = 18; y <= 46; y++)	//y��Χ
		for(x = 8,j = 8; x <= 119;x = x+8,j++)	//x��Χ
		{
			temp = LOGO[y-18][j-8];
			for(t1=0;t1<8;t1++)
			{
				temp&0x80 ? OLED_DrawPoint(x+t1,y,mode) : OLED_DrawPoint(x+t1,y,!mode);
				temp<<=1;
			}
		}
		
	OLED_Update_Diagrams();	//OLED����ͼ��
}

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_IIC_WriteCmd (0xb0+i);    //����ҳ��ַ��0~7��
		OLED_IIC_WriteCmd (0x00);      //������ʾλ�á��е͵�ַ
		OLED_IIC_WriteCmd (0x10);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_IIC_WriteDat(0); 
	} //������ʾ
}


////OLED_ShowBMP
////OLED��ʾBMPͼƬ
////x1,x1:��������0~127
////x2,y2:ҳ����_0~7
////BMP:��ʾ����
//void OLED_ShowBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
//{ 	
//	unsigned int j=0;
//	unsigned char x,y;
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//		for(x=x0;x<x1;x++)
//		{      
//			OLED_IIC_WriteDat(BMP[j++]);	    	
//		}
//	}
//} 

//��ʾ����
void OLED_ShowChinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
		OLED_IIC_WriteDat(Hzk[2*no][t]);
		adder+=1;
     }	
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{	
		OLED_IIC_WriteDat(Hzk[2*no+1][t]);
		adder+=1;
    }					
}
/*
���ܣ���ʾ�����ַ���
������x0,y0ԭ������
	  startnum��һ�������ֿ���еı��
	  endnum���һ�������ֿ���еı��
	  fontsize �����С
*/
void OLED_ShowChineseString(u8 x0,u8 y0,u8 startnum,u8 endnum,u8 fontsize)
{
	u8 i = 0;
	u8 j = 0;
	for(i=startnum,j=0;i<=endnum;i++,j++)
	{
		if((x0+j*(fontsize+2))>128||y0>8)
		{
			break;
		}
		OLED_ShowChinese(x0+j*(fontsize+2),y0,i);		
	}
}
/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;
  
	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_IIC_WriteDat(BMP[j++]);	    	
		}
	}
} 
