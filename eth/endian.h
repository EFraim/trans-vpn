#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <stdint.h>

uint32_t uint32FromLittleEndian(uint8_t *buffer);
void uint32ToLittleEndian(uint32_t x, uint8_t *buffer);

#endif
