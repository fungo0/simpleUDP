#include "include/generate_key_util.h"
#include "include/wolfssl_include_util.h"

int GenerateKey(RNG* rng, byte* key, int size, byte* salt, int pad) {
    int ret;

    if((ret = wc_RNG_GenerateBlock(rng, salt, SALT_SIZE)) != 0) {
        perror("wc_RNG_GenerateBlock");
        return -1020;
    }

    if (pad == 0) {
        salt[0] = 0;
    }

    /* stretches key */
    if((ret = wc_PBKDF2(key, key, strlen((const char*)key), salt, SALT_SIZE, 4096, size, SHA256)) != 0) {
        perror("wc_PBKDF2");
        return -1030;
    }

    return 0;
}
