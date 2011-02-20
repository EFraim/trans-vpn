#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>

void enc28j60_init(void* iodir, void* iopin, uint8_t macaddr[], const char* port);

void enc28j60_get_mac_address(uint8_t *macaddr);

void enc28j60_packet_send(uint32_t len, uint8_t *packet);
unsigned int enc28j60_packet_receive(uint32_t maxlen, uint8_t *packet);

#endif
