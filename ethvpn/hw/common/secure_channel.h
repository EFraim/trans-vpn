#ifndef _SECURE_CHANNEL_H_
#define _SECURE_CHANNEL_H_

#include "channel_interface.h"

#include <stdint.h>

#include <polarssl/aes.h>
#include <polarssl/rsa.h>

#define AES_BLOCK_SIZE    16
#define AES_KEY_SIZE      16
#define RSA_BLOCK_SIZE    128
#define RSA_KEY_SIZE      128
#define CHALLENGE_SIZE    16

/*
 * State of the secure channel's authentication.
 * 
 * SC_STATE_INITIAL     - Initial state.
 * SC_STATE_ID_SENT     - Client's id information was sent, and now waiting for a server's
 *                        response 
 * SC_STATE_ESTABLISHED - Server's response was received and all the following packets
 *                        are data packets.
 */
enum SecureChannelAuthState {
    SECURE_CHANNEL_INITIAL,
    SECURE_CHANNEL_ID_SENT,
    SECURE_CHANNEL_ESTABLISHED
};

// can be safely increased if more layers are added
#define MAX_HEADERS_SIZE    2

/*
 * Authentication information.
 */
typedef struct {
    const uint8_t* CLIENT_PUBLIC_MODULUS;
    const uint8_t* CLIENT_PUBLIC_KEY;
    const uint8_t* CLIENT_PRIVATE_KEY;
    const uint8_t* SERVER_PUBLIC_MODULUS;
    const uint8_t* SERVER_PUBLIC_KEY;
} secure_channel_auth_info_t;

/*
 * State of the secure channel.
 */
typedef struct {
    // interface to other layers
    channel_interface_t         interface;
    
    // authentication state
    enum SecureChannelAuthState auth_state;
    
    // context of RSA encryption
    rsa_context                 rsa_ctx;
    
    // context of AES encryption
    aes_context                 aes_encrypt_ctx;
    
    // context of AES decryption
    aes_context                 aes_decrypt_ctx;
    
    // challenge string sent to server
    uint8_t                     challenge[CHALLENGE_SIZE];
    
    // buffer for outgoing messages generated by authentication protocol
    uint8_t                     auth_send_buffer[MAX_HEADERS_SIZE + 
                                                 RSA_KEY_SIZE +
                                                 CHALLENGE_SIZE];
                            
    // pointer to auth_send_buffer after the lower headers (actually used buffer) 
    uint8_t*                    auth_send_data;
    
    // authentication information
    secure_channel_auth_info_t  auth_info;
} secure_channel_state_t;


// initializes the protocol state ("reset" still must be called after this call)
void    secure_channel_init(secure_channel_state_t* state, channel_interface_t* interface,
                            secure_channel_auth_info_t* auth_info);

// resets the state to initial
void    secure_channel_reset(secure_channel_state_t* state);
void    secure_channel_input_packet(secure_channel_state_t* state, void* data, size_t size);
void    secure_channel_poll(secure_channel_state_t* state);
int     secure_channel_send(secure_channel_state_t* state, void* data, size_t size);
void    secure_channel_close(secure_channel_state_t* state);

size_t  secure_channel_header_size();

#endif
