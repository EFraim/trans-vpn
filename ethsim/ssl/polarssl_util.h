#ifndef _POLARSLL_UTIL_H_
#define _POLARSLL_UTIL_H_

#include <polarssl/rsa.h>

void rsa_set_public_modulus(rsa_context* ctx, const char* mod_hex);
void rsa_set_public_key(rsa_context* ctx, const char* pub_hex);
void rsa_set_private_key(rsa_context* ctx, const char* priv_hex);
void rsa_get_public_modulus(rsa_context* ctx, char* mod_hex, int* mod_len);
void rsa_get_public_key(rsa_context* ctx, char* pub_hex, int* pub_len);
void rsa_get_private_key(rsa_context* ctx, char* priv_hex, int* priv_len);
int rsa_gen_keypair(rsa_context* ctx, int nbits);

#endif

