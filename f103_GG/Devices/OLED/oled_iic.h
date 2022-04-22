#ifndef _OLED_IIC_H
#define _OLED_IIC_H

#include "gpio.h"

void OLED_IIC_CMD(uint8_t cmd);
void OLED_IIC_DATA(uint8_t dat);

#endif
