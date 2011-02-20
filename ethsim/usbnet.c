#include "usbnet.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "sock_util.h"
#include "log.h"



/*
 * USB ring definition
 */

typedef struct {
    uint8_t* data;
    uint16_t length;
    uint16_t current;
} usb_buffer_t;

#define USB_RING_CAPACITY 4
typedef struct {
    usb_buffer_t buffers[USB_RING_CAPACITY];
    int begin;
    int size;
    int nfreed;
} usb_ring_t;

/*
 * USB ring implemetation
 */

void usbring_init(usb_ring_t* ring) {
    ring->begin = 0;
    ring->size = 0;
    ring->nfreed = 0;
}

int usbring_post_buffer(usb_ring_t* ring, uint8_t* buffer, uint16_t length) {
    usb_buffer_t* buf;
    if (ring->size + ring->nfreed == USB_RING_CAPACITY) {
        return 0;
    }
    buf = &ring->buffers[(ring->begin + ring->size) & (USB_RING_CAPACITY - 1)];
    buf->data = buffer;
    buf->length = length;
    buf->current = 0;
    
    ring->size++;

    return 1;    
}

void usbring_free_buffer(usb_ring_t* ring) {
    ring->begin = (ring->begin + 1) & (USB_RING_CAPACITY - 1);
    ring->size--;
    ring->nfreed++;
}

int usbring_pop_freed(usb_ring_t* ring) {
    if (ring->nfreed == 0) {
        return 0;
    } else {
        int first = (ring->begin - ring->nfreed) & (USB_RING_CAPACITY - 1);
        usb_buffer_t* buf = &ring->buffers[first];
        int len = buf->current;
        ring->nfreed--;
        return len;
    }
}

void usbring_reset(usb_ring_t* ring) {
    if (ring->size > 0) {
        ring->buffers[ring->begin].current = 0;
    }
}

/*
 * Send and Recieve rings
 */ 

usb_ring_t send_ring;
usb_ring_t recv_ring;
int recv_ring_drop;

/*
 * API implementation
 */

static int server_socket = -1;
static int data_socket = -1;

static void poll_server() {
    if (socket_poll_read(server_socket)) {
        data_socket = accept(server_socket, NULL, NULL);
        if (data_socket == -1) {
            LOG_WARNING("Failed to accept");
        }
        LOG_INFO("usbnet: connection accepted");
    }
}

void usbnet_init(const char* port) {
    usbring_init(&send_ring);
    usbring_init(&recv_ring);
    recv_ring_drop = 0;
    
    server_socket = socket_create_server("localhost", port);
    if (server_socket == -1) {
        abort();
    }
    poll_server();
}

int usbnet_send(uint8_t* buffer, uint16_t length) {
    usbring_post_buffer(&send_ring, buffer, length);
    
    if (data_socket != -1) {
        int len = length;
        if (send(data_socket, &len, sizeof(len), 0) == -1) goto close_conn;
        if (send(data_socket, buffer, len, 0) == -1) goto close_conn;
    } else {
        poll_server();
    }
    
    send_ring.buffers[send_ring.begin].current = length;
    usbring_free_buffer(&send_ring);
    
    return 1;

close_conn:
    LOG_INFO("usbnet: Closing connection");
    close(data_socket);
    data_socket = -1;

    return 1;
}

int usbnet_recv(uint8_t* buffer, uint16_t length) {
    return usbring_post_buffer(&recv_ring, buffer, length);
}

int usbnet_pop_completed_send() {
    return usbring_pop_freed(&send_ring);
}

int usbnet_pop_completed_recv() {
    static int framelen = -1;
    
    if (recv_ring.size > 0) {
        if (data_socket != -1) {
            usb_buffer_t* buf = &recv_ring.buffers[recv_ring.begin];
            
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
                    int bytes_read = recv(data_socket, buf->data + buf->current,
                                          framelen - buf->current, 0);
                    if (bytes_read <= 0) {
                        goto close_conn;
                    }
                    buf->current += bytes_read;
                }
                
                if (framelen == buf->current) {
                    framelen = -1;
                    usbring_free_buffer(&recv_ring);
                }
            }
        } else {
            poll_server();
        }
    }
    
    return usbring_pop_freed(&recv_ring);
close_conn:
    LOG_INFO("usbnet: Closing connection");
    close(data_socket);
    data_socket = -1;
    return usbring_pop_freed(&recv_ring);
}
