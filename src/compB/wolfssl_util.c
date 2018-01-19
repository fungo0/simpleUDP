#include "include/wolfssl_util.h"
#include "include/include_util.h"
#include "include/wolfssl_include_util.h"
#include "include/string_util.h"
#include "include/create_socket.h"
#include "include/nonBlocking_dtls.h"

WOLFSSL_CTX* get_send_ctx() {
    char cert_array[] = "/vagrant/certs/ca-cert.pem";
    char* certs = cert_array;
    WOLFSSL_CTX* ctx = 0;

    /* Set ctx to DTLS 1.2 */
    if ((ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method())) == NULL) {
        fprintf(stderr, "wolfSSL_CTX_new error.\n");
        return NULL;
    }

    /* Load certificates into ctx variable */
    if (wolfSSL_CTX_load_verify_locations(ctx, certs, 0) != SSL_SUCCESS) {
        fprintf(stderr, "Error loading %s, please check the file.\n", certs);
        return NULL;
    }

    return ctx;
}

WOLFSSL_CTX* get_recv_ctx() {
    char caCertLoc[] = "/vagrant/certs/ca-cert.pem";
    char servCertLoc[] = "/vagrant/certs/server-cert.pem";
    char servKeyLoc[] = "/vagrant/certs/server-key.pem";
    WOLFSSL_CTX* ctx = 0;

    /* Set ctx to DTLS 1.2 */
    if ((ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method())) == NULL) {
        fprintf(stderr, "wolfSSL_CTX_new error.\n");
        return NULL;
    }

    /* Load CA certificates */
    if (wolfSSL_CTX_load_verify_locations(ctx, caCertLoc, 0) != SSL_SUCCESS) {
        fprintf(stderr, "Error loading %s, please check the file.\n", caCertLoc);
        return NULL;
    }

    /* Load server certificates */
    if (wolfSSL_CTX_use_certificate_file(ctx, servCertLoc, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        printf("Error loading %s, please check the file.\n", servCertLoc);
        return NULL;
    }
    /* Load server Keys */
    if (wolfSSL_CTX_use_PrivateKey_file(ctx, servKeyLoc, SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        printf("Error loading %s, please check the file.\n", servKeyLoc);
        return NULL;
    }

    return ctx;
}

WOLFSSL* get_send_ssl(int *send_fd, WOLFSSL_CTX *target_ctx, struct sockaddr_in *this_addr, struct sockaddr_in *target_addr) {
    WOLFSSL* ssl = 0;

    /* Assign ssl variable */
    ssl = wolfSSL_new(target_ctx);
    if (ssl == NULL) {
        printf("unable to get ssl object");
        return NULL;
    } else {
        // printf("Got ssl object\n");
    }

    /* setup for connect */
    memset(target_addr, 0, sizeof(*target_addr));
	target_addr->sin_family = AF_INET;
	target_addr->sin_addr.s_addr = inet_addr(A_IP_ADDR);
	target_addr->sin_port = htons(A_RECEIVE_PORT_NUM);

    wolfSSL_dtls_set_peer(ssl, target_addr, sizeof(*target_addr));

    if ((*send_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
       printf("cannot create a socket.");
       return NULL;
    }

    /* Set the file descriptor for ssl and connect with ssl variable */
    wolfSSL_set_fd(ssl, *send_fd);
    wolfSSL_set_using_nonblock(ssl, 1);
    fcntl(*send_fd, F_SETFL, O_NONBLOCK);

    if((ssl = send_nonBlock_connect(ssl)) == NULL) {
        perror("nonBlock_connect");
        return NULL;
    } else {
        // printf("Connected compA\n");
    }

    return ssl;
}

WOLFSSL* get_recv_ssl(int recv_fd, int listen_fd, struct timeval timeout, int currTimeout, WOLFSSL_CTX *target_ctx) {
    WOLFSSL* ssl = 0;

    /* Assign ssl variable */
    ssl = wolfSSL_new(target_ctx);
    if (ssl == NULL) {
        printf("unable to get ssl object");
        return NULL;
    } else {
        // printf("Got ssl object\n");
    }

    /* Set the file descriptor for ssl and connect with ssl variable */
    wolfSSL_set_fd(ssl, recv_fd);

    wolfSSL_set_using_nonblock(ssl, 1);

    if((ssl = nonBlock_connect(listen_fd, timeout, currTimeout, ssl)) == NULL) {
        perror("nonBlock_connect");
        return NULL;
    }

    return ssl;
}

int wolfssl_send_buf(void *buf, int buflen, WOLFSSL* ssl) {
    if (wolfSSL_write(ssl, buf, buflen) != buflen) {
        perror("SSL_write failed");
        return -1;
    } else {
        // printf("Written to compA\n");
    }
    return 0;
}

int wolfssl_send_stream(FILE* input_stream, WOLFSSL* ssl) {
    char sendLine[MAXLINE];

    fgets(sendLine, MAXLINE, input_stream);

    if (wolfSSL_write(ssl, sendLine, strlen(sendLine)) != strlen(sendLine)) {
        perror("SSL_write failed");
        return -1;
    }

    return 0;
}

int wolfssl_recv(WOLFSSL* ssl, void* buff, int buff_len) {
    int recvLen = 0;
    recvLen = wolfSSL_read(ssl, buff, buff_len);

    if (recvLen < 0) {
        int readErr = wolfSSL_get_error(ssl, 0);
        if(readErr != SSL_ERROR_WANT_READ) {
            perror("wolfSSL_read failed");
            return -1;
        }
    } else {

    }

    return recvLen;
}
