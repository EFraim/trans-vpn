#include "pkt_channel.h"

#include <stdint.h>
#include <string.h>

#include <log.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

void pkt_channel_init(pkt_channel_state_t* state, channel_interface_t* interface,
                      allocate_buffer_function_t allocate_buffer,
                      free_buffer_function_t free_buffer)
{
    state->interface = *interface;
    state->allocate_buffer = allocate_buffer;
    state->free_buffer = free_buffer;
}

void pkt_channel_reset(pkt_channel_state_t* state) {
    state->packet_state = PKT_CHANNEL_NEW_PACKET;
    state->work_buffer = NULL;
    state->work_buffer_size = 0;
    state->current_pkt_total_size = 0;
    state->current_pkt_recv_size = 0;
}

static uint16_t read16(const uint8_t* data) {
    return (uint16_t)data[0] | (((uint16_t)data[1]) << 8);
}

void pkt_channel_input_packet(pkt_channel_state_t* state, void* data, size_t size) {
    uint8_t* pkt_data = data;
    size_t   pkt_size = size;
    while (pkt_size > 0) {
        if (state->packet_state == PKT_CHANNEL_NEW_PACKET) {
            
            // get new working buffer
            state->work_buffer = state->allocate_buffer(&(state->work_buffer_size));
            if (!state->work_buffer) {
                LOG_WARNING("PKT_CHANNEL: Failed to allocate work buffer - dropping packet!");
            }
            
            state->current_pkt_total_size = pkt_data[0];
            state->packet_state = PKT_CHANNEL_PARTIAL_HEADER;
            pkt_data += 1;
            pkt_size -= 1;
            
        } else if (state->packet_state == PKT_CHANNEL_PARTIAL_HEADER) {
            state->current_pkt_total_size |= ((uint16_t)pkt_data[0]) << 8;
            
            if (state->current_pkt_total_size > state->work_buffer_size) {
                LOG_ERROR("PKT_CHANNEL: Received too long packet: %d > %d!",
                    state->current_pkt_total_size, state->work_buffer_size);
                pkt_channel_close(state);
                return;
            }
            
            state->current_pkt_recv_size = 0;
            state->packet_state = PKT_CHANNEL_PARTIAL_PACKET;
            
            pkt_data += 1;
            pkt_size -= 1;
            
        } else {
            
            uint16_t current_size= MIN(pkt_size, state->current_pkt_total_size -
                                                 state->current_pkt_recv_size);
            
            // if no buffer is supplied - drop the packet
            if (!state->work_buffer) {
                return;
            }
            
            // append payload and update number of received bytes
            memcpy(state->work_buffer + state->current_pkt_recv_size,
                   pkt_data, current_size);
            state->current_pkt_recv_size += current_size;

            pkt_data += current_size;
            pkt_size -= current_size;
            
            // if full packet is received, pass it to upper layer and free working buffer
            if (state->current_pkt_recv_size == state->current_pkt_total_size) {
                state->packet_state = PKT_CHANNEL_NEW_PACKET;
                
                state->interface.input_function(state->interface.upper_state,
                                                state->work_buffer,
                                                state->current_pkt_total_size);
                
                state->free_buffer(state->work_buffer);
            }
        }
    }
    
}

void pkt_channel_poll(pkt_channel_state_t* state) {
    state->interface.poll_function(state->interface.upper_state);
}

int pkt_channel_send(pkt_channel_state_t* state, void* data, size_t size) {
    // add the header
    uint16_t* header = (uint16_t*)data - 1;
    *header = (uint16_t)size;
    return state->interface.send_function(state->interface.lower_state,
                                          header, size + sizeof(uint16_t));
}

void pkt_channel_close(pkt_channel_state_t* state) {
    state->interface.close_function(state->interface.lower_state);
}

size_t  pkt_channel_header_size() {
    return 2;
}
