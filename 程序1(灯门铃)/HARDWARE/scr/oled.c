#include "oled.h"
#include "delay.h"
#include "oledfont.h"
#include "iic.h"

uint8_t OLED_Diagrams[8][128];

//OLED_Update_Diagrams
//OLED更新图表
void OLED_Update_Diagrams(void)
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8页for循环
	{
		OLED_IIC_WriteCmd(0xb0+page);	//8页地址由0~7
		OLED_IIC_WriteCmd(0x00);	//设置8位起始列地址的低四位
		OLED_IIC_WriteCmd(0x10);	//设置8位起始列地址的高四位
		for(row = 0; row < 128; row++)	//128列for循环
			OLED_IIC_WriteDat(OLED_Diagrams[page][row]);	//更新OLED图表
	}
}

//OLED_Init
//OLED初始化
void OLED_Init(void)
{
	Delay_Us(200);					//延时
	OLED_IIC_WriteCmd(0xAE);	//关闭显示
	OLED_IIC_WriteCmd(0x20);	//设置内存寻址模式命令字
	OLED_IIC_WriteCmd(0x00);	//水平寻址方式
	OLED_IIC_WriteCmd(0xC8);	//设置COM输出扫描方向,重新映射模式。从COM[N-1]扫描到COM0,其中N是多路复用比。
	OLED_IIC_WriteCmd(0x40);	//设置显示起始行
	OLED_IIC_WriteCmd(0x81);	//对比度设置命令字
	OLED_IIC_WriteCmd(0xFF);	//对比度设置值(默认最大)
	OLED_IIC_WriteCmd(0xA1);	//设置段重映射,列地址127映射到SEG0
	OLED_IIC_WriteCmd(0xA6);	//设置正常显示
	OLED_IIC_WriteCmd(0xA8);	//设置多路复用比命令字
	OLED_IIC_WriteCmd(0x3F);	//将多路复用比设置为N+1 MUX	N=111111b,即63d，64MUX
	OLED_IIC_WriteCmd(0xA4);	//恢复到RAM内容显示,输出遵循RAM内容
	OLED_IIC_WriteCmd(0xD3);	//设置显示偏移命令字
	OLED_IIC_WriteCmd(0x00);	//偏移为0
	OLED_IIC_WriteCmd(0xD5);	//设置显示分时比/振荡器频率命令字
	OLED_IIC_WriteCmd(0xF0);	//振荡器频率为最大1111b,分频为0000b,不分频
	OLED_IIC_WriteCmd(0xD9);	//设置预充电周期命令字
	OLED_IIC_WriteCmd(0xF1);	//[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_IIC_WriteCmd(0xDA);	//设置COM引脚硬件配置命令字
	OLED_IIC_WriteCmd(0x12);	//启用COM左/右重新映射,顺序COM引脚配置
	OLED_IIC_WriteCmd(0x8D);	//电荷泵设置命令字
	OLED_IIC_WriteCmd(0x14);	//电荷泵设置值-开启电荷泵
	OLED_IIC_WriteCmd(0xAF);	//开启显示
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_IIC_WriteCmd(0xb0+y);
	OLED_IIC_WriteCmd(((x&0xf0)>>4)|0x10);
	OLED_IIC_WriteCmd((x&0x0f)); 
}  

//OLED_Display_On
//OLED开启显示
void OLED_Display_On(void)
{
	OLED_IIC_WriteCmd(0x8D);	//电荷泵设置命令字
	OLED_IIC_WriteCmd(0x14);	//电荷泵设置值-开启电荷泵
	OLED_IIC_WriteCmd(0xAF);	//开启显示
}

//OLED_Display_Off
//OLED关闭显示
void OLED_Display_Off(void)
{
	OLED_IIC_WriteCmd(0x8D);	//电荷泵设置命令字
	OLED_IIC_WriteCmd(0x10);	//电荷泵设置值-关闭电荷泵
	OLED_IIC_WriteCmd(0xAE);	//关闭显示
}

//OLED_SetContrast
//OLED对比度调节
//contrast:对比度设置_1~256
void OLED_SetContrast(uint8_t contrast)
{
	OLED_IIC_WriteCmd(0x81);	//对比度设置命令字
	OLED_IIC_WriteCmd(contrast);//对比度设置值	
}

