#include "app.h"

#include "log.h"
#include "drivers/enc28j60.h"
#include "drivers/usb.h"
#include "usbnet.h"

#define ETHERNET_FRAME_SIZE_MAX  1536

typedef struct {
    uint8_t data[ETHERNET_FRAME_SIZE_MAX + 4];
    uint16_t length;
} packet_t;

#define RING_SIZE 4

typedef struct {
    packet_t pkt[RING_SIZE];
    int begin;
    int size;
} ring_t;

void ring_init(ring_t* ring) {
    ring->begin = 0;
    ring->size = 0;
}
int ring_is_empty(ring_t* ring) {
    return ring->size == 0;
}
int ring_is_full(ring_t* ring) {
    return ring->size == RING_SIZE;
}
void ring_append(ring_t* ring) {
    ring->size++;
}
packet_t* ring_next(ring_t* ring) {
    return &(ring->pkt[(ring->begin + ring->size) & (RING_SIZE - 1)]);
}
packet_t* ring_top(ring_t* ring) {
    return &(ring->pkt[ring->begin]);
}
void ring_pop(ring_t* ring) {
    ring->begin = (ring->begin + 1) & (RING_SIZE - 1);
    ring->size--;
}



void app_loop() {
    ring_t rx_ring;
    ring_t tx_ring;
    
    ring_init(&rx_ring);
    ring_init(&tx_ring);
    
    for(;;) {

        // if the receive ring is not full, try to recieve a new frame
        if (!ring_is_full(&rx_ring)) {
            packet_t* pkt = ring_next(&rx_ring); // get the next free packet
            int recv_len = enc28j60_packet_receive(sizeof(pkt->data), pkt->data);
            if (recv_len > 0) {
                LOG_DEBUG("Received ethernet frame of size %d", recv_len);
                if (usbnet_send(pkt->data, recv_len)) {
                    pkt->length = recv_len;
                    ring_append(&rx_ring);
                } else {
                    LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
                }
            }
        } else {
            //LOG_WARNING("** Rx Ring is full - cannot store received ethernet frame!");
        }
        
        // while there are free buffers in tx queue, post them to USB engine
        while (!ring_is_full(&tx_ring)) {
            packet_t* pkt = ring_next(&tx_ring);
            pkt->length = sizeof(pkt->data);
            if (usbnet_recv(pkt->data, pkt->length)) {
                ring_append(&tx_ring);
            } else {
                LOG_ERROR("*** Posting tx buffer to USB failed! Probably rings desynchronization!");
            }
        }
        
        // free rx buffers that were successfully delivered to host
        while (usbnet_pop_completed_send()) {
            LOG_DEBUG("Sending full frame to USB done");
            ring_pop(&rx_ring);
        }
        
        int recv_len;
        // go over received buffers from host, and send them over Ethernet
        while ((recv_len = usbnet_pop_completed_recv())) {
            packet_t* pkt = ring_top(&tx_ring);
            pkt->length = recv_len;
            enc28j60_packet_send(pkt->length, pkt->data);
            LOG_DEBUG("Sent ethernet frame of size %d", pkt->length);
            ring_pop(&tx_ring);
        }   
    }
}
