#include "app.h"

#include "drivers/enc28j60.h"
#include "usbnet.h"
#include "log.h"
#include "util.h"

#include <uip/uip.h>
#include <uip/uip_arp.h>

#include <ssl/polarssl_util.h>

#include <string.h>

typedef struct {
    uint16_t length;
    uint8_t  data[1];
} buffer_t;

// max ethernet packet size of the host is maximal
// size of the TCP payload (which is 1500 - 40 = 1460), minus
// 2 (which is two-byte length field preceeded each sent packet).
// We do so, since we don't want the host's packets to be fragmented
// by TCP - this would cause performance degradation.
#define ETHERNET_FRAME_SIZE_MAX  1458

typedef struct {
    uint16_t length;
    uint8_t data[ETHERNET_FRAME_SIZE_MAX];
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

typedef struct {
    uint16_t length;
    uint8_t data[640];
} certificate_buffer_t;

// Both rings below hold host's packets, NOT the network packets!

// ring holding host's ethernet packets that should be sent to a server
ring_t* tx_ring;

// ring holding ethernet packets received from a server and should be sent to a host.
ring_t* rx_ring;

/*
 * State of the connection to the server:
 * 
 * SERVER_STATE_DISCONNECTED - disconnected and can try to connect anytime.
 * SERVER_STATE_CONNECTING   - connect was initiated but not yet established
 * SERVER_STATE_CONNECTED    - connection has been established
 * SERVER_STATE_ABORTED      - connection request was aborted (by RST). In this
 *                             state we wait some time (1 second usually) and then
 *                             go to SERVER_STATE_DISCONNECTED state which will
 *                             eventually lead to new connection request.
 */
enum ServerConnState {
    SERVER_STATE_DISCONNECTED,
    SERVER_STATE_CONNECTING,
    SERVER_STATE_CONNECTED,
    SERVER_STATE_ABORTED
};

/*
 * State of the packet reception over TCP connection.
 * 
 * CH_STATE_NEW_PACKET     - previous packet was fully received and next data
 *                           is expected to be a length of the new packet followed
 *                           by its payload.
 * CH_STATE_PARTIAL_PACKET - length of the current packet was received, but not its
 *                           full payload.
 */
enum ChannelState {
    CH_STATE_NEW_PACKET,
    CH_STATE_PARTIAL_PACKET
};

/*
 * State of the secure channel.
 * 
 * SC_STATE_INITIAL   - Initial state - TCP connection was just established.
 * SC_STATE_CERT_SENT - Client's certificate was sent, and now waiting for a server's
 *                      response (which is a key for symmetric encryption algorithm)
 * SC_STATE_CONNECTED - Server's response was received and all the following packets
 *                      are data packets.
 */
enum SecureChannelState {
    SC_STATE_INITIAL,
    SC_STATE_CERT_SENT,
    SC_STATE_CONNECTED
};

enum ServerConnState    server_conn_state;
enum ChannelState       channel_state;
enum SecureChannelState secure_channel_state;

// time when next connection attepmt will be taken
uint32_t connect_retry_timer;


// Certificate data

const int CLIENT_ID = 1;

const char* PUBLIC_MODULUS =
    "C3C8A4A90AE206C35E07DEE98648269B5FDA20B9424776403B350923F3EB491C"
    "60C793A818BE6B762EEFECC8E6B0AB9A64D792DFBE4B7A04582823BB64411A36"
    "80DD96D0C0526E3C3DF02E2D8FF575D7E6BDEE654157B31E319C80D59F40ED68"
    "AE8D87AD885E48053EA10DBF8B42800F7B2563A7AA5ED8D1ADBCD00C01713641";
    
const char* PUBLIC_KEY = "010001";

const char* PRIVATE_KEY =
    "232271B2485AB19E03D4E3C302AF1606921802015D0C980304DD0AABE20D1A4C"
    "B66D7A69132FB0F73F8B1CF21CBC1DBF9253416F57A611DA8FBB7C5617B2BB32"
    "365E2035D972D49D23354F774AF8EC74A32649A893BAA7426957DC452BF6FEB1"
    "1428AFE8555799ED56B2D5A40BD8F0B75EF10C4AEBEC51BDF8EEA2C84AFC3431";




void uip_log(char* msg) {
    printf("%s%s", msg, "\n");
}


void UIP_UDP_APPCALL() {}

void UIP_APPCALL() {
    // true iff a packet was sent to uip but not acked
    static int sending_in_progress = 0;
    
    // length of the current packet
    static int current_pkt_recv_length;
    
    static certificate_buffer_t cert_buffer;
    
    static buffer_t* last_sent_buffer = NULL;
    
    // TCP connection has been established
    if (uip_connected()) {
        // initialize state
        LOG_DEBUG("Connection established");
        server_conn_state = SERVER_STATE_CONNECTED;
        channel_state = CH_STATE_NEW_PACKET;
        secure_channel_state = SC_STATE_INITIAL;
        sending_in_progress = 0;
    }
    
    // new data has been received over the TCP connection
    if (uip_newdata() && uip_datalen() > 0) {
        LOG_DEBUG("New packet arrived from UIP");
        
        if (!ring_is_full(rx_ring)) {
            packet_t* pkt = ring_next(rx_ring); // get the next free packet
            
            if (channel_state == CH_STATE_NEW_PACKET) {
                if (uip_datalen() < 2) {
                    LOG_ERROR("Got TCP packet shorter than 2!");
                }
                
                // read length field
                pkt->length = *(uint16_t*)uip_appdata;
                // update number of received bytes
                current_pkt_recv_length = uip_datalen() - sizeof(pkt->length);
                // copy payload
                memcpy(pkt->data, uip_appdata + sizeof(pkt->length), current_pkt_recv_length);
                
                channel_state = CH_STATE_PARTIAL_PACKET;
            } else {
                // append payload and update number of received bytes
                memcpy(pkt->data + current_pkt_recv_length,
                       uip_appdata, uip_datalen());
                current_pkt_recv_length += uip_datalen();
            }
            
            // check if full packet was received
            if (current_pkt_recv_length == pkt->length) {
                channel_state = CH_STATE_NEW_PACKET;
                
                if (secure_channel_state == SC_STATE_CONNECTED) {
                    
                    // TODO: decrypt the received packet
                    
                    // send it to USB
                    if (usbnet_send(pkt->data, pkt->length)) {
                        ring_append(rx_ring);
                    } else {
                        LOG_ERROR("*** Failed to send frame over USB! Probably rings desynchronization!");
                    }
                } else if (secure_channel_state == SC_STATE_CERT_SENT) {
                    if (pkt->data[0] == 'y') {
                        LOG_INFO("Secure channel established");
                        secure_channel_state = SC_STATE_CONNECTED;

                        // TODO: read encryption key and use it...

                    } else {
                        if (pkt->data[0] != 'n') {
                            LOG_WARNING("Invalid server response!");
                        }
                        LOG_INFO("Secure channel authorization failed!");
                        uip_close();
                        server_conn_state = SERVER_STATE_ABORTED;
                        connect_retry_timer = get_current_time();
                    }
                } else {
                    LOG_ERROR("Protocol error - should not receive packets in this state");
                }
            }
        } else {
            LOG_WARNING("** Rx Ring is full - dropping tunneled frame!");
        }
    }
    
    // application is polled for new outgoing data
    // do nothing if previous data has not been acked yet.
    if (uip_poll() && !sending_in_progress) {
        if (secure_channel_state == SC_STATE_INITIAL) {
            // Generate the certificate 
            
            rsa_context ctx;
            rsa_init(&ctx, RSA_PKCS_V15, 0);
            rsa_set_public_modulus(&ctx, PUBLIC_MODULUS);
            rsa_set_private_key(&ctx, PRIVATE_KEY);

            memset(cert_buffer.data, 0, sizeof(cert_buffer.data));
            
            strcpy((char*)cert_buffer.data, PUBLIC_MODULUS);
            strcpy((char*)cert_buffer.data + 256, PUBLIC_KEY);

            uint8_t rsa_input[128];
            memset(rsa_input, 0, sizeof(rsa_input));
            *(int*)(rsa_input + sizeof(rsa_input) - sizeof(int)) = CLIENT_ID;
            
            rsa_private(&ctx, rsa_input, cert_buffer.data + 512);

            cert_buffer.length = 640;
                        
            // Send certificate to the server
            uip_send(&cert_buffer, cert_buffer.length + sizeof(cert_buffer.length));
            last_sent_buffer = (buffer_t*)&cert_buffer;
            sending_in_progress = 1;
            secure_channel_state = SC_STATE_CERT_SENT;
        } else if (secure_channel_state == SC_STATE_CONNECTED) {
            // get a packet from USB, encrypt it and send to the server
            size_t len = usbnet_pop_completed_recv();
            if (len > 0) {
                LOG_DEBUG("Got new packet from USB. Sending to UIP...");
                packet_t* pkt = ring_top(tx_ring);
                pkt->length = len;
                
                // TODO: encrypt it here
                
                uip_send(pkt, pkt->length + sizeof(pkt->length));
                last_sent_buffer = (buffer_t*)pkt;
                sending_in_progress = 1;
            }
        }
    }
    
    // previously sent data has been acked
    if (uip_acked()) {
        // clear the 'sending_in_progress' flag and pop the data packet from the tx ring
        LOG_DEBUG("Sent packet acked");
        sending_in_progress = 0;
        if (last_sent_buffer != (buffer_t*)&cert_buffer) {
            ring_pop(tx_ring);
        }
    }
    
    // retransmit of the last sent data is requested
    if (uip_rexmit()) {
        // send the top packet from the tx ring
        LOG_DEBUG("Retransmit requested");
        uip_send(last_sent_buffer, last_sent_buffer->length + sizeof(last_sent_buffer->length));
    }
    
    // TCP connection was closed
    if (uip_closed()) {
        LOG_DEBUG("Connection closed!");
        uip_close();
        server_conn_state = SERVER_STATE_DISCONNECTED;
    }
    
    // TCP connection was aborted
    if (uip_aborted()) {
        LOG_DEBUG("Connection aborted!");
        uip_close();
        server_conn_state = SERVER_STATE_ABORTED;
        // retry one second later...
        connect_retry_timer = get_current_time();
    }
}

void app_loop() {
    
    uint32_t myt = get_current_time();
    while (1) {
        uint32_t t = get_current_time();
        if (time_ms_diff(t, myt) > 1000) {
            LOG_DEBUG("Timed out!");
            myt = t;
        }
        ms_sleep(10);
    }
    
    // allocate rings on the stack, so they won't be a part of executable
    // => smaller image will be uploaded to the board
    ring_t rx_ring_instance;
    ring_t tx_ring_instance;
    
    rx_ring = &rx_ring_instance;
    tx_ring = &tx_ring_instance;
    
    ring_init(rx_ring);
    ring_init(tx_ring);
    
    // set MAC address of the board
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
    
    // timeout for periodic UIP timer
    uint32_t uiptimer = get_current_time();
    
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
            if (time_ms_diff(get_current_time(), connect_retry_timer) > 1000) {
                LOG_DEBUG("Timed out. Now will try to reconnect.");
                server_conn_state = SERVER_STATE_DISCONNECTED;
            }
        } else if (server_conn_state == SERVER_STATE_DISCONNECTED) {
            LOG_DEBUG("Trying to connect...");
            conn = uip_connect(&addr, HTONS(7777));
            if (!conn) {
                LOG_DEBUG("Failed to create UIP connection");
                server_conn_state = SERVER_STATE_ABORTED;
                connect_retry_timer = get_current_time();
            } else {
                server_conn_state = SERVER_STATE_CONNECTING;
            }
        }
        
        if (conn && time_ms_diff(get_current_time(), uiptimer) > 200) {
            uiptimer = get_current_time();
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
