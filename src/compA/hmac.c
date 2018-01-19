#include "include/hmac.h"
#include "include/wolfssl_include_util.h"
#include "include/string_util.h"
#include "include/generate_key_util.h"

/* password need initial key need NULL terminator for KeyGen */
byte* hmac(byte* input, char option, char* password) {
    Hmac hmac;
    int key_size = 256;             // sha256
    byte* key = malloc(key_size);
    memset(key, 0, key_size);
    memcpy(key, password, strlen(password));
    key[strlen(password)+1] = 0;
    byte* output = 0;

    /* encrypt */
    if(option == 'e') {
        if((output = HMAC_Encrypt(&hmac, key, key_size, input)) == NULL) {
            return NULL;
        }
    } else if(option == 'd') {
        if((output = HMAC_Decrypt(&hmac, key, key_size, input)) == NULL) {
            return NULL;
        }
    }

    return output;
}

byte* HMAC_Encrypt(Hmac* hmac, byte* key, int size, byte* input) {
    RNG rng;
    byte* cloned_input;
    byte salt[SALT_SIZE] = {0};
    byte* hmacDigest = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest, 0, SHA256_DIGEST_SIZE);
    byte* concat_output = malloc(SALT_SIZE+SHA256_DIGEST_SIZE);

    int padCounter = 0;
    int ret = 0;
    int inputLength = strlen(input);
    int length = inputLength;
    /* !! excluding NULL terminator !! */
    cloned_input = malloc(length);
    memset(cloned_input, 0, length);
    memcpy(cloned_input, input, strlen(input));

    /* not sure if need or not for key generation */
    while (length % HMAC_BLOCK_SIZE != 0) {
        length++;
        padCounter++;
    }

    if(wc_InitRng(&rng) != 0) {
        return NULL;
    }

    /* stretches key to fit size */
    if((ret = GenerateKey(&rng, key, size, (byte*)&salt, padCounter)) != 0) {
        return NULL;
    }

    /* sets key */
    if((ret = wc_HmacSetKey(hmac, SHA256, key, size)) != 0) {
        return NULL;
    }

    /* !! excluding NULL terminator in cloned_input !! */
    if((ret = wc_HmacUpdate(hmac, cloned_input, inputLength)) != 0) {
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac, hmacDigest)) != 0) {
        return NULL;
    }

    memcpy(concat_output, salt, SALT_SIZE);
    memcpy(concat_output+SALT_SIZE, hmacDigest, SHA256_DIGEST_SIZE);

    memset(key, 0, size);
    free(key);
    free(hmacDigest);
    wc_FreeRng(&rng);
    free(cloned_input);

    return concat_output;
}

char* HMAC_Decrypt(Hmac* hmac, byte* key, int size, byte* input) {
    RNG rng;
    byte* cloned_input;
    byte salt[SALT_SIZE] = {0};
    char* validate_result;
    int i = 0;
    int ret = 0;
    int inputLength = SALT_SIZE+SHA256_DIGEST_SIZE;
    int length = inputLength;
    /* !! no NULL terminator !! */
    cloned_input = malloc(length);
    memset(cloned_input, 0, length);
    memcpy(cloned_input, input, length);

    if(wc_InitRng(&rng) != 0) {
        return NULL;
    }

    /* finds salt from input message */
    for (i = 0; i < SALT_SIZE; i++) {
        salt[i] = input[i];
    }

    /* replicates old key if keys match */
    if((ret = wc_PBKDF2(key, key, strlen((const char*)key), (byte*)&salt, SALT_SIZE, 4096, size, SHA256)) != 0) {
        perror("wc_PBKDF2");
        return NULL;
    }

    /* sets key */
    if((ret = wc_HmacSetKey(hmac, SHA256, key, size)) != 0) {
        return NULL;
    }

    /* change length to remove salt/iv block from being decrypted */
    length -= SALT_SIZE;
    /* shifts message: ignores salt/iv on message*/
    for (i = 0; i < length; i++) {
        cloned_input[i] = input[i + (SALT_SIZE)];
    }
    cloned_input[i] = 0;

    validate_result = validate_HMAC(hmac, cloned_input, length);

    if(validate_result == NULL) {
        return NULL;
    }

    memset(key, 0, size);
    free(key);
    free(cloned_input);
    wc_FreeRng(&rng);

    return validate_result;
}

