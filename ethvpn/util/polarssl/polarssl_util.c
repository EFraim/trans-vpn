/*
 * Contains conveniece functions for python,
 * and some functionality that is not present in polarssl API.
 */

#include "polarssl_util.h"

#include <polarssl/aes.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Allocation and free functions of RSA context (for use in python).
 */

rsa_context* alloc_rsa_context() {
    return (rsa_context*)malloc(sizeof(rsa_context));
}

void free_rsa_context(rsa_context* ctx) {
    free(ctx);
}

/*
 * Interface for setting public/private keys.
 * Doesn't present in polarssl for some reason...
 */

void rsa_set_public_modulus(rsa_context* ctx, const char* mod_hex) {
    mpi_init(&ctx->N, NULL);
    ctx->len = strlen(mod_hex) / 2;
    mpi_read_string(&ctx->N, 16, mod_hex);
}

void rsa_set_public_modulus_binary(rsa_context* ctx, const unsigned char* buf, size_t len) {
	mpi_init(&ctx->N, NULL);
    ctx->len = len;
    mpi_read_binary(&ctx->N, buf, len);
}

void rsa_set_public_key(rsa_context* ctx, const char* pub_hex) {
    mpi_init(&ctx->E, NULL);
    mpi_read_string(&ctx->E, 16, pub_hex);
}

void rsa_set_public_key_binary(rsa_context* ctx, const unsigned char* buf, size_t len) {
    mpi_init(&ctx->E, NULL);
    mpi_read_binary(&ctx->E, buf, len);
}

void rsa_set_private_key(rsa_context* ctx, const char* priv_hex) {
    mpi_init(&ctx->D, NULL);
    mpi_read_string(&ctx->D, 16, priv_hex);
}

void rsa_set_private_key_binary(rsa_context* ctx, const unsigned char* buf, size_t len) {
    mpi_init(&ctx->D, NULL);
    mpi_read_binary(&ctx->D, buf, len);
}


void rsa_get_public_modulus(rsa_context* ctx, char* mod_hex, int* mod_len) {
    mpi_write_string(&ctx->N, 16, mod_hex, mod_len);
}

void rsa_get_public_key(rsa_context* ctx, char* pub_hex, int* pub_len) {
    mpi_write_string(&ctx->E, 16, pub_hex, pub_len);
}

void rsa_get_private_key(rsa_context* ctx, char* priv_hex, int* priv_len) {
    mpi_write_string(&ctx->D, 16, priv_hex, priv_len);
}


static int myrand(void* unused) {
    return rand();
}

// more convenient interface of key generation (uses rand() for random generation)
int rsa_gen_keypair(rsa_context* ctx, int nbits) {
    srand(time(NULL));
    return rsa_gen_key(ctx, myrand, NULL, nbits, 65537);
}

#define DATA_SIZE_WIDTH 2
#define MIN(a,b) ((a) < (b) ? (a) : (b))

size_t rsa_get_encryption_size(rsa_context* ctx, size_t input_size) {
    size_t block_size = ctx->len;
    size_t input_block_size = block_size - DATA_SIZE_WIDTH;
    size_t nblocks = (input_size + input_block_size - 1) / input_block_size;
    return block_size * nblocks;
}

size_t rsa_get_decryption_size(rsa_context* ctx, size_t input_size) {
    size_t block_size = ctx->len;
    size_t output_block_size = block_size - DATA_SIZE_WIDTH;
    size_t nblocks = input_size / block_size;
    return output_block_size * nblocks;
}

int rsa_encrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode)
{
    size_t block_size = ctx->len;
    size_t input_block_size = block_size - DATA_SIZE_WIDTH;
    size_t nblocks = (input_size + input_block_size - 1) / input_block_size;
    
    // dynamic allocation on stack... (supported by GCC)
    uint8_t input_buffer[block_size];
    
    // check if there is enough space in output
    if (block_size * nblocks > *output_size) {
        return RSA_ERROR_SMALL_OUTPUT;
    }
    
    *output_size = 0;
    while (input_size > 0) {
        size_t current_size = MIN(input_size, input_block_size);
        memcpy(input_buffer + block_size - current_size, input, current_size);
        *(uint16_t*)input_buffer = (uint16_t)current_size;

        if (mode == RSA_PUBLIC) {
            rsa_public(ctx, input_buffer, output);
        } else {
            rsa_private(ctx, input_buffer, output);
        }
        
        input += current_size;
        input_size -= current_size;
        output += block_size;
        (*output_size) += block_size;
    }
    
    return 0;
}

int rsa_decrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode)
{
    size_t block_size = ctx->len;
    size_t output_block_size = block_size - DATA_SIZE_WIDTH;
    
    // dynamic allocation on stack... (supported by GCC)
    uint8_t output_buffer[block_size];
    
    size_t real_size = 0;
    while (input_size > 0) {
        if (mode == RSA_PUBLIC) {
            rsa_public(ctx, input, output_buffer);
        } else {
            rsa_private(ctx, input, output_buffer);
        }
        
        size_t current_size = *(uint16_t*)output_buffer;
        if (current_size > output_block_size) {
            return RSA_ERROR_BAD_INPUT;
        }
        if (real_size + current_size > *output_size) {
            return RSA_ERROR_SMALL_OUTPUT;
        }
        memcpy(output, output_buffer + block_size - current_size, current_size);
        
        input += block_size;
        input_size -= block_size;
        output += current_size;
        real_size += current_size;
    }
    
    *output_size = real_size;
    
    return 0;
}


/*
 * Allocation and free functions of AES context (for use in python).
 */

aes_context* alloc_aes_context() {
    return (aes_context*)malloc(sizeof(aes_context));
}

void free_aes_context(aes_context* ctx) {
    free(ctx);
}
