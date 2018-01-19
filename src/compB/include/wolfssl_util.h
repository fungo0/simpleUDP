#ifndef WOLFSSL_UTIL_H_INCLUDED
#define WOLFSSL_UTIL_H_INCLUDED

#include "udp_include_util.h"
#include "wolfssl_include_util.h"

#define MAXLINE   4096

WOLFSSL_CTX* get_send_ctx();
WOLFSSL_CTX* get_recv_ctx();
WOLFSSL* get_send_ssl(int *send_fd, WOLFSSL_CTX *target_ctx, struct sockaddr_in *this_addr, struct sockaddr_in *target_addr);
WOLFSSL* get_recv_ssl(int recv_fd, int listen_fd, struct timeval timeout, int currTimeout, WOLFSSL_CTX *target_ctx);
int wolfssl_send_buf(void *buf, int buflen, WOLFSSL* ssl);
int wolfssl_send_stream(FILE* input_stream, WOLFSSL* ssl);
int wolfssl_recv(WOLFSSL* ssl, void* buff, int buff_len);

#endif
