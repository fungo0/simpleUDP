#ifndef NONBLOCKING_DTLS_H_INCLUDED
#define NONBLOCKING_DTLS_H_INCLUDED

#include "wolfssl_include_util.h"
#include "include_util.h"

enum {
    TEST_SELECT_FAIL,
    TEST_TIMEOUT,
    TEST_RECV_READY,
    TEST_ERROR_READY
};

WOLFSSL* nonBlock_connect(int listen_fd, struct timeval timeout, int currTimeout, WOLFSSL* ssl);

WOLFSSL* send_nonBlock_connect(WOLFSSL* ssl);

#endif
