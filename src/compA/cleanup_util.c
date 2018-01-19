#include "include/cleanup_util.h"
#include "include/wolfssl_include_util.h"

void round_clean_ssl(WOLFSSL* target_ssl) {
    wolfSSL_shutdown(target_ssl);
    wolfSSL_free(target_ssl);
    return;
}

void end_clean_ctx(int target_fd, WOLFSSL_CTX *target_ctx) {
    close(target_fd);
    wolfSSL_CTX_free(target_ctx);
    return;
}
