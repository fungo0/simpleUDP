#include "include/io_handler.h"
#include "include/include_util.h"
#include "include/aes_encrypt.h"
#include "include/hmac.h"
#include "include/wolfssl_include_util.h"
#include "include/string_util.h"
#include "include/generate_key_util.h"
#include "include/dtls_send.h"
#include "include/udp_A.h"

int encrypt_send(char **args) {
    byte *send = "SEND";
    byte *request = "REQUEST";
    byte *check = "CHECK";
    byte *refill = "REFILL";
    int skip_flag = 0;

    byte* send_buf = malloc(SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
    memset(send_buf, 0, SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
    char option = 'e';
    char *command_pw = "comp3334";

    char *command = malloc(strlen(args[0])+1);
    memcpy(command, args[0], strlen(args[0])+1);         // !! already copied NULL terminator !!

    byte* command_output = hmac(command, option, command_pw);       // no null terminator
    memcpy(send_buf, command_output, SALT_SIZE+SHA256_DIGEST_SIZE);

    if(args[0] == NULL || cmpByte(refill, command, 6) == 0 || cmpByte(check, command, 5) == 0) {
        skip_flag = 1;
    }

    if(skip_flag != 1) {
        char *value_pw = malloc(strlen(args[0])+1);
        memcpy(value_pw, args[0], strlen(args[0])+1);       // !! already copied NULL terminator !!
        char *value = malloc(strlen(args[1])+1);
        memcpy(value, args[1], strlen(args[1])+1);          // !! already copied NULL terminator !!
        byte* value_output = aes(value, option, value_pw);
        memcpy(send_buf+SALT_SIZE+SHA256_DIGEST_SIZE, value_output, SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);

        free(value_pw);
        free(value);
        free(value_output);
    }

    if(cmpByte(send, command, 4) == 0) {
        INVENTORY -= str2num(args[1]);
        dtls_send(send_buf, 0);
        printf("SEND: %d sended\n", str2num(args[1]));
    } else {
        // if(cmpByte(request, command, 7) == 0) {
        //     printf("REQUEST: %d sended\n", str2num(args[1]));
        // }
        dtls_send(send_buf, 1);
    }

    free(send_buf);
    free(command);
    free(command_output);

    return 1;

}

byte* encrypt_react(byte* target_command, char* target_value) {
    byte *send = "SEND";
    byte *request = "REQUEST";
    byte *check = "CHECK";
    byte *refill = "REFILL";
    byte *insufficient = "INSUFFICIENT";
    char option = 'e';
    char *command_pw = "comp3334";
    char *command;
    byte* send_buf = malloc(SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
    memset(send_buf, 0, SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);

    if(memcmp(send, target_command, 4) == 0) {
        command = malloc(5);
        memset(command, 0, 5);
        memcpy(command, target_command, 4);
    } else if(memcmp(check, target_command, 5) == 0) {
        command = malloc(6);
        memset(command, 0, 6);
        memcpy(command, target_command, 5);
    } else if(memcmp(refill, target_command, 6) == 0) {
        command = malloc(7);
        memset(command, 0, 7);
        memcpy(command, target_command, 6);
    } else if(memcmp(request, target_command, 7) == 0) {
        command = malloc(8);
        memset(command, 0, 8);
        memcpy(command, target_command, 7);
    } else if(memcmp(insufficient, target_command, 12) == 0) {
        command = malloc(13);
        memset(command, 0, 13);
        memcpy(command, target_command, 12);
    }

    byte* command_output = hmac(command, option, command_pw);       // no null terminator
    memcpy(send_buf, command_output, SALT_SIZE+SHA256_DIGEST_SIZE);

    char *value = malloc(strlen(target_value)+1);
    memcpy(value, target_value, strlen(target_value)+1);          // !! already copied NULL terminator !!
    byte* value_output = aes(value, option, command);
    memcpy(send_buf+SALT_SIZE+SHA256_DIGEST_SIZE, value_output, SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);

    free(value);
    free(value_output);
    free(command);
    free(command_output);

    return send_buf;
}

byte* decrypt_react(byte* cipher_text, int end_flag) {
    byte *send = "SEND";
    byte *request = "REQUEST";
    byte *check = "CHECK";
    byte *refill = "REFILL";
    byte *insufficient = "INSUFFICIENT";
    char option = 'd';
    char *command_pw = "comp3334";
    int skip_flag = 0;
    int target_inventory = 0;
    char *value_pw;
    byte* value_cipher;
    value_cipher = malloc(SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
    char* value;
    byte* resent_cipher;

    byte* command_cipher = malloc(SALT_SIZE+SHA256_DIGEST_SIZE);
    memset(command_cipher, 0, SALT_SIZE+SHA256_DIGEST_SIZE);
    memcpy(command_cipher, cipher_text, SALT_SIZE+SHA256_DIGEST_SIZE);
    char *command = hmac(command_cipher, option, command_pw);
    value_pw = malloc(strlen(command)+1);

    if(command == NULL){
        skip_flag = 1;
        printf("error in decrypting command\n");
    }
    if(end_flag == 0) {
        if(memcmp(check, command, 5) == 0 || memcmp(refill, command, 6) == 0) {
            skip_flag = 1;
        }
    }
    if(skip_flag != 1) {

        memcpy(value_pw, command, strlen(command)+1);       // !! already copied NULL terminator !!
        memset(value_cipher, 0, SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
        memcpy(value_cipher, cipher_text+SALT_SIZE+SHA256_DIGEST_SIZE, SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE);
        value = aes(value_cipher, option, value_pw);
        target_inventory = str2num(value);

        free(value);

    }

    if(cmpByte(send, command, 4) == 0) {
        INVENTORY += target_inventory;
        end_flag = 1;
        printf("SEND: %d received\n", target_inventory);
    }
    if(end_flag == 0) {
        if(cmpByte(check, command, 5) == 0) {
            target_inventory = INVENTORY;
        } else if(cmpByte(refill, command, 6) == 0) {
            target_inventory = 500;
            INVENTORY -= target_inventory;
            printf("Received refill request\nREFILL: 500 sended\n");
        } else if(cmpByte(request, command, 7) == 0) {
            if(target_inventory > INVENTORY) {
                command = (char *) realloc(command, 13);
                memset(command, 0, 13);
                memcpy(command, insufficient, 12);
                target_inventory = -1;
            } else {
                INVENTORY -= target_inventory;
                printf("REQUEST: %d sended\n", target_inventory);
            }
        }
    } else if(end_flag == 1) {
        if(cmpByte(check, command, 5) == 0) {
            printf("CompB currently have inventory: %d\n", target_inventory);
        } else if(cmpByte(refill, command, 6) == 0) {
            INVENTORY += target_inventory;
            printf("REFILL: 500 received\n");
        } else if(cmpByte(request, command, 7) == 0) {
            INVENTORY += target_inventory;
            printf("REQUEST: %d received\n", target_inventory);
        } else if((target_inventory == -1) && (cmpByte(insufficient, command, 12) == 0)) {
            printf("Cannot request, `CHECK` for no. in compB before `REQUEST`!\n");
        }
    }

    free(command_cipher);
    free(value_pw);
    free(value_cipher);

    if(end_flag == 0) {
        resent_cipher = encrypt_react(command, num2str(target_inventory));
        free(command);
        return resent_cipher;
    } else {
        free(command);
        return 0;
    }

}
