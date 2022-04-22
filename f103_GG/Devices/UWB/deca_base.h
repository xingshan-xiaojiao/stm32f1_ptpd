#ifndef _DECA_BASE_H
#define _DECA_BASE_H

#include "spi.h"
#include "gpio.h"
#include "tim.h"

typedef int decaIrqStatus_t ; // Type for remembering IRQ status

int writetospi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t bodyLength, const uint8_t *bodyBuffer);
int readfromspi(uint16_t headerLength, const uint8_t *headerBuffer, uint32_t readLength, uint8_t *readBuffer);

decaIrqStatus_t decamutexon(void) ;

void decamutexoff(decaIrqStatus_t s) ;
decaIrqStatus_t decamutexon(void);

void deca_sleep(unsigned int time_ms);
void deca_usleep(unsigned int time_ms);

void setup_DW1000RSTnIRQ(int enable);
void reset_DW1000(void);
void wakeup_dw1000(void);
void set_dw1000_slowrate(void);
void set_dw1000_fastrate(void);


#endif
