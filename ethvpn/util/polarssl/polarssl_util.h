#ifndef _POLARSLL_UTIL_H_
#define _POLARSLL_UTIL_H_

#include <stdint.h>

#include <polarssl/rsa.h>

void rsa_set_public_modulus(rsa_context* ctx, const char* mod_hex);
void rsa_set_public_key(rsa_context* ctx, const char* pub_hex);
void rsa_set_private_key(rsa_context* ctx, const char* priv_hex);

void rsa_get_public_modulus(rsa_context* ctx, char* mod_hex, int* mod_len);
void rsa_get_public_key(rsa_context* ctx, char* pub_hex, int* pub_len);
void rsa_get_private_key(rsa_context* ctx, char* priv_hex, int* priv_len);

int rsa_gen_keypair(rsa_context* ctx, int nbits);

#define RSA_ERROR_SMALL_OUTPUT  1
#define RSA_ERROR_BAD_INPUT     2

/*
 * The following functions implement encryption/decryption of
 * input data of arbitrary length (without limitations caused
 * by RSA).
 * 
 * The assumption is that RSA's key size is a multiple of 8
 * (so that MSB of the key's first byte is 1)
 * 
 * Input data is divided into blocks of size at most "key_size - 2" (key_size in bytes).
 * Each block is padded in the beginning with "uint16_t" representing block's size.
 * Assuming that block's size is below 2**15, this assures that padded block
 * will have its MSB equal to zero, thus it will be valid input for RSA.
 */

size_t rsa_get_encryption_size(rsa_context* ctx, size_t input_size);
size_t rsa_get_decryption_size(rsa_context* ctx, size_t input_size);

int rsa_encrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode);

int rsa_decrypt(rsa_context* ctx, const uint8_t* input, size_t input_size,
                uint8_t* output, size_t* output_size, int mode);


#endif

