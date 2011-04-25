#include "secure_channel.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"
#include <log.h>
#include <polarssl_util.h>

#include <uip/clock.h>

#define MAX_AES_OVERHEAD 16

static void generate_random_buffer(uint8_t* buffer, size_t size) {
    srand(clock_time());
    size_t i;
    for (i = 0; i < size; ++i) {
        buffer[i] = rand() & 0xff;
    }
}


void secure_channel_init(secure_channel_state_t* state, channel_interface_t* interface,
                         secure_channel_auth_info_t* auth_info)
{
    state->interface = *interface;
    state->auth_info = *auth_info;
    state->auth_send_data = state->auth_send_buffer + interface->header_size;
}

void secure_channel_reset(secure_channel_state_t* state) {
    state->auth_state = SECURE_CHANNEL_INITIAL;
}

static int send_server_response(secure_channel_state_t* state, const char* response) {
    strcpy((char*)state->auth_send_data, response);
    return state->interface.send_function(state->interface.lower_state,
                                          state->auth_send_data, strlen(response));
}

void secure_channel_input_packet(secure_channel_state_t* state, void* data, size_t size) {

    uint8_t* pkt_data = (uint8_t*)data;
    switch (state->auth_state) {
        case SECURE_CHANNEL_ID_SENT:
            
            // check whether server's response was negative
            if (pkt_data[0] != 'y') {
                if (pkt_data[0] != 'n') {
                    LOG_ERROR("Invalid server's response!");
                }
                LOG_INFO("Secure channel authorization failed: %s!", pkt_data + 1);
                secure_channel_close(state);
                return;
            }
            
            // verify response's length
            if (size != 2 * RSA_BLOCK_SIZE + 1) {
                LOG_ERROR("Invalid response length from server");
                secure_channel_close(state);
                return;
            }
            
            // decrypt the server's message
            rsa_init(&state->rsa_ctx, RSA_PKCS_V15, 0);
            rsa_set_public_modulus(&state->rsa_ctx, state->auth_info.CLIENT_PUBLIC_MODULUS);
            rsa_set_private_key(&state->rsa_ctx, state->auth_info.CLIENT_PRIVATE_KEY);

            uint8_t signed_data[RSA_BLOCK_SIZE];
            size_t  output_size = sizeof(signed_data);
            rsa_decrypt(&state->rsa_ctx, pkt_data + 1, size - 1,
                        signed_data, &output_size, RSA_PRIVATE);
            
            if (output_size != sizeof(signed_data)) {
                LOG_ERROR("Invalid server's response - signed data's length is too short");
                secure_channel_close(state);
                return;
            }

            // check whether server signed the message with its private key
            rsa_free(&state->rsa_ctx);
            rsa_init(&state->rsa_ctx, RSA_PKCS_V15, 0);
            rsa_set_public_modulus(&state->rsa_ctx, state->auth_info.SERVER_PUBLIC_MODULUS);
            rsa_set_public_key(&state->rsa_ctx, state->auth_info.SERVER_PUBLIC_KEY);
            
            uint8_t* server_response = pkt_data;
            output_size = CHALLENGE_SIZE + AES_KEY_SIZE;
            rsa_decrypt(&state->rsa_ctx, signed_data, sizeof(signed_data),
                        server_response, &output_size,
                        RSA_PUBLIC);
            
            if (output_size != CHALLENGE_SIZE + AES_KEY_SIZE ||
                memcmp(server_response, state->challenge, CHALLENGE_SIZE) != 0)
            {
                LOG_INFO("Server failed to sign!");
                
                send_server_response(state, "nInvalid signature");
                secure_channel_close(state);
            } else {
                LOG_INFO("Secure channel established");

                state->auth_state = SECURE_CHANNEL_ESTABLISHED;
                
                uint8_t* aes_key = server_response + CHALLENGE_SIZE;
                aes_setkey_enc(&state->aes_encrypt_ctx, aes_key, AES_KEY_SIZE * 8);
                aes_setkey_dec(&state->aes_decrypt_ctx, aes_key, AES_KEY_SIZE * 8);

                send_server_response(state, "y");
            }
            
            break;
            
        case SECURE_CHANNEL_ESTABLISHED:
            // empty statement, otherwise GCC won't compile declaration after "case" label...
            ;
            
            unsigned char block[AES_BLOCK_SIZE];

            // decrypt message block by block
            size_t i;
            for (i = 0; i < size - 1; i += AES_BLOCK_SIZE) {
                aes_crypt_ecb(&state->aes_decrypt_ctx, AES_DECRYPT, pkt_data + i, block);
                memcpy(pkt_data + i, block, AES_BLOCK_SIZE);
            }
            
            // determine real packet's length
            uint8_t  pad_size = pkt_data[size - 1];
            if (pad_size > AES_BLOCK_SIZE - 1) {
                LOG_ERROR("Invalid packet - padding length is above block size");
                secure_channel_close(state);
            }
            uint16_t payload_size = size - 1 - pad_size;
            
            state->interface.input_function(state->interface.upper_state,
                                            pkt_data, payload_size);
        
            break;
        default:
            LOG_ERROR("Should not receive packets in this state");
            break;
    }
}

void secure_channel_poll(secure_channel_state_t* state) {
    
    switch (state->auth_state) {
        case SECURE_CHANNEL_INITIAL:
            // generate random challenge string and send it together with client's
            // public key
            generate_random_buffer(state->challenge, CHALLENGE_SIZE);
            
            memset(state->auth_send_data, 0, RSA_HEX_KEY_SIZE * 2 + CHALLENGE_SIZE);
            strcpy((char*)state->auth_send_data, state->auth_info.CLIENT_PUBLIC_MODULUS);
            strcpy((char*)state->auth_send_data + RSA_HEX_KEY_SIZE, state->auth_info.CLIENT_PUBLIC_KEY);
            memcpy(state->auth_send_data + RSA_HEX_KEY_SIZE * 2, state->challenge, CHALLENGE_SIZE);
            
            state->interface.send_function(state->interface.lower_state, state->auth_send_data,
                                           RSA_HEX_KEY_SIZE * 2 + CHALLENGE_SIZE);

            state->auth_state = SECURE_CHANNEL_ID_SENT;
            break;
            
        case SECURE_CHANNEL_ESTABLISHED:
            
            state->interface.poll_function(state->interface.upper_state);
            
            break;
        default:
            break;
    }
}

int secure_channel_send(secure_channel_state_t* state, void* data, size_t size) {
    uint8_t* plain = (uint8_t*)data;
    uint8_t* cipher = plain - MAX_AES_OVERHEAD;
    uint8_t block[AES_BLOCK_SIZE];

    // encrypt the message and add one byte indicating number of unused bytes in last block
    size_t i;
    for (i = 0; i < size; i += AES_BLOCK_SIZE) {
        aes_crypt_ecb(&state->aes_encrypt_ctx, AES_ENCRYPT, plain + i, block);
        memcpy(cipher + i, block, AES_BLOCK_SIZE);
    }

    size_t cipher_size = (size + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE * AES_BLOCK_SIZE;
    cipher[cipher_size] = cipher_size - size;
    
    return state->interface.send_function(state->interface.lower_state,
                                          cipher, cipher_size + 1);
}

void secure_channel_close(secure_channel_state_t* state) {
    state->interface.close_function(state->interface.lower_state);
}

size_t secure_channel_header_size() {
    return MAX_AES_OVERHEAD;
}
