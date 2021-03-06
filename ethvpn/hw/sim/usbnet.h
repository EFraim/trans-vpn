#ifndef _USBNET_H_
#define _USBNET_H_

#include <stdint.h>

void usbnet_init(const char* myport, const char* otherport);

int usbnet_send(uint8_t* buffer, uint16_t length);
int usbnet_recv(uint8_t* buffer, uint16_t length);

int usbnet_pop_completed_send();
int usbnet_pop_completed_recv();

#endif
