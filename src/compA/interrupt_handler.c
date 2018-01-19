#include "include/interrupt_handler.h"
#include "include/wolfssl_include_util.h"
#include "include/udp_A.h"

void sig_handler(const int sig) {
    // printf("\nSIGINT %d handled\n", sig);
    cleanup = 1;
    return;
}

void handle_sig() {
    struct sigaction act, oact;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, &oact);
}
