#include "include/dtls_send.h"
#include "include/wolfssl_include_util.h"
#include "include/wolfssl_util.h"
#include "include/cleanup_util.h"
#include "include/create_socket.h"
#include "include/generate_key_util.h"
#include "include/udp_B.h"
#include "include/io_handler.h"
#define BUFSIZE 4096

WOLFSSL_CTX* send_ctx = 0;
int send_cleanup = 0;                 // flag to indicate cleanup

int dtls_send(byte* cipherText, int flag) {
    int cipherText_size = SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE;
    int sockfd;
    int bytesRcvd = 0;
    void *send_buf = malloc(cipherText_size);
    memset(send_buf, 0, cipherText_size);
    void *recv_buf = malloc(BUFSIZE);
    memset(recv_buf, 0, BUFSIZE);
    WOLFSSL* send_ssl = 0;
    void *cipher_text = malloc(cipherText_size);
    memset(cipher_text, 0, cipherText_size);
    send_cleanup = 0;
    send_ctx = get_send_ctx();

     /* Await Datagram */
    while (send_cleanup != 1) {
        /* initialize both socket and ssl */
        if((send_ssl = get_send_ssl(&sockfd, send_ctx, &B_addr, &A_addr)) == NULL) {
            perror("get_send_ssl");
            send_cleanup = 1;
            break;
        } else {
            // printf("Connected to compA\n");
        }

        memcpy(send_buf, cipherText, cipherText_size);

        /* send udp with ssl */
        if(wolfssl_send_buf(send_buf, cipherText_size, send_ssl) == -1) {
            perror("wolfssl_send_buf");
            send_cleanup = 1;
            break;
        } else {
            // printf("Sended to compA\n");
        }

        if(flag == 1) {
            /* receive udp with ssl */
            while((bytesRcvd = wolfssl_recv(send_ssl, recv_buf, cipherText_size)) <= 0) {
                // perror("wolfssl_recv");
                send_cleanup = 1;
            }

            memcpy(cipher_text, recv_buf, cipherText_size);

            decrypt_react(cipher_text, 1);

        }


        /* restart new ssl every time */
        round_clean_ssl(send_ssl);

        send_cleanup = 1;
    }

    if(send_cleanup == 1) {
        close(sockfd);
        wolfSSL_CTX_free(send_ctx);
        wolfSSL_Cleanup();
        memset(send_buf, 0, cipherText_size);
        memset(recv_buf, 0, BUFSIZE);
        free(send_buf);
        free(recv_buf);
        free(cipher_text);
    }

    return EXIT_SUCCESS;
}
