#ifndef HMAC_H_INCLUDED
#define HMAC_H_INCLUDED

#include "include_util.h"
#include "wolfssl_include_util.h"

#define MAX_HMAC_DIGEST_SIZE 50
byte* hmac(byte* input, char option, char* password);
byte* HMAC_Encrypt(Hmac* hmac, byte* key, int size, byte* input);
char* HMAC_Decrypt(Hmac* hmac, byte* key, int size, byte* input);
char* validate_HMAC(Hmac* hmac, byte* input, int input_length);

#endif
