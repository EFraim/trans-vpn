#include "enc28j60.h"

#include <string.h>

#include <udp_channel.h>
#include <log.h>

static udp_channel_t chan;

void enc28j60_init(void* iodir, void* iopin, uint8_t macaddr[],
                   const char* myport, const char* otherport)
{
    udp_channel_create(&chan, "localhost", myport, "localhost", otherport);
}

void enc28j60_get_mac_address(uint8_t *macaddr) {
    memset(macaddr, 0, 6);
}

void enc28j60_packet_send(uint32_t len, uint8_t *packet) {
    udp_channel_send(&chan, packet, len);
}

unsigned int enc28j60_packet_receive(uint32_t maxlen, uint8_t *packet) {
    return udp_channel_recv(&chan, packet, maxlen);
}
