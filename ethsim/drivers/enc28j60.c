#include "enc28j60.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "sock_util.h"
#include "log.h"

static int server_socket = -1;
static int data_socket = -1;

#define MAX_FRAME_LEN   2000

static void poll_server() {
    if (socket_poll_read(server_socket)) {
        data_socket = accept(server_socket, NULL, NULL);
        if (data_socket == -1) {
            LOG_WARNING("Failed to accept");
        }
        LOG_INFO("enc28j60: connection accepted");
    }
}

void enc28j60_init(void* iodir, void* iopin, uint8_t macaddr[], const char* port) {
    server_socket = socket_create_server("localhost", port);
    if (server_socket == -1) {
        abort();
    }
    poll_server();
}

void enc28j60_get_mac_address(uint8_t *macaddr) {
    memset(macaddr, 0, 6);
}


void enc28j60_packet_send(uint32_t len, uint8_t *packet) {
    if (data_socket != -1) {
        if (send(data_socket, &len, sizeof(len), 0) == -1) goto close_conn;
        if (send(data_socket, packet, len, 0) == -1) goto close_conn;
    } else {
        poll_server();
    }
    return;
close_conn:
    LOG_INFO("enc28j60: Closing connection");
    close(data_socket);
    data_socket = -1;
}

unsigned int enc28j60_packet_receive(uint32_t maxlen, uint8_t *packet) {
    static int framelen = -1;
    static int framebufsize = 0;
    static uint8_t framebuf[MAX_FRAME_LEN];
    
    if (data_socket != -1) {
        if (socket_poll_read(data_socket)) {
            int payload_ready;
            if (framelen == -1) {
                if (recv(data_socket, &framelen, sizeof(framelen), 0) != sizeof(framelen)) {
                    //LOG_ERROR("Received only a part of 'framelen'");
                    goto close_conn;
                }
                payload_ready = socket_poll_read(data_socket);
            } else {
                payload_ready = 1;
            }
            if (payload_ready) {
                int bytes_read = recv(data_socket, framebuf + framebufsize,
                                      framelen - framebufsize, 0);
                if (bytes_read <= 0) {
                    goto close_conn;
                }
                framebufsize += bytes_read;
            }
            if (framelen == framebufsize) {
                int retlen = framelen;
                memcpy(packet, framebuf, framebufsize);
                framelen = -1;
                framebufsize = 0;
                return retlen;
            }
        }
    } else {
        poll_server();
    }
    return 0;
    
close_conn:
    LOG_INFO("enc28j60: Closing connection");
    close(data_socket);
    data_socket = -1;
    return 0;
}
