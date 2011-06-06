#ifndef _USBNET_H_
#define _USBNET_H_

#include <stdint.h>

#include "drivers/usb.h"

int usbnet_send(uint8_t* buffer, uint16_t length);
int usbnet_recv(uint8_t* buffer, uint16_t length);

int usbnet_pop_completed_send();
int usbnet_pop_completed_recv();

extern usb_device_logic_t usbNetDriver;

#endif
