#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int socket_create_server(const char* addr, const char* port);
int socket_poll_read(int sock);

typedef struct {
    int              sock;
    struct addrinfo* otheraddr;
} udp_channel_t;

void udp_channel_create(udp_channel_t* chan,
                        const char* myaddress, const char* myport,
                        const char* otheraddress, const char* otherport);

void udp_channel_close(udp_channel_t* chan);

void udp_channel_send(udp_channel_t* chan, const void* data, size_t size);

size_t udp_channel_recv(udp_channel_t* chan, void* data, size_t maxsize);

#endif
