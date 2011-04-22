#include "appnet.h"

#include "drivers/enc28j60.h"
#include "drivers/usb.h"
#include "usbnet.h"

#include "log.h"
#include "util.h"

#include <uip/uip.h>
#include <uip/uip_arp.h>

#include <string.h>

#define ETHERNET_FRAME_SIZE_MAX  1050

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


ring_t* rx_ring;
ring_t* tx_ring;
    
void uip_log(char* msg) {
    printf("%s%s", msg, "\n");
}


void UIP_APPCALL() {}

void UIP_UDP_APPCALL() {
    if (uip_newdata()) {
        if (!ring_is_full(rx_ring)) {
            packet_t* pkt = ring_next(rx_ring); // get the next free packet
            pkt->length = uip_datalen();
            memcpy(pkt->data, uip_appdata, pkt->length);
            if (usbnet_send(pkt->data, pkt->length)) {
                ring_append(rx_ring);
            } else {
                LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
            }
        } else {
            LOG_WARNING("** Rx Ring is full - dropping tunneled frame!");
        }

    }
    if (uip_poll()) {
        size_t len = usbnet_pop_completed_recv();
        if (len > 0) {
            packet_t* pkt = ring_top(tx_ring);
            memcpy(uip_appdata, pkt->data, len);
            uip_udp_send(len);
            ring_pop(tx_ring);
        }
    }
}

void appnet_loop() {
    ring_t rx_ring_instance;
    ring_t tx_ring_instance;
    
    rx_ring = &rx_ring_instance;
    tx_ring = &tx_ring_instance;
    
    ring_init(rx_ring);
    ring_init(tx_ring);
    
    struct uip_eth_addr mac_addr = {{20, 21, 22, 23, 24, 25}};
    uip_ipaddr_t addr;
    
    uip_init();
    
    uip_setethaddr(mac_addr);
    
    uip_ipaddr(&addr, 10, 0, 0, 2);
    uip_sethostaddr(&addr);
    
    uip_ipaddr(&addr, 255, 255, 255, 0);
    uip_setnetmask(&addr);
    
    uip_ipaddr(&addr, 10, 0, 0, 1);
    
    struct uip_udp_conn* conn = uip_udp_new(&addr, HTONS(7777));
    uip_udp_bind(conn, HTONS(7777));
    
    for(;;) {

        uip_len = enc28j60_packet_receive(sizeof(uip_buf), uip_buf);
        if (uip_len > 0) {
            struct uip_eth_hdr* eth_header = (struct uip_eth_hdr*)uip_buf;
            if (eth_header->type == HTONS(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();
                if (uip_len > 0) {
                    uip_arp_out();
                    enc28j60_packet_send(uip_len, uip_buf);
                }
            } else if (eth_header->type == HTONS(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                if (uip_len > 0) {
                    enc28j60_packet_send(uip_len, uip_buf);
                }
            }
        } else {
            for (int i = 0; i < UIP_UDP_CONNS; ++i) {
                uip_udp_periodic(i);
                if (uip_len > 0) {
                    uip_arp_out();
                    enc28j60_packet_send(uip_len, uip_buf);
                }
            }
        }
        
        // while there are free buffers in tx queue, post them to USB engine
        while (!ring_is_full(tx_ring)) {
            packet_t* pkt = ring_next(tx_ring);
            pkt->length = sizeof(pkt->data);
            if (usbnet_recv(pkt->data, pkt->length)) {
                ring_append(tx_ring);
            } else {
                LOG_ERROR("*** Posting tx buffer to USB failed! Probably rings desynchronization!");
            }
        }
        
        // free rx buffers that were successfully delivered to host
        while (usbnet_pop_completed_send()) {
            LOG_DEBUG("Sending full frame to USB done");
            ring_pop(rx_ring);
        }
                
        ms_sleep(1);
    }
}
