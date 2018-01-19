#ifndef DTLS_SEND_H_INCLUDED
#define DTLS_SEND_H_INCLUDED

#include "wolfssl_include_util.h"

extern WOLFSSL_CTX* send_ctx;
extern int send_cleanup;

int dtls_send(byte* cipherText, int flag);

#endif
