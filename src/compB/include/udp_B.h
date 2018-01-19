#ifndef UDP_B_H_INCLUDED
#define UDP_B_H_INCLUDED

#include "wolfssl_include_util.h"
#include "udp_include_util.h"
/* global variables */
extern WOLFSSL_CTX* recv_ctx;
extern struct sockaddr_in A_addr;
extern struct sockaddr_in B_addr;
extern int cleanup;
extern int INVENTORY;

#endif
