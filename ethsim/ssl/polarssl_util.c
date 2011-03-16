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

void rsa_set_public_key(rsa_context* ctx, const char* pub_hex) {
    mpi_init(&ctx->E, NULL);
    mpi_read_string(&ctx->E, 16, pub_hex);
}

void rsa_set_private_key(rsa_context* ctx, const char* priv_hex) {
    mpi_init(&ctx->D, NULL);
    mpi_read_string(&ctx->D, 16, priv_hex);
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


/*
 * Allocation and free functions of AES context (for use in python).
 */

aes_context* alloc_aes_context() {
    return (aes_context*)malloc(sizeof(aes_context));
}

void free_aes_context(aes_context* ctx) {
    free(ctx);
}
