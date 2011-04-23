#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#define 	S0SPCR_MSTR   0x20

// public functions
void initSPI(void);
uint8_t spiPut (uint8_t valueIn);

#endif
