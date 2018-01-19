#ifndef CMSG_UTIL_H_INCLUDED
#define CMSG_UTIL_H_INCLUDED

#include "include_util.h"
#include "udp_include_util.h"

ssize_t udp_send(int socket, void *buf, int buflen, struct sockaddr_in *this_addr, struct sockaddr_in *target_addr);

#endif