char* validate_HMAC(Hmac* hmac, byte* input, int input_length) {
    int ret = 0;
    byte *send = "SEND";
    byte *request = "REQUEST";
    byte *check = "CHECK";
    byte *refill = "REFILL";
    byte *insufficient = "INSUFFICIENT";

    Hmac* hmac1 = malloc(sizeof(*hmac));
    Hmac* hmac2 = malloc(sizeof(*hmac));
    Hmac* hmac3 = malloc(sizeof(*hmac));
    Hmac* hmac4 = malloc(sizeof(*hmac));
    Hmac* hmac5 = malloc(sizeof(*hmac));
    copyByte(hmac1, hmac, sizeof(*hmac));
    copyByte(hmac2, hmac, sizeof(*hmac));
    copyByte(hmac3, hmac, sizeof(*hmac));
    copyByte(hmac4, hmac, sizeof(*hmac));
    copyByte(hmac5, hmac, sizeof(*hmac));
    byte* hmacDigest1 = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest1, 0, SHA256_DIGEST_SIZE);
    byte* hmacDigest2 = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest2, 0, SHA256_DIGEST_SIZE);
    byte* hmacDigest3 = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest3, 0, SHA256_DIGEST_SIZE);
    byte* hmacDigest4 = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest4, 0, SHA256_DIGEST_SIZE);
    byte* hmacDigest5 = malloc(SHA256_DIGEST_SIZE);
    memset(hmacDigest5, 0, SHA256_DIGEST_SIZE);
    char* output;

    /* hash send */
    if((ret = wc_HmacUpdate(hmac1, send, 4)) != 0) {
        perror("wc_HmacUpdate1");
        printf("err: %d\n", ret);
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac1, hmacDigest1)) != 0) {
        perror("wc_HmacFinal1");
        printf("err: %d\n", ret);
        return NULL;
    }

    /* hash request */
    if((ret = wc_HmacUpdate(hmac2, request, 7)) != 0) {
        perror("wc_HmacUpdate2");
        printf("err: %d\n", ret);
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac2, hmacDigest2)) != 0) {
        perror("wc_HmacFinal2");
        printf("err: %d\n", ret);
        return NULL;
    }

    /* hash check */
    if((ret = wc_HmacUpdate(hmac3, check, 5)) != 0) {
        perror("wc_HmacUpdate3");
        printf("err: %d\n", ret);
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac3, hmacDigest3)) != 0) {
        perror("wc_HmacFinal3");
        printf("err: %d\n", ret);
        return NULL;
    }

    /* hash refill */
    if((ret = wc_HmacUpdate(hmac4, refill, 6)) != 0) {
        perror("wc_HmacUpdate4");
        printf("err: %d\n", ret);
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac4, hmacDigest4)) != 0) {
        perror("wc_HmacFinal4");
        printf("err: %d\n", ret);
        return NULL;
    }

    /* hash insufficient */
    if((ret = wc_HmacUpdate(hmac5, insufficient, 12)) != 0) {
        perror("wc_HmacUpdate5");
        printf("err: %d\n", ret);
        return NULL;
    }

    if((ret = wc_HmacFinal(hmac5, hmacDigest5)) != 0) {
        perror("wc_HmacFinal5");
        printf("err: %d\n", ret);
        return NULL;
    }

    if(cmpByte(input, hmacDigest1, input_length) == 0) {
        output = malloc(strlen(send)+1);
        memset(output, 0, strlen(send)+1);
        copyByte(output, send, strlen(send)+1);
    } else if(cmpByte(input, hmacDigest2, input_length) == 0) {
        output = malloc(strlen(request)+1);
        memset(output, 0, strlen(request)+1);
        copyByte(output, request, strlen(request)+1);
    } else if(cmpByte(input, hmacDigest3, input_length) == 0) {
        output = malloc(strlen(check)+1);
        memset(output, 0, strlen(check)+1);
        copyByte(output, check, strlen(check)+1);
    } else if(cmpByte(input, hmacDigest4, input_length) == 0) {
        output = malloc(strlen(refill)+1);
        memset(output, 0, strlen(refill)+1);
        copyByte(output, refill, strlen(refill)+1);
    } else if(cmpByte(input, hmacDigest5, input_length) == 0) {
        output = malloc(strlen(insufficient)+1);
        memset(output, 0, strlen(insufficient)+1);
        copyByte(output, insufficient, strlen(insufficient)+1);
    } else {
        output = NULL;
    }

    free(hmac1);
    free(hmac2);
    free(hmac3);
    free(hmac4);
    free(hmac5);
    free(hmacDigest1);
    free(hmacDigest2);
    free(hmacDigest3);
    free(hmacDigest4);
    free(hmacDigest5);
    return output;
}
