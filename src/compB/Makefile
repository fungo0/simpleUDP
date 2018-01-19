#src: https://stackoverflow.com/questions/8025766/makefile-auto-dependency-generation
CC=gcc
CFLAGS += -DWOLFSSL_DTLS
CFLAGS += -g -MD -MP
LIBS=-lwolfssl -lpthread

SRC = \
aes_encrypt.c\
cleanup_util.c\
cli/execute.c\
cli/input.c\
cli/shell.c\
cli/shell_loop.c\
cli/tokenize.c\
create_socket.c\
dtls_send.c\
generate_key_util.c\
hmac.c\
interrupt_handler.c\
io_handler.c\
nonBlocking_dtls.c\
string_util.c\
udp_B.c\
wolfssl_util.c


comp3334: $(SRC:%.c=%.o)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

-include $(SRC:.c=.d)

# Clean
.PHONY: clean

clean:
	rm -f *.o;rm -f *.d;
