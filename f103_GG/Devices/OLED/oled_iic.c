#include "oled_iic.h"

#define OLED_ADDRESS 0x78

#define OLED_SCL_RESET() HAL_GPIO_WritePin(GPIOB,OLED_SCL_Pin,GPIO_PIN_RESET)
#define OLED_SCL_SET() HAL_GPIO_WritePin(GPIOB,OLED_SCL_Pin,GPIO_PIN_SET)

#define OLED_SDA_RESET() HAL_GPIO_WritePin(GPIOB,OLED_SDA_Pin,GPIO_PIN_RESET)
#define OLED_SDA_SET() HAL_GPIO_WritePin(GPIOB,OLED_SDA_Pin,GPIO_PIN_SET)

static void OLED_IIC_Start(void)
{
	OLED_SDA_SET();
	OLED_SCL_SET();
	OLED_SDA_RESET();
	OLED_SCL_RESET();
}

static void OLED_IIC_Stop(void)
{
	OLED_SCL_RESET();
	OLED_SDA_RESET();
	OLED_SCL_SET();
	OLED_SDA_SET();
}

static void OLED_IIC_WaitAck(void)
{
	OLED_SCL_SET();
	OLED_SCL_RESET();
}

static void OLED_IIC_WriteByte(uint8_t byte)
{
	uint8_t i;
	OLED_SCL_RESET();
	for(i = 0; i < 8; i++)
	{
		if(byte & 0x80)
			OLED_SDA_SET();
		else
			OLED_SDA_RESET();
		byte<<=1; 	  
		OLED_SCL_SET(); 
		OLED_SCL_RESET();	
	}
}

void OLED_IIC_CMD(uint8_t cmd)
{
	OLED_IIC_Start();
	OLED_IIC_WriteByte(OLED_ADDRESS);
	OLED_IIC_WaitAck();
	OLED_IIC_WriteByte(0x00);
	OLED_IIC_WaitAck();
	OLED_IIC_WriteByte(cmd);
	OLED_IIC_WaitAck();
	OLED_IIC_Stop();
}

void OLED_IIC_DATA(uint8_t dat)
{
	OLED_IIC_Start();
	OLED_IIC_WriteByte(OLED_ADDRESS);
	OLED_IIC_WaitAck();
	OLED_IIC_WriteByte(0x40);
	OLED_IIC_WaitAck();
	OLED_IIC_WriteByte(dat);
	OLED_IIC_WaitAck();
	OLED_IIC_Stop();
}

