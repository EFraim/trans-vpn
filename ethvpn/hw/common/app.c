#include "app.h"

#include "drivers/enc28j60.h"
#include "usbnet.h"
#include "log.h"
#include "util.h"
#include "pkt_channel.h"
#include "secure_channel.h"

#include <string.h>
#include <stdlib.h>

#include <uip/timer.h>
#include <uip/uip.h>
#include <uip/uip_arp.h>

#include <polarssl_util.h>
#include <polarssl/aes.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define TCP_MAX_PAYLOAD 1460

/*
 * Buffer type for storing packets being transfered between UIP and host.
 */
typedef struct {
    uint8_t data[TCP_MAX_PAYLOAD];
} buffer_t;

// must be power of 2
#define RING_CAPACITY 4

/*
 * Ring of the defined above buffers.
 */
typedef struct {
    buffer_t pkt[RING_CAPACITY];
    int begin;
    int size;
} ring_t;

/*
 * Various methods on ring datatype.
 */

// intitializes the ring data structure
void ring_init(ring_t* ring) {
    ring->begin = 0;
    ring->size = 0;
}

// checks whether ring is empty
int ring_is_empty(ring_t* ring) {
    return ring->size == 0;
}

// checks whether ring is full
int ring_is_full(ring_t* ring) {
    return ring->size == RING_CAPACITY;
}

// returns the next free buffer in the ring
buffer_t* ring_next(ring_t* ring) {
    return &(ring->pkt[(ring->begin + ring->size) & (RING_CAPACITY - 1)]);
}

// adds a next free buffer to the ring (returned by ring_next)
void ring_append(ring_t* ring) {
    ring->size++;
}

// returns the top buffer in the ring (the first that was added)
buffer_t* ring_top(ring_t* ring) {
    return &(ring->pkt[ring->begin]);
}

// pops the top buffer from the ring 
void ring_pop(ring_t* ring) {
    ring->begin = (ring->begin + 1) & (RING_CAPACITY - 1);
    ring->size--;
}


// Client's RSA information

const char* CLIENT_PUBLIC_MODULUS =
    "C3C8A4A90AE206C35E07DEE98648269B5FDA20B9424776403B350923F3EB491C"
    "60C793A818BE6B762EEFECC8E6B0AB9A64D792DFBE4B7A04582823BB64411A36"
    "80DD96D0C0526E3C3DF02E2D8FF575D7E6BDEE654157B31E319C80D59F40ED68"
    "AE8D87AD885E48053EA10DBF8B42800F7B2563A7AA5ED8D1ADBCD00C01713641";
    
const char* CLIENT_PUBLIC_KEY = "010001";

const char* CLIENT_PRIVATE_KEY =
    "232271B2485AB19E03D4E3C302AF1606921802015D0C980304DD0AABE20D1A4C"
    "B66D7A69132FB0F73F8B1CF21CBC1DBF9253416F57A611DA8FBB7C5617B2BB32"
    "365E2035D972D49D23354F774AF8EC74A32649A893BAA7426957DC452BF6FEB1"
    "1428AFE8555799ED56B2D5A40BD8F0B75EF10C4AEBEC51BDF8EEA2C84AFC3431";

// Server's RSA information

const char* SERVER_PUBLIC_MODULUS =
    "AEEE654C29703F6A6895BECFEB12B8301DE9004E9FC42E52075E26AE8127A44C"
    "69FA3521F67387687605BE269D1CC72B9C7447B40B8779FC6AA5C68CB20A63AD"
    "CFA30A6A9CF2C49C5889CB8071C2910B47EEC5DA543A316270A6670CD7F5D49A"
    "29801B44994A3AE0DFF74A246A63775F1C773EDE0A939AEF2D415DB8347AA939";

const char* SERVER_PUBLIC_KEY = "010001";

// empty logger for UIP
void uip_log(char* msg) {
}

/*
 * State of communication with host.
 */
typedef struct {
    ring_t  rx_ring; // receive ring - incoming packets from UIP
    ring_t  tx_ring; // transmit ring - outgoing packets to UIP
    size_t  headers_size; // total size of encryption headers 
} host_state_t;

/*
 * State of the TCP connection to the server:
 * 
 * TCP_CONN_DISCONNECTED - disconnected and will try to connect immediately.
 * TCP_CONN_CONNECTING   - connection attept was initiated but not yet established
 * TCP_CONN_CONNECTED    - connection has been established
 * TCP_CONN_CLOSED       - connection was closed. In this state we wait some time
 *                             (1 second) and then go to TCP_CONN_DISCONNECTED state
 *                             which will eventually lead to new connection request.
 */
