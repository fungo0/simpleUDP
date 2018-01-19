#ifndef CLEANUP_UTIL_H_INCLUDED
#define CLEANUP_UTIL_H_INCLUDED

#include "wolfssl_include_util.h"

void round_clean_ssl(WOLFSSL* target_ssl);
void end_clean_ctx(int target_fd, WOLFSSL_CTX *target_ctx);

#endif
