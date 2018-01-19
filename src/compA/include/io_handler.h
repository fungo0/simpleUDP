#ifndef IO_HANDLER_H_INCLUDED
#define IO_HANDLER_H_INCLUDED

#include "wolfssl_include_util.h"

int encrypt_send(char **args);
byte* encrypt_react(byte* target_command, char* target_value);
byte* decrypt_react(byte* cipher_text, int end_flag);

#endif
