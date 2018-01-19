#ifndef UDP_UTIL_H_INCLUDED
#define UDP_UTIL_H_INCLUDED

#include "udp_include_util.h"

void *get_in_addr(struct sockaddr *sa);     // get sockaddr, IPv4 or IPv6:
void print_addrinfo_address(struct addrinfo *target_addrinfo);
void print_sockaddr_in_address(struct sockaddr_in *target_sockaddr_in);
void print_in_addr_address(struct in_addr *target_in_address);
struct sockaddr_in *storage2in(struct sockaddr_storage *target_storage);

#endif
