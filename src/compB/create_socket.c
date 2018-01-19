#include "include/create_socket.h"
#include "include/include_util.h"
#include "include/udp_include_util.h"
#include "include/ancillary_data.h"

/* needed for AF_INET only */
int create_udp_sock(struct sockaddr_in *this_addr, struct sockaddr_in *target_addr, int bind_port_no) {
    int sock_fd, on = 1;
    struct sockaddr_in B_addr;

    /* Creates an UDP socket (SOCK_DGRAM) */
    if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        perror("setsockopt reuseaddr");
        return -1;
    }

    /* setup for bind */
    memset(this_addr, 0, sizeof(*this_addr));
    this_addr->sin_family = AF_INET;
    this_addr->sin_addr.s_addr = inet_addr(B_IP_ADDR);
    this_addr->sin_port = htons(bind_port_no);

    /* Bind the UDP socket to the port A_PORT_NUM and to the current machines IP address */
    if (bind(sock_fd, (struct sockaddr *)this_addr, sizeof(*this_addr)) == -1) {
        perror("bind()");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}
