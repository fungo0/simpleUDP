#include "include/nonBlocking_dtls.h"
#include "include/wolfssl_include_util.h"
#include "include/include_util.h"
#include "include/udp_A.h"

WOLFSSL* nonBlock_connect(int listen_fd, struct timeval timeout, int currTimeout, WOLFSSL* ssl) {
    int ret;
    int nfds;
    int error;
    int result;
    int select_ret;
    fd_set recvfds;
    fd_set errfds;

    ret = wolfSSL_accept(ssl);
    currTimeout = 1;
    error = wolfSSL_get_error(ssl, 0);

    listen_fd = (int)wolfSSL_get_fd(ssl);
    nfds = listen_fd + 1;
    while (cleanup != 1 && (ret != SSL_SUCCESS && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE))) {
        if (cleanup == 1) {
            wolfSSL_free(ssl);
            wolfSSL_shutdown(ssl);
            return NULL;
        }

        // if (error == SSL_ERROR_WANT_READ)
            // printf("... server would read block\n");
        // else
            // printf("... server would write block\n");

        currTimeout = wolfSSL_dtls_get_current_timeout(ssl);

        FD_ZERO(&recvfds);
        FD_SET(listen_fd, &recvfds);
        FD_ZERO(&errfds);
        FD_SET(listen_fd, &errfds);

        result = select(nfds, &recvfds, NULL, &errfds, 0);      // &timeout bugged...

        if (result == 0) {
            select_ret = TEST_TIMEOUT;
        }
        else if (result > 0) {
            if (FD_ISSET(listen_fd, &recvfds)) {
                select_ret = TEST_RECV_READY;
            }
            else if(FD_ISSET(listen_fd, &errfds)) {
                select_ret = TEST_ERROR_READY;
            }
        }
        else {
            select_ret = TEST_SELECT_FAIL;
        }

        if ((select_ret == TEST_RECV_READY) || (select_ret == TEST_ERROR_READY)) {
            ret = wolfSSL_accept(ssl);
            error = wolfSSL_get_error(ssl, 0);
        }
        else if (select_ret == TEST_TIMEOUT && !wolfSSL_dtls(ssl)) {
            error = SSL_ERROR_WANT_READ;
        }
        else if (select_ret == TEST_TIMEOUT && wolfSSL_dtls(ssl) && wolfSSL_dtls_got_timeout(ssl) >= 0) {
            error = SSL_ERROR_WANT_READ;
        }
        else {
            error = SSL_FATAL_ERROR;
        }
    }
    if (ret != SSL_SUCCESS) {
        printf("SSL_accept failed with %d.\n", ret);
        cleanup = 1;
        return NULL;
    } else {
        cleanup = 0;
        // printf("Finished ssl handshake\n");
    }

    return ssl;
}

WOLFSSL* send_nonBlock_connect(WOLFSSL* ssl) {
    int ret;
    int nfds;
    int error = 0;
    int result;
    int select_ret;
    int nb_sockfd;
    fd_set recvfds;
    fd_set errfds;
    int currTimeout;
    struct timeval timeout;

    ret = wolfSSL_connect(ssl);
    error = wolfSSL_get_error(ssl, 0);

    nb_sockfd = (int) wolfSSL_get_fd(ssl);
    nfds = nb_sockfd + 1;

    while (ret != SSL_SUCCESS && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE)) {
        /* Variables that will reset upon every iteration */
        currTimeout = wolfSSL_dtls_get_current_timeout(ssl);

        timeout = (struct timeval) { (currTimeout > 0) ? currTimeout : 0, 0};

	    // if (error == SSL_ERROR_WANT_READ) {
	        // printf("... client would read block\n");
        // } else {
            // printf("... client would write block\n");
        // }

        /* Tcp select using dtls nonblocking functionality */
        FD_ZERO(&recvfds);
        FD_SET(nb_sockfd, &recvfds);
        FD_ZERO(&errfds);
        FD_SET(nb_sockfd, &errfds);

        result = select(nfds, &recvfds, NULL, &errfds, &timeout);       // timeout on reply?

        select_ret = TEST_SELECT_FAIL;

        if (result == 0) {
            select_ret = TEST_TIMEOUT;
        }
        else if (result > 0) {
            if (FD_ISSET(nb_sockfd, &recvfds)) {
                select_ret = TEST_RECV_READY;
            }
            else if (FD_ISSET(nb_sockfd, &errfds)) {
                select_ret = TEST_ERROR_READY;
            }
        }
        /* End "Tcp select ..." code */

        if (  select_ret == TEST_RECV_READY || select_ret == TEST_ERROR_READY ) {
            ret = wolfSSL_connect(ssl);
	        error = wolfSSL_get_error(ssl, 0);
        }
	    else if (select_ret == TEST_TIMEOUT && !wolfSSL_dtls(ssl)) {
	        error = 2;
	    }
	    else if (select_ret == TEST_TIMEOUT && wolfSSL_dtls(ssl) && wolfSSL_dtls_got_timeout(ssl) >= 0) {
	        error = 2;
	    }
	    else {
	        error = SSL_FATAL_ERROR;
	    }
    }

    if (ret != SSL_SUCCESS) {
        printf("SSL_connect failed with %d\n", ret);
        return NULL;
    }

    return ssl;
}
