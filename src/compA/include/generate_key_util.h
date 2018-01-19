#ifndef GENERATE_KEY_UTIL_H_INCLUDED
#define GENERATE_KEY_UTIL_H_INCLUDED

#include "include_util.h"
#include "wolfssl_include_util.h"

#define SALT_SIZE 8

int GenerateKey(RNG* rng, byte* key, int size, byte* salt, int pad);

#endif
