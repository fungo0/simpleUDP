#ifndef ANCILLARY_DATA_H_INCLUDED
#define ANCILLARY_DATA_H_INCLUDED

#include "udp_include_util.h"

/* linux */
#if defined IP_PKTINFO
# define DSTADDR_SOCKOPT IP_PKTINFO
# define DSTADDR_DATASIZE (CMSG_SPACE(sizeof(struct in_pktinfo)))
# define get_source_addr(x) (((struct in_pktinfo *)(CMSG_DATA(x)))->ipi_addr)
#else
# error "can't determine socket option"
#endif

/* ancillary data - IPv4 recvdstaddr */
typedef union _ancillary_u {
    struct cmsghdr cmsg;
    char from_ip[DSTADDR_DATASIZE];
} ancillary_t;

#endif
