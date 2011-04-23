#ifndef _PKT_CHANNEL_H_
#define _PKT_CHANNEL_H_

#include "channel_interface.h"

/*
 * This is implementation of Packet Channel over TCP.
 * 
 * Packet Channel provides packets communication over streaming protocol.
 * It adds header of packet length in the beginning of each logical packet, and
 * assebles/splits one or several incoming TCP packets into one or several logical
 * packets.
 */

/*
 * State of the packet reception.
 * 
 * PKT_CHANNEL_NEW_PACKET     - previous packet was fully received and next data
 *                              is expected to be a length of the new packet followed
 *                              by its payload.
 * PKT_CHANNEL_PARTIAL_PACKET - length of the current packet was received, but not its
 *                              full payload.
 */
enum PacketChannelPacketState {
    PKT_CHANNEL_NEW_PACKET,
    PKT_CHANNEL_PARTIAL_PACKET
};

// allocation and freeing functions needed for the implementation
typedef void* (*allocate_buffer_function_t)(size_t* size);
typedef void  (*free_buffer_function_t)(void* data);

/*
 * State of the protocol.
 */
typedef struct {
    // interface to other layers
    channel_interface_t             interface;
    
    // allocator of working buffers
    allocate_buffer_function_t      allocate_buffer;
    
    // freeer of working buffers
    free_buffer_function_t          free_buffer;
    
    // current data state
    enum PacketChannelPacketState   packet_state;
    
    // work buffer (used for packet assembly)
    void*                           work_buffer;
    
    // size of the work buffer
    size_t                          work_buffer_size;
    
    // total size of the current packet 
    size_t                          current_pkt_total_size;
    
    // size of the current packet received so far
    size_t                          current_pkt_recv_size;
} pkt_channel_state_t;


// initializes the protocol state ("reset" still must be called after this call)
void    pkt_channel_init(pkt_channel_state_t* state, channel_interface_t* interface, 
                         allocate_buffer_function_t allocate_buffer,
                         free_buffer_function_t free_buffer);
// resets the state to initial
void    pkt_channel_reset(pkt_channel_state_t* state);
void    pkt_channel_input_packet(pkt_channel_state_t* state, void* data, size_t size);
void    pkt_channel_poll(pkt_channel_state_t* state);
int     pkt_channel_send(pkt_channel_state_t* state, void* data, size_t size);
void    pkt_channel_close(pkt_channel_state_t* state);

size_t  pkt_channel_header_size();

#endif
