#ifndef _USBCON_H_
#define _USBCON_H_

#include <stdint.h>

#define MAX_CMD_LEN 384
//Both strings are supposed to be terminated by \n\0 - reply on input, cmd on output.
void usbcon_send_response_await_query(const char* reply, char* cmd);

extern usb_device_logic_t usbConDriver;

#endif
