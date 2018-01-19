#include "include/udp_A.h"
#include "include/include_util.h"
#include "include/udp_include_util.h"
#include "include/wolfssl_include_util.h"
#include "include/create_socket.h"
#include "include/wolfssl_util.h"
#include "include/generate_key_util.h"
#include "include/string_util.h"
#include "include/cleanup_util.h"
#include "include/interrupt_handler.h"
#include "cli/header/shell_loop.h"
#include "include/io_handler.h"
#include "include/dtls_send.h"

#define BUFSIZE MAXLINE

WOLFSSL_CTX* recv_ctx = NULL;
struct sockaddr_in A_addr;
struct sockaddr_in B_addr;
int cleanup = 0;                 // flag to indicate cleanup
int INVENTORY = 1000;

int main(int argc, const char *argv[]) {
    int bytesRcvd = 0;
    int readWriteErr = 0;
    int listenfd = 0;
    int connectfd = 0;
    int cipherText_size = SALT_SIZE+SHA256_DIGEST_SIZE + SALT_SIZE+AES_BLOCK_SIZE+AES_BLOCK_SIZE;
    char *connection_buf = malloc(BUFSIZE);      // !!!unsigned char for recvfrom!!!
    memset(connection_buf, 0, BUFSIZE);
    char tmp_buf[BUFSIZE];
    byte *send_buf = malloc(cipherText_size);
    memset(send_buf, 0, cipherText_size);
    byte *recv_buf = malloc(BUFSIZE);
    memset(recv_buf, 0, BUFSIZE);
    socklen_t B_addr_len = sizeof(B_addr);
    WOLFSSL* recv_ssl = NULL;
    /* DTLS set nonblocking flag */
    int flags = fcntl(*(&listenfd), F_GETFL, 0);
    /* NonBlockingSSL_Accept variables */
    struct timeval timeout = {0,0};
    int currTimeout;
    /* select variables */
    int sel;
    fd_set readfds;
    FD_ZERO(&readfds);
    fd_set errorfds;
    FD_ZERO(&errorfds);
    /* cli return flags */
    int cli_flag;
    /* resend flag */
    byte *reply_buffer = malloc(cipherText_size);
    memset(reply_buffer, 0, cipherText_size);
    byte *send_reply_buffer;
    /* refill commands */
    char* refill_command = malloc(8);
    memset(refill_command, 0, 8);
    memcpy(refill_command, "REFILL", 7);
    int refill_value = 500;

    /* Code for handling signals */
    handle_sig();

    /* Initialize wolfSSL before assigning ctx */
    wolfSSL_Init();

    /* for debug */
    // wolfSSL_Debugging_ON();

    recv_ctx = get_recv_ctx();

     /* Await Datagram */
    while (cleanup != 1) {

        /* initialize timer for nonblocking socket */
        timeout.tv_sec = (currTimeout > 0) ? currTimeout : 0;

        listenfd = create_udp_sock(&A_addr, &B_addr, A_RECEIVE_PORT_NUM);
        /* DTLS set nonblocking */
        flags = fcntl(*(&listenfd), F_SETFL, flags | O_NONBLOCK);
        if (flags < 0) {
            printf("fcntl set failed.\n");
            cleanup = 1;
        }

        if(INVENTORY == 0) {
            printf("Sended refill request\n");
            char* refill_value_str = num2str(refill_value);
            byte* request_cipher = encrypt_react(refill_command, refill_value_str);
            dtls_send(request_cipher, 1);
            free(request_cipher);
            free(refill_value_str);
        }

        /* set up select */
        FD_ZERO(&readfds);
        FD_ZERO(&errorfds);
        FD_SET(listenfd, &readfds);
        FD_SET(listenfd, &errorfds);
        FD_SET(STDIN_FILENO, &readfds);
        sel = select(listenfd+1, &readfds, (fd_set*)0, &errorfds, 0);
        if (sel <= 0) {
            continue;
        }
        else if (sel > 0) {
            if (FD_ISSET(listenfd, &readfds) || FD_ISSET(listenfd, &errorfds)) {
                /* UDP-read-connect */
                do {
                    if (cleanup == 1) {
                        break;
                    }

                    bytesRcvd = (int)recvfrom(listenfd, (char*)connection_buf, BUFSIZE, MSG_PEEK, (struct sockaddr*)&B_addr, &B_addr_len);
                } while (bytesRcvd <= 0);

                if (bytesRcvd > 0) {
                    if(strcmp(inet_ntoa(B_addr.sin_addr),B_IP_ADDR) != 0) {
                        /* setup for connect */
                        memset(&B_addr, 0, sizeof(B_addr));
                    	B_addr.sin_family = AF_INET;
                    	B_addr.sin_addr.s_addr = inet_addr(B_IP_ADDR);
                    	B_addr.sin_port = htons(B_SEND_PORT_NUM);
                    }
                    /* connect the UDP socket to the port A_PORT_NUM and to the A machine's IP address */
                    if(connect(listenfd, (struct sockaddr *)&B_addr, sizeof(B_addr)) != 0) {
                		perror("connect()");
                		cleanup = 1;
                        break;
                	} else {
                        // printf("Connected to compB\n");
                    }
                } else {
                    printf("Recvfrom failed.\n");
                    cleanup = 1;
                }

                // printf("Connected!\n");

                /* ensure connection_buf is empty upon each call */
                memset(connection_buf, 0, BUFSIZE);
                connectfd = listenfd;

                /* accept connection from client */
                if((recv_ssl = get_recv_ssl(connectfd, listenfd, timeout, currTimeout, recv_ctx)) == NULL) {
                    perror("get_recv_ssl");
                    continue;
                    // cleanup = 1;
                } else {
                    // printf("Accepted compB\n");
                }

                /* receive udp with ssl */
                bytesRcvd = wolfssl_recv(recv_ssl, recv_buf, BUFSIZE-1);

                /* Begin do-while read */
                do {
                    if (cleanup == 1) {
                        memset(recv_buf, 0, strlen(recv_buf));
                        break;
                    }
                    if (bytesRcvd < 0) {
                        readWriteErr = wolfSSL_get_error(recv_ssl, 0);
                        if (readWriteErr != SSL_ERROR_WANT_READ) {
                            printf("Read Error, error was: %d.\n", readWriteErr);
                            cleanup = 1;
                        }
                        else {
                            bytesRcvd = wolfssl_recv(recv_ssl, recv_buf, BUFSIZE-1);
                        }
                    }
                } while (readWriteErr == SSL_ERROR_WANT_READ && bytesRcvd < 0 && cleanup != 1);
                if (bytesRcvd > 0) {
                    // printf("recved\n");
                    memcpy(reply_buffer, recv_buf, cipherText_size);
                    send_reply_buffer = decrypt_react(reply_buffer,  0);

                    if(send_reply_buffer != 0) {
                        memcpy(send_buf, send_reply_buffer, cipherText_size);
                    }
                }
                else {
                    printf("Connection Timed Out.\n");
                }

                if(send_reply_buffer != 0) {
                    /* Begin do-while write */
                    do {
                        if (cleanup == 1) {
                           memset(recv_buf, 0, BUFSIZE);
                           break;
                        }
                        readWriteErr = wolfSSL_get_error(recv_ssl, 0);
                        if (( wolfssl_send_buf(send_buf, cipherText_size, recv_ssl)) < 0) {
                           perror("wolfssl_send_buf");
                           cleanup = 1;
                        }
                        // printf("Reply sent\n");
                    } while(readWriteErr == SSL_ERROR_WANT_WRITE && cleanup != 1);
                    /* End do-while write */
                }

                /* free allocated memory */
                memset(recv_buf, 0, BUFSIZE);
                do {
                    if (cleanup == 1) {
                        break;
                    }

                    bytesRcvd = (int)recvfrom(listenfd, (char *)&tmp_buf, sizeof(tmp_buf), 0, (struct sockaddr*)&B_addr, &B_addr_len);
                } while (bytesRcvd <= 0);

                memset(tmp_buf, 0, BUFSIZE);
                bytesRcvd = 0;

                wolfSSL_free(recv_ssl);

            }

            //if stdin is ready to be read
            if(FD_ISSET(STDIN_FILENO, &readfds)) {
                close(listenfd);
                cli_flag = shell_loop(argc, argv);
                if(cli_flag == 0) {             // shutdown
                    cleanup = 1;
                    break;
                } else if(cli_flag == -1) {
                    perror("shell_loop");
                    continue;
                }
            }
        }

        close(listenfd);

    }

    if(cleanup == 1) {
        end_clean_ctx(listenfd, recv_ctx);
        wolfSSL_Cleanup();
        free(send_buf);
        free(recv_buf);
        free(connection_buf);
        free(reply_buffer);
        free(refill_command);
        free(send_reply_buffer);
    }

    return EXIT_SUCCESS;

}