//oled_DrawPoint
//OLED画点
//x:横轴坐标_0~127
//y:纵轴坐标_0~63
//mode:1->填充		0->清空
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
	uint8_t page, temp, line;
	if(x>127 || y>63)	return;	//超出范围检测
	page = y/8;	//得到设置页数
	temp = y%8;	//得到设置页数内第几行
	line = 1<<temp;	//转化为十六进制
	mode ? (OLED_Diagrams[page][x] |= line) : (OLED_Diagrams[page][x] &= ~line);	////mode为1，写1，反之写0
}

//OLED_GlobalFill
//OLED全域填充
//mode:1->全部点亮	0->全部熄灭
void OLED_GlobalFill(uint8_t mode)
{
	uint8_t page, row;
	for(page = 0; page < 8; page++)	//8页for循环
		for(row = 0; row < 128; row++)	//128列for循环
			mode ? (OLED_Diagrams[page][row]=0xFF) : (OLED_Diagrams[page][row]=0x00);	//mode为1，写1，反之写0
	OLED_Update_Diagrams();	//OLED更新图表
}

//OLED_AreaFill
//OLED区域填充
//x1,y1:第一点坐标
//x2,y2:第二点坐标
//mode:1->填充		0->清空
//note:x1<=x2, y1<=y2
void OLED_AreaFill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
	uint8_t x, y;
	for(x = x1; x <= x2; x++)	//x范围
		for(y = y1; y <= y2; y++)	//y范围
			OLED_DrawPoint(x, y, mode);	//画点
	OLED_Update_Diagrams();	//OLED更新图表
}

//OLED_ShowChar
//OLED显示字符
//x,y:x,y坐标
//size:选择字体 6*8(8)/6*12(12)/8*16(16)/12*24(24)
//mode:1->正常显示	0->反向显示
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
	uint8_t temp,t,t1;
	uint8_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	if(size == 8)	csize = 6;	//如果是6*8字体，直接强制转换
	chr=chr-' ';//得到偏移后的值		 
	for(t=0;t<csize;t++)
	{   
		if(size==8)temp=asc2_0806[chr][t]; 	 	//调用0806字体
		else if(size==12)temp=asc2_1206[chr][t]; 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
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
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

//OLED_ShowNum
//OLED显示n个数字
//x,y :起点坐标	 
//len :数字的位数
//size:选择字体 6*8(8)/6*12(12)/8*16(16)/12*24(24)
//num:数值(0~4294967295);	 		  
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
//OLED显示字符串
//x,y:起点坐标  
//size:选择字体 6*8(8)/6*12(12)/8*16(16)/12*24(24) 
//*p:字符串起始地址 
void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size)
{	
   uint8_t size0;
	if(size == 8)	size0 = 12;
	else size0 = size;
	while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
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
	
	OLED_Update_Diagrams();		//更新图表
}

/******Private******/
//OLED_TRUTH_LOGO
//OLED显示TRUTH_LOGO
//mode:1->填充		0->清空
void OLED_TRUTH_LOGO(uint8_t mode)
{
	uint8_t x, y, t1, temp, j;
	mode ? OLED_GlobalFill(0) : OLED_GlobalFill(1);
	for(y = 18; y <= 46; y++)	//y范围
		for(x = 8,j = 8; x <= 119;x = x+8,j++)	//x范围
		{
			temp = LOGO[y-18][j-8];
			for(t1=0;t1<8;t1++)
			{
				temp&0x80 ? OLED_DrawPoint(x+t1,y,mode) : OLED_DrawPoint(x+t1,y,!mode);
				temp<<=1;
			}
		}
		
	OLED_Update_Diagrams();	//OLED更新图表
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_IIC_WriteCmd (0xb0+i);    //设置页地址（0~7）
		OLED_IIC_WriteCmd (0x00);      //设置显示位置—列低地址
		OLED_IIC_WriteCmd (0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_IIC_WriteDat(0); 
	} //更新显示
}


////OLED_ShowBMP
////OLED显示BMP图片
////x1,x1:横轴坐标0~127
////x2,y2:页码数_0~7
////BMP:显示数组
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

//显示汉字
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
功能：显示中文字符串
参数：x0,y0原点坐标
	  startnum第一个字在字库表中的编号
	  endnum最后一个字在字库表中的编号
	  fontsize 字体大小
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
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
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
