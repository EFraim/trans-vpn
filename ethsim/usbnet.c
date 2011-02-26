#include "usbnet.h"

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

static udp_channel_t chan;

void usbnet_init(const char* myport, const char* otherport) {
    usbring_init(&send_ring);
    usbring_init(&recv_ring);
    recv_ring_drop = 0;
    
    udp_channel_create(&chan, "localhost", myport, "localhost", otherport);
}

int usbnet_send(uint8_t* buffer, uint16_t length) {
    usbring_post_buffer(&send_ring, buffer, length);

    udp_channel_send(&chan, buffer, length);
    
    send_ring.buffers[send_ring.begin].current = length;
    usbring_free_buffer(&send_ring);
    
    return 1;
}

int usbnet_recv(uint8_t* buffer, uint16_t length) {
    return usbring_post_buffer(&recv_ring, buffer, length);
}

int usbnet_pop_completed_send() {
    return usbring_pop_freed(&send_ring);
}

int usbnet_pop_completed_recv() {
    if (recv_ring.size > 0) {
        usb_buffer_t* buf = &recv_ring.buffers[recv_ring.begin];
        size_t recvsize = udp_channel_recv(&chan, buf->data, buf->length);
        if (recvsize > 0) {
            buf->current = recvsize;
            usbring_free_buffer(&recv_ring);
        }
    }
    
    return usbring_pop_freed(&recv_ring);
}
