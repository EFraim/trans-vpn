#include "sock_util.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

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