enum TcpConnState {
    TCP_CONN_DISCONNECTED,
    TCP_CONN_CONNECTING,
    TCP_CONN_CONNECTED,
    TCP_CONN_CLOSED
};

typedef struct {
    enum TcpConnState    state; // state of TCP connection
    void*                   sent_buffer; // last sent bufffer (needed for retransmitions)
    size_t                  sent_buffer_size; // last sent buffer's size
    int                     is_sending_in_progress; // whether last sent was not yet acked
    struct timer            reconnect_timer; // timer for reconnection attempt
    //uint32_t                close_time; // time when connection was closed
} connection_state_t;


// pointers to all state machines
connection_state_t*     conn_state;
pkt_channel_state_t*    pkt_channel_state;
secure_channel_state_t* secure_channel_state;
host_state_t*           host_state;


void host_input(host_state_t* state, void* data, size_t size) {
    if (usbnet_send(data, size)) {
        LOG_DEBUG("USB packet queued for sending");
        ring_append(&state->rx_ring);
    } else {
        LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
    }
}

void host_poll(host_state_t* state) {
    size_t len = usbnet_pop_completed_recv();
    if (len > 0) {
        if (len > (TCP_MAX_PAYLOAD - state->headers_size)) {
            LOG_ERROR("Received too long ethernet frame from USB!");
            secure_channel_close(secure_channel_state);
            return;
        }
        
        LOG_DEBUG("Got new packet from USB. Processing...");
        buffer_t* pkt = ring_top(&state->tx_ring);
        
        secure_channel_send(secure_channel_state,
                            pkt->data + state->headers_size, len);
    }
}


void server_connection_close(connection_state_t* state) {
    LOG_INFO("Closing server connection");
    uip_close();
    timer_set(&state->reconnect_timer, CLOCK_SECOND * 1);
    //state->close_time = get_current_time();
    state->state = TCP_CONN_CLOSED;
}

int server_connection_send(connection_state_t* state, void* data, size_t size) {
    if (state->is_sending_in_progress) {
        return 0;
    } else {
        LOG_DEBUG("Sending packet to UIP of length %d", size);
        state->sent_buffer = data;
        state->sent_buffer_size = size;
        state->is_sending_in_progress = 1;
        uip_send(data, size);
        return 1;
    }
}

void* get_next_rx_buffer(size_t* size) {
    if (ring_is_full(&host_state->rx_ring)) {
        return NULL;
    } else {
        buffer_t* pkt = ring_next(&host_state->rx_ring);
        *size = sizeof(pkt->data);
        return pkt->data;
    }
}

void do_nothing(void* data) {
}

void UIP_UDP_APPCALL() {}

void UIP_APPCALL() {
    
    // TCP connection has been established
    if (uip_connected()) {
        // initialize state
        LOG_DEBUG("TCP connection established");
        
        conn_state->state = TCP_CONN_CONNECTED;
        conn_state->is_sending_in_progress = 0;
        conn_state->sent_buffer = NULL;
        conn_state->sent_buffer_size = 0;
        
        pkt_channel_reset(pkt_channel_state);
        secure_channel_reset(secure_channel_state);
    }
    
    // new data has been received over the TCP connection
    if (uip_newdata() && uip_datalen() > 0) {
        LOG_DEBUG("New packet arrived from UIP");
        
        pkt_channel_input_packet(pkt_channel_state, uip_appdata, uip_datalen());
    }
    
    // application is polled for new outgoing data
    // do nothing if previous data has not been acked yet.
    if (uip_poll() && !conn_state->is_sending_in_progress) {
        pkt_channel_poll(pkt_channel_state);
    }
    
    // previously sent data has been acked
    if (uip_acked()) {
        // clear the 'sending_in_progress' flag and pop the data packet from the tx ring
        LOG_DEBUG("Sent packet acked");
        if (conn_state->is_sending_in_progress &&
            conn_state->sent_buffer == ring_top(&host_state->tx_ring)->data)
        {
            ring_pop(&host_state->tx_ring);
        }
        conn_state->is_sending_in_progress = 0;
    }
    
    // retransmit of the last sent data is requested
    if (uip_rexmit()) {
        // send the top packet from the tx ring
        LOG_DEBUG("Retransmit requested");
        uip_send(conn_state->sent_buffer, conn_state->sent_buffer_size);
    }
    
    // TCP connection was closed
    if (uip_closed()) {
        LOG_DEBUG("Connection closed!");
        server_connection_close(conn_state);
    }
    
    // TCP connection was aborted
    if (uip_aborted()) {
        LOG_DEBUG("Connection aborted!");
        server_connection_close(conn_state);
    }
}

