#include "oled.h"
#include "oled_font.h"
#include <stdarg.h>
#include <stdio.h>

#define	Max_Column		128
#define	Max_Row			8
#define	X_WIDTH			128
#define	Y_WIDTH			8

#define OLED_CMD        0
#define OLED_DATA       1

static void OLED_WriteByte(uint8_t dat,uint8_t cmd)
{
	if(cmd)
		OLED_IIC_DATA(dat);
	else
		OLED_IIC_CMD(dat);
}

static void OLED_SetPos(uint8_t x, uint8_t y) 
{ 
	OLED_WriteByte(0xB0 + y, OLED_CMD);
	OLED_WriteByte(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
	OLED_WriteByte((x & 0x0F) | 0x01, OLED_CMD); 
}   


void OLED_DisplayOn(void)
{
    OLED_WriteByte(0x8D, OLED_CMD);  
	OLED_WriteByte(0x14, OLED_CMD);  
    OLED_WriteByte(0xAF, OLED_CMD);  
}

void OLED_DisplayOff(void)
{
    OLED_WriteByte(0x8D, OLED_CMD); 
	OLED_WriteByte(0x10, OLED_CMD); 
	OLED_WriteByte(0xAE, OLED_CMD); 
}

void OLED_Clear(void)
{
    uint8_t i,j;
	for(i = 0; i < 8; i ++)  
	{  
		OLED_WriteByte(0xB0 + i, OLED_CMD);  
		OLED_WriteByte(0x02, OLED_CMD);      
		OLED_WriteByte(0x10, OLED_CMD);      
		for(j = 0; j < Max_Column; j ++)
		{
			OLED_WriteByte(0, OLED_DATA); 
		}
	} 
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{      	
	uint8_t c = 0, i = 0;	
	c = chr - ' ';	
	if(x > Max_Column - 1)
	{
		x = 0;
		y += 1;
	}
	OLED_SetPos(x, y);
    for(i = 0; i < 6; i ++)
    {
        OLED_WriteByte(F6x8[c][i],OLED_DATA);
    }        
} 

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{		
		OLED_ShowChar(x, y, chr[j]);
		x += 6;
		if(x > 120)
		{
			x = 0; 
			y += 1;
		}
		j++;
	}
}

void OLED_ShowFloat(uint8_t x, uint8_t y, float data, uint8_t num)
{
    uint8_t StrTemp[20] = {0};
    uint8_t Site = 0;
    int bit = 0;
    int32_t datatemp = 0;

    while(Site < num)
    {
        data *= 10;
        Site++;
    }
    datatemp = (int32_t)data;

    Site = 0;
    if(datatemp < 0)
    {
        StrTemp[Site] = '-';
        datatemp = -datatemp;
        Site++;
    }
    else if(datatemp == 0)
    {
        StrTemp[Site] = '0';
        Site++;
    }

    bit = 0;
    int32_t d = datatemp;
    while(d > 0)
    {
        d /= 10;
        bit++;                   
    }

    uint8_t dotsite = 0; 
    if(bit <= num)
    {
        StrTemp[Site] = '0';
        Site++;
        dotsite = 0;
        bit = num;
    }
    else
    {
        dotsite = bit - num;
    }

    StrTemp[Site + bit + 1] = '\0';
    while(bit >= 0)
    {
        if(bit == dotsite)
        {
            StrTemp[Site + bit] = '.';
        }
        else
        {
            StrTemp[Site + bit] = datatemp % 10 + '0';
            datatemp /= 10;
        }
        bit--;
    }
    OLED_ShowString(x, y, StrTemp);
}

void OLED_ShowBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1, const uint8_t BMP[])
{ 	
	uint32_t j = 0;
	uint8_t x, y;
  
	if(y1 % 8 == 0)
	{
		y = y1 / 8;
	}		      
	else 
	{
		y = y1 / 8 + 1;
	}
    for(y = y0; y < y1; y ++)
    {
        OLED_SetPos(x0, y);
        for(x = x0; x < x1; x ++)
        {      
        
            OLED_WriteByte(BMP[j ++], OLED_DATA);	    	
        }
    }
} 

void OLED_printf(uint8_t x, uint8_t y, const char *pFormat, ...) 
{
    uint8_t pStr[25] = {'\0'};
    va_list ap;
    va_start(ap, pFormat);
    vsprintf((char *)pStr, pFormat, ap);
    va_end(ap);
    OLED_ShowString(x, y, pStr);
}


void OLED_Init(void)
{ 	 	 
	OLED_WriteByte(0xAE, OLED_CMD);//--turn off oled panel
	
	OLED_WriteByte(0x40, OLED_CMD);//---set low column address
	OLED_WriteByte(0xB0, OLED_CMD);//---set high column address
	
	OLED_WriteByte(0xC8, OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	
	OLED_WriteByte(0x81, OLED_CMD);//--set contrast control register
	OLED_WriteByte(0xFF, OLED_CMD);//--Set SEG Output Current Brightness
	
	OLED_WriteByte(0xA1, OLED_CMD);//--Set SEG/Column Mapping     0xa0×óÓÒ·´ÖÃ 0xa1Õý³£

	OLED_WriteByte(0xA6, OLED_CMD);//--set normal display

	OLED_WriteByte(0xA8, OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WriteByte(0x1f, OLED_CMD);//--1/64 duty

	OLED_WriteByte(0xD3, OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteByte(0x00, OLED_CMD);//-not offset

	OLED_WriteByte(0xD5, OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WriteByte(0xF0, OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	
	OLED_WriteByte(0xD9, OLED_CMD);//--set pre-charge period
	OLED_WriteByte(0x22, OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	
	OLED_WriteByte(0xDA, OLED_CMD);//--set com pins hardware configuration
	OLED_WriteByte(0x02, OLED_CMD);
	
	OLED_WriteByte(0xDB, OLED_CMD);//--set vcomh
	OLED_WriteByte(0x49, OLED_CMD);//Set VCOM Deselect Level
	
	OLED_WriteByte(0x8D, OLED_CMD);//--set Charge Pump enable/disable
	OLED_WriteByte(0x14, OLED_CMD);//--set(0x10) disable	

	OLED_WriteByte(0xAF, OLED_CMD); 
		
	OLED_Clear();
	OLED_SetPos(0, 0);  
    
} 


/************************ (C) COPYRIGHT CSU_RM_FYT *************END OF FILE****/
