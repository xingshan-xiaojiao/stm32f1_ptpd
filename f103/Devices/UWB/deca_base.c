#include "deca_base.h"
#include <string.h>

#define BUFFLEN 128

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn writetospi()
 *
 * @brief
 * NB: In porting this to a particular microprocessor, the implementer needs to define the two low
 * level abstract functions to write to and read from the SPI the definitions should be in deca_spi.c file.
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 *
 * Note: The body of this function is defined in deca_spi.c and is platform specific
 *
 * input parameters:
 * @param headerLength  - number of bytes header being written
 * @param headerBuffer  - pointer to buffer containing the 'headerLength' bytes of header to be written
 * @param bodylength    - number of bytes data being written
 * @param bodyBuffer    - pointer to buffer containing the 'bodylength' bytes od data to be written
 *
 * output parameters
 *
 * returns DWT_SUCCESS for success, or DWT_ERROR for error
 */
int writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t bodyLength, const uint8_t *bodyBuffer)
{
	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY){};

    HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_RESET); /**< Put chip select line low */

    HAL_SPI_Transmit(&hspi1, (uint8_t *)&headerBuffer[0], headerLength, 10);	/* Send header in polling mode */
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&bodyBuffer[0], bodyLength, 10);		/* Send data in polling mode */

    HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_SET); /**< Put chip select line high */

	return 0;
}
/*! ------------------------------------------------------------------------------------------------------------------
 * @fn readfromspi()
 *
 * @brief
 * NB: In porting this to a particular microprocessor, the implementer needs to define the two low
 * level abstract functions to write to and read from the SPI the definitions should be in deca_spi.c file.
 * Low level abstract function to write to the SPI
 * Takes two separate byte buffers for write header and write data
 * returns 0 for success, or -1 for error
 *
 * Note: The body of this function is defined in deca_spi.c and is platform specific
 *
 * input parameters:
 * @param headerLength  - number of bytes header to write
 * @param headerBuffer  - pointer to buffer containing the 'headerLength' bytes of header to write
 * @param readlength    - number of bytes data being read
 * @param readBuffer    - pointer to buffer containing to return the data (NB: size required = headerLength + readlength)
 *
 * output parameters
 *
 * returns DWT_SUCCESS for success (and the position in the buffer at which data begins), or DWT_ERROR for error
 */
int readfromspi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t readlength, uint8_t *readBuffer)
{
	uint8_t spi_TmpBuffer[BUFFLEN];
	assert_param(headerLength+readlength < BUFFLEN );
	
	/* Blocking: Check whether previous transfer has been finished */
	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);

	HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_RESET); /**< Put chip select line low */

	HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)headerBuffer, spi_TmpBuffer, (uint16_t)(headerLength+readlength), 10);

	HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_SET); /**< Put chip select line high */

	memcpy((uint8_t*)readBuffer , (uint8_t*)&spi_TmpBuffer[headerLength], readlength);
	
	return 0;
}
/*! ------------------------------------------------------------------------------------------------------------------
 * @fn decamutexon()
 *
 * @brief This function should disable interrupts. This is called at the start of a critical section
 * It returns the IRQ state before disable, this value is used to re-enable in decamutexoff call
 *
 * Note: The body of this function is defined in deca_mutex.c and is platform specific
 *
 * input parameters:
 *
 * output parameters
 *
 * returns the state of the DW1000 interrupt
 */
decaIrqStatus_t decamutexon(void) 
{
	decaIrqStatus_t s = EXTI_GetITEnStatus(EXTI9_5_IRQn);
	if(s){
		EXTI9_5_DisableIRQ();
	}
	return s;
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn decamutexoff()
 *
 * @brief This function should re-enable interrupts, or at least restore their state as returned(&saved) by decamutexon
 * This is called at the end of a critical section
 *
 * Note: The body of this function is defined in deca_mutex.c and is platform specific
 *
 * input parameters:
 * @param s - the state of the DW1000 interrupt as returned by decamutexon
 *
 * output parameters
 *
 * returns the state of the DW1000 interrupt
 */
void decamutexoff(decaIrqStatus_t s)
{
	if(s){
		EXTI9_5_EnableIRQ();
	}
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn deca_sleep()
 *
 * @brief Wait for a given amount of time.
 * NB: The body of this function is defined in deca_sleep.c and is platform specific
 *
 * input parameters:
 * @param time_ms - time to wait in milliseconds
 *
 * output parameters
 *
 * no return value
 */
void deca_sleep(unsigned int time_ms)
{
	HAL_Delay(time_ms);
}

void deca_usleep(unsigned int time_ms)
{
	TIM3_Delayus(time_ms);
}

void setup_DW1000RSTnIRQ(int enable)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if(enable)
	{
		// Enable GPIO used as DECA RESET for interrupt
		GPIO_InitStruct.Pin = DW_RESET_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(DW_RESET_GPIO_Port, &GPIO_InitStruct);

		HAL_NVIC_EnableIRQ(EXTI0_IRQn);		//pin #0 -> EXTI #0
		HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	}
	else
	{
		HAL_NVIC_DisableIRQ(EXTI0_IRQn);	//pin #0 -> EXTI #0

		//put the pin back to tri-state ... as 
		//output open-drain (not active)
		GPIO_InitStruct.Pin = DW_RESET_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DW_RESET_GPIO_Port, &GPIO_InitStruct);
		HAL_GPIO_WritePin(DW_RESET_GPIO_Port, DW_RESET_Pin, GPIO_PIN_SET);
	}
}

void reset_DW1000(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	// Enable GPIO used for DW1000 reset as open collector output
	GPIO_InitStruct.Pin = DW_RESET_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DW_RESET_GPIO_Port, &GPIO_InitStruct);

	//drive the RSTn pin low
	HAL_GPIO_WritePin(DW_RESET_GPIO_Port, DW_RESET_Pin, GPIO_PIN_RESET);
	
	deca_usleep(1);
	//put the pin back to output open-drain (not active)
	setup_DW1000RSTnIRQ(0);

	deca_sleep(2);
}

void wakeup_dw1000(void)
{
	HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_RESET);
    deca_sleep(1);
	HAL_GPIO_WritePin(DW_CS_GPIO_Port, DW_CS_Pin, GPIO_PIN_SET);
    deca_sleep(7);						//wait 7ms for DW1000 XTAL to stabilise
}

void set_dw1000_slowrate(void)
{
	__HAL_SPI_DISABLE(&hspi1);
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	HAL_SPI_Init(&hspi1);
	__HAL_SPI_ENABLE(&hspi1);
}

void set_dw1000_fastrate(void)
{
	__HAL_SPI_DISABLE(&hspi1);
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	HAL_SPI_Init(&hspi1);
	__HAL_SPI_ENABLE(&hspi1);
}
