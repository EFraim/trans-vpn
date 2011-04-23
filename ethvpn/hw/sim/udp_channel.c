#include "udp_channel.h"

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"

int socket_create_server(const char* addr, const char* port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        LOG_ERROR("socket() failed");
        return -1;
    }
    
    struct addrinfo* res = NULL;
    if (getaddrinfo(addr, port, NULL, &res)) {
        LOG_ERROR("Error from getaddrinfo()");
        return -1;
    }
    
    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1) {
        LOG_ERROR("bind() failed");
        return -1;
    }
    listen(sock, 1);
    
    freeaddrinfo(res);
    
    return sock;
}

int socket_poll_read(int sock) {
    struct pollfd pfd;
    pfd.fd = sock;
    pfd.events = POLLIN; 
    return poll(&pfd, 1, 0);
}


void udp_channel_create(udp_channel_t* chan,
                        const char* myaddress, const char* myport,
                        const char* otheraddress, const char* otherport)
{
    chan->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (chan->sock == -1) {
        LOG_ERROR("socket() failed");
        abort();
    }
    
    struct addrinfo* myaddr = NULL;
    if (getaddrinfo(myaddress, myport, NULL, &myaddr)) {
        LOG_ERROR("Error from getaddrinfo()");
        abort();
    }
    
    if (bind(chan->sock, myaddr->ai_addr, myaddr->ai_addrlen) == -1) {
        LOG_ERROR("bind() failed");
        abort();
    }
    
    freeaddrinfo(myaddr);
    
    if (getaddrinfo(otheraddress, otherport, NULL, &chan->otheraddr)) {
        LOG_ERROR("Error from getaddrinfo()");
        abort();
    }
}

void udp_channel_close(udp_channel_t* chan) {
    close(chan->sock);
    freeaddrinfo(chan->otheraddr);
}

void udp_channel_send(udp_channel_t* chan, const void* data, size_t size) {
    sendto(chan->sock, data, size, 0, chan->otheraddr->ai_addr, chan->otheraddr->ai_addrlen);
}

size_t udp_channel_recv(udp_channel_t* chan, void* data, size_t maxsize) {
    struct pollfd pfd;
    pfd.fd = chan->sock;
    pfd.events = POLLIN; 
    if (poll(&pfd, 1, 0)) {
        return recvfrom(chan->sock, data, maxsize, 0, NULL, NULL);
    } else {
        return 0;
    }
}
