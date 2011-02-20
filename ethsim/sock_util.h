#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include <stdint.h>

int socket_create_server(const char* addr, const char* port);
int socket_poll_read(int sock);

#endif
