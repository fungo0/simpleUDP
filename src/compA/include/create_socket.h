#ifndef CREATE_SOCKET_H_INCLUDED
#define CREATE_SOCKET_H_INCLUDED

#include "udp_include_util.h"

#define A_RECEIVE_PORT_NUM 11111
#define A_SEND_PORT_NUM 4950
#define A_IP_ADDR "192.168.30.21"
#define B_RECEIVE_PORT_NUM 4950
#define B_SEND_PORT_NUM 11111
#define B_IP_ADDR "192.168.30.22"

int create_udp_sock(struct sockaddr_in *this_addr, struct sockaddr_in *target_addr, int bind_port_no);

#endif
