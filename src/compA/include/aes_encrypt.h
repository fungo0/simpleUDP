#ifndef AES_ENCRYPT_H_INCLUDED
#define AES_ENCRYPT_H_INCLUDED

#include "include_util.h"
#include "wolfssl_include_util.h"

byte* aes(byte* input, char option, char* password);
byte* AesEncrypt(Aes* aes, byte* key, int size, byte* input);
byte* AesDecrypt(Aes* aes, byte* key, int size, byte* input);

#endif
