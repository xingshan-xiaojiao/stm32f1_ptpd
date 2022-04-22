#ifndef _OLED_H
#define _OLED_H

#include "oled_iic.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr);
void OLED_ShowBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1, const uint8_t BMP[]);
void OLED_ShowFloat(uint8_t x, uint8_t y, float data, uint8_t num);
void OLED_printf(uint8_t x, uint8_t y, const char *pFormat, ...);

#endif
  
