#include "../include/include_util.h"
#include "header/input.h"
#define LSH_RL_BUFSIZE 1024         // BUFSIZE?

char *shell_read_line(void){        // return string
    int bufsize = LSH_RL_BUFSIZE;   // bufsize
    int position = 0;               // pos?
    char *buffer = (char*)malloc(sizeof(char) * bufsize);      // string buffer
    int c;                          // use to catch the return value of getchar(), likely c= 0 is error-free but != 0 or EOF is end or error

    if (!buffer) {                  // not successful malloc, echo error
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();             // keep reading character

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            // \n -> end of command
            // EOF -> terminate program
            buffer[position] = '\0';    // C-style string
            return buffer;  // return user input
        } else {
            buffer[position] = c;      // save character into buffer
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = (char*)realloc(buffer, bufsize);
            /*
                         * void * realloc ( void * ptr, size_t size );
                           ptr是指向之前已經宣告好的記憶體位置
                           ptr若是null，其功能就如同malloc()
                           size則是"調整之後的記憶體大小"
                         */
            if (!buffer) {              // realloc error
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
