#include "app.h"

#include "drivers/enc28j60.h"
#include "usbnet.h"

#include "log.h"
#include "util.h"

#include <uip/uip.h>
#include <uip/uip_arp.h>

#include <string.h>

#define ETHERNET_FRAME_SIZE_MAX  1050

typedef struct {
    uint16_t length;
    uint8_t data[ETHERNET_FRAME_SIZE_MAX + 4];
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

enum ServerConnState {
    SERVER_STATE_DISCONNECTED,
    SERVER_STATE_CONNECTING,
    SERVER_STATE_CONNECTED,
    SERVER_STATE_ABORTED
};

enum ServerConnState server_conn_state;
uint32_t connect_retry_timeout;

void uip_log(char* msg) {
    printf("%s%s", msg, "\n");
}


void UIP_UDP_APPCALL() {}

void UIP_APPCALL() {
    static int sending_in_progress = 0;
    
    if (uip_connected()) {
        LOG_DEBUG("Connection established");
        server_conn_state = SERVER_STATE_CONNECTED;
        sending_in_progress = 0;
    }
    
    if (uip_newdata() && uip_datalen() > 0) {
        LOG_DEBUG("New packet arrived from UIP");
        
        if (!ring_is_full(rx_ring)) {
            packet_t* pkt = ring_next(rx_ring); // get the next free packet
            
            pkt->length = *(uint16_t*)uip_appdata;
            if (pkt->length + sizeof(pkt->length) > uip_datalen()) {
                LOG_ERROR("Too short TCP packet received: %d!", (int)uip_datalen());
            }
            
            memcpy(pkt->data, uip_appdata + sizeof(pkt->length), pkt->length);
            
            if (usbnet_send(pkt->data, pkt->length)) {
                ring_append(rx_ring);
            } else {
                LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
            }
        } else {
            LOG_WARNING("** Rx Ring is full - dropping tunneled frame!");
        }
    }
    if (uip_poll() && !sending_in_progress) {
        size_t len = usbnet_pop_completed_recv();
        if (len > 0) {
            LOG_DEBUG("Got new packet from USB. Sending to UIP...");
            
            packet_t* pkt = ring_top(tx_ring);
            pkt->length = len;
            uip_send(pkt, pkt->length + sizeof(pkt->length));
            sending_in_progress = 1;
        }
    }
    if (uip_acked()) {
        LOG_DEBUG("Sent packet acked");
        sending_in_progress = 0;
        ring_pop(tx_ring);
    }
    if (uip_rexmit()) {
        LOG_DEBUG("Retransmit requested");
        packet_t* pkt = ring_top(tx_ring);
        uip_send(pkt->data, pkt->length);
    }
    if (uip_closed()) {
        LOG_DEBUG("Connection closed!");
        if (sending_in_progress) {
            ring_pop(tx_ring);
        }
        uip_close();
        server_conn_state = SERVER_STATE_DISCONNECTED;
    }
    if (uip_aborted()) {
        LOG_DEBUG("Connection aborted!");
        if (sending_in_progress) {
            ring_pop(tx_ring);
        }
        uip_close();
        server_conn_state = SERVER_STATE_ABORTED;
        connect_retry_timeout = get_current_time() + 1000;
    }
}

int time_greater(uint32_t t1, uint32_t t2) {
    return (t1 - t2) < 0x80000000;
}

void app_loop() {

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
    
    // set my address
    uip_ipaddr(&addr, 192, 168, 1, 2);
    uip_sethostaddr(&addr);
    
    // set my network mask
    uip_ipaddr(&addr, 255, 255, 255, 0);
    uip_setnetmask(&addr);
    
    // server's address
    uip_ipaddr(&addr, 192, 168, 1, 1);
    
    server_conn_state = SERVER_STATE_DISCONNECTED;
    
    struct uip_conn* conn = NULL;
    
    uint32_t uiptimeout = get_current_time() + 200;
    
    for(;;) {

        // receive the Ethernet frame and send it to UIP stack
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
        } else if (conn) {
            uip_poll_conn(conn);
            if (uip_len > 0) {
                uip_arp_out();
                enc28j60_packet_send(uip_len, uip_buf);
            }
        }
        
        if (server_conn_state == SERVER_STATE_ABORTED) {
            if (time_greater(get_current_time(), connect_retry_timeout)) {
                LOG_DEBUG("Timed out. Now will try to reconnect.");
                server_conn_state = SERVER_STATE_DISCONNECTED;
            }
        } else if (server_conn_state == SERVER_STATE_DISCONNECTED) {
            LOG_DEBUG("Trying to connect...");
            conn = uip_connect(&addr, HTONS(7777));
            if (!conn) {
                LOG_DEBUG("Failed to create UIP connection");
                server_conn_state = SERVER_STATE_ABORTED;
                connect_retry_timeout = get_current_time() + 1000;
            } else {
                server_conn_state = SERVER_STATE_CONNECTING;
            }
        }
        
        if (conn && time_greater(get_current_time(), uiptimeout)) {
            uiptimeout += 200;
            uip_periodic_conn(conn);
            if (uip_len > 0) {
                uip_arp_out();
                enc28j60_packet_send(uip_len, uip_buf);
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