void app_loop() {
    
    // Initialize all state machines
    
    // allocate all states on the stack, so they won't be a part of executable
    // and so smaller image will be uploaded to the board
    connection_state_t conn_state_instance = {
        TCP_CONN_DISCONNECTED,
        NULL,
        0,
        0
    };
    pkt_channel_state_t pkt_channel_state_instance;
    secure_channel_state_t secure_channel_state_instance;
    host_state_t host_state_instance;
    
    conn_state = &conn_state_instance;
    pkt_channel_state = &pkt_channel_state_instance;
    secure_channel_state = &secure_channel_state_instance;
    host_state = &host_state_instance;
    
    channel_interface_t pkt_channel_interface = {
        secure_channel_state,
        (channel_input_function_t)secure_channel_input_packet,
        (channel_poll_function_t)secure_channel_poll,
        0,
        conn_state,
        (channel_send_function_t)server_connection_send,
        (channel_close_function_t)server_connection_close
    };
    pkt_channel_init(pkt_channel_state, &pkt_channel_interface, get_next_rx_buffer, do_nothing);
    
    channel_interface_t secure_channel_interface = {
        host_state,
        (channel_input_function_t)host_input,
        (channel_poll_function_t)host_poll,
        pkt_channel_header_size(),
        pkt_channel_state,
        (channel_send_function_t)pkt_channel_send,
        (channel_close_function_t)pkt_channel_close
    };
    secure_channel_auth_info_t secure_channel_auth_info = {
        CLIENT_PUBLIC_MODULUS,
        CLIENT_PUBLIC_KEY,
        CLIENT_PRIVATE_KEY,
        SERVER_PUBLIC_MODULUS,
        SERVER_PUBLIC_KEY
    };
    secure_channel_init(secure_channel_state, &secure_channel_interface, &secure_channel_auth_info);
    
    ring_init(&host_state->rx_ring);
    ring_init(&host_state->tx_ring);
    host_state->headers_size = secure_channel_header_size() + pkt_channel_header_size();
        
    // MAC address of the board
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
    
    struct uip_conn* conn = NULL;
    
    struct timer periodic_uip_timer;
    
    timer_set(&periodic_uip_timer, CLOCK_SECOND / 5);
    
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
        
        if (conn_state->state == TCP_CONN_CLOSED) {
            if (timer_expired(&conn_state->reconnect_timer)) {
                LOG_DEBUG("Timed out. Now will try to reconnect.");
                conn_state->state = TCP_CONN_DISCONNECTED;
            }
        } else if (conn_state->state == TCP_CONN_DISCONNECTED) {
            LOG_DEBUG("Trying to connect...");
            conn = uip_connect(&addr, HTONS(7777));
            if (!conn) {
                LOG_DEBUG("Failed to create UIP connection");
                conn_state->state = TCP_CONN_CLOSED;
                timer_restart(&conn_state->reconnect_timer);
            } else {
                conn_state->state = TCP_CONN_CONNECTING;
            }
        }
        
        if (conn && timer_expired(&periodic_uip_timer)) {
            timer_reset(&periodic_uip_timer);
            uip_periodic_conn(conn);
            if (uip_len > 0) {
                uip_arp_out();
                enc28j60_packet_send(uip_len, uip_buf);
            }
        }
        
        // while there are free buffers in tx queue, post them to USB engine
        while (!ring_is_full(&host_state->tx_ring)) {
            buffer_t* pkt = ring_next(&host_state->tx_ring);
            //pkt->length = sizeof(pkt->data);
            if (usbnet_recv(pkt->data + host_state->headers_size,
                            sizeof(pkt->data) - host_state->headers_size))
            {
                ring_append(&host_state->tx_ring);
            } else {
                LOG_ERROR("*** Posting tx buffer to USB failed! Probably rings desynchronization!");
            }
        }
        
        // free rx buffers that were successfully delivered to host
        while (usbnet_pop_completed_send()) {
            LOG_DEBUG("Sending full frame to USB done");
            ring_pop(&host_state->rx_ring);
        }
                
        sim_sleep(1);
    }
}
