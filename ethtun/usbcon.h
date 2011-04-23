#ifndef _USBCON_H_
#define _USBCON_H_

#include <stdint.h>

static const int MAX_CMD_LEN=256;
int usbcon_send(const char* reply);
int usbcon_recv(char* cmd);

extern usb_device_logic_t usbConDriver;

#endif
