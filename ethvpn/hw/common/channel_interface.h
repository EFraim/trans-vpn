#ifndef _CHANNEL_INTERFACE_H_
#define _CHANNEL_INTERFACE_H_

#include <stddef.h>

// interface functions to upper layer

typedef void  (*channel_input_function_t)(void* state, void* data, size_t size);
typedef void  (*channel_poll_function_t)(void* state);

// interface functions to lower layer
typedef int   (*channel_send_function_t)(void* state, void* data, size_t size);
typedef void  (*channel_close_function_t)(void* state);

typedef struct {
    
    
    // communication interface to upper protocol
    
    // state of upper protocol
    void*                           upper_state;
    // function for passing incoming packet
    channel_input_function_t        input_function;
    // function for polling (to initiate outgoing packets)
    channel_poll_function_t         poll_function;
    
    
    // communication interface to lower protocol
    
    // total header size of all lower protocols
    size_t                          header_size;
    
    // state of lower protocol
    void*                           lower_state;
    // function for sending packets
    channel_send_function_t         send_function;
    // function for closing communication
    channel_close_function_t        close_function;
    
} channel_interface_t;

#endif
