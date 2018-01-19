#include "include/aes_encrypt.h"
#include "include/wolfssl_include_util.h"
#include "include/string_util.h"
#include "include/generate_key_util.h"

/* AES_BLOCK_SIZE 16 bytes */
/* !! both encrypt decrypt excludeing NULL terminator for input !! */

byte* aes(byte* input, char option, char* password) {
    Aes aes;
    int key_size = 192;             // aes key length, must be 128 / 192 / 256
    byte* key = malloc(key_size);       // aes key to be generated
    memset(key, 0, key_size);
    memcpy(key, password, strlen(password)+1);      // !! already have NULL terminator !!
    byte* output;

    /* encrypt */
    if(option == 'e') {
        if((output = AesEncrypt(&aes, key, key_size, input)) == NULL) {
            return NULL;
        }
    } else if(option == 'd') {
        if((output = AesDecrypt(&aes, key, key_size, input)) == NULL) {
            return NULL;
        }
    }

    return output;
}

byte* AesEncrypt(Aes* aes, byte* key, int size, byte* input) {
    RNG rng;
    byte iv[AES_BLOCK_SIZE];
    byte* expanded_input;
    byte* output;
    byte salt[SALT_SIZE] = {0};

    int i = 0;
    int ret = 0;
    int padCounter = 0;
    int inputLength = strlen(input);        // excluding NULL terminator
    int length = inputLength;
    /* pads the length until it evenly matches a block / increases pad number*/
    while (length % AES_BLOCK_SIZE != 0) {
        length++;
        padCounter++;
    }
    byte* concat_output = malloc(SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);

    /* no NULL terminator */
    expanded_input = malloc(length);
    memset(expanded_input, 0, length);
    memcpy(expanded_input, input, strlen(input));
    output = malloc(length);        // input length == output length in aes
    memset(output, 0, length);


    if(wc_InitRng(&rng) != 0) {
        return NULL;
    }

    /* padds the added characters with the number of pads */
    for (i = inputLength; i < length; i++) {
        expanded_input[i] = padCounter;
    }

    if((ret = wc_RNG_GenerateBlock(&rng, iv, AES_BLOCK_SIZE)) != 0) {
        perror("wc_RNG_GenerateBlock");
        return NULL;
    }

    /* stretches key to fit size */
    if((ret = GenerateKey(&rng, key, size, (byte*)&salt, padCounter)) != 0) {
        perror("GenerateKey");
        return NULL;
    }

    /* sets key */
    if((ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_ENCRYPTION)) != 0) {
        perror("wc_AesSetKey");
        return NULL;
    }

    /* encrypts the message to the ouput based on input length + padding */
    if((ret = wc_AesCbcEncrypt(aes, output, expanded_input, length)) != 0) {
        return NULL;
    }

    memcpy(concat_output, salt, SALT_SIZE);
    memcpy(concat_output+SALT_SIZE, iv, AES_BLOCK_SIZE);
    memcpy(concat_output+SALT_SIZE+AES_BLOCK_SIZE, output, length);

    memset(key, 0, size);
    free(key);
    free(expanded_input);
    free(output);
    wc_FreeRng(&rng);

    return concat_output;
}

byte* AesDecrypt(Aes* aes, byte* key, int size, byte* input) {
    RNG rng;
    byte iv[AES_BLOCK_SIZE];
    byte* local_input;
    byte* output;
    byte salt[SALT_SIZE] = {0};

    int i = 0;
    int ret = 0;

    int length = SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE;
    local_input = malloc(length);
    memset(local_input, 0, length);
    memcpy(local_input, input, length);     // no NULL terminator
    output = malloc(AES_BLOCK_SIZE);        // input length == output length in aes
    memset(output, 0, AES_BLOCK_SIZE);

    if(wc_InitRng(&rng) != 0) {
        perror("wc_InitRng");
        return NULL;
    }

    /* finds salt from input message */
    for (i = 0; i < SALT_SIZE; i++) {
        salt[i] = input[i];
    }

    /* finds iv from input message */
    for (i = SALT_SIZE; i < AES_BLOCK_SIZE + SALT_SIZE; i++) {
        iv[i - SALT_SIZE] = input[i];
    }

    /* replicates old key if keys match */
    if((ret = wc_PBKDF2(key, key, strlen((const char*)key), (byte*)&salt, SALT_SIZE, 4096, size, SHA256)) != 0) {
        perror("wc_PBKDF2");
        return NULL;
    }
    /* sets key */
    if((ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_DECRYPTION)) != 0) {
        perror("wc_AesSetKey");
        return NULL;
    }

    /* change length to remove salt/iv block from being decrypted */
    length -= (AES_BLOCK_SIZE + SALT_SIZE);

    /* shifts message: ignores salt/iv on message*/
    for (i = 0; i < length; i++) {
        local_input[i] = input[i + (AES_BLOCK_SIZE + SALT_SIZE)];
    }

    /* decrypts the message to output based on input length + padding*/
    if((ret = wc_AesCbcDecrypt(aes, output, local_input, length)) != 0) {
        perror("wc_AesCbcDecrypt");
        printf("err:%d\n", ret);
        return NULL;
    }

    /* reduces length based on number of padded elements */
    if (salt[0] != 0) {
        length -= output[length-1];
    }
    output[length] = 0;

    memset(key, 0, size);
    free(key);
    free(local_input);
    wc_FreeRng(&rng);

    return output;
}
