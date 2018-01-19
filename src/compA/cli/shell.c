#include "../include/include_util.h"
#include "header/shell.h"
#include "../include/io_handler.h"
#include "../include/string_util.h"
#include "../include/udp_A.h"

/*
 Function Declarations for builtin shell commands:
 */
 int shell_SEND(char **args);
 int shell_REQUEST(char **args);
 int shell_CHECK(char **args);
 int shell_REFILL(char **args);
 int shell_help(char **args);
 int shell_endProgram(char **args);

/*
 List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "SEND",
    "REQUEST",
    "CHECK",
	"REFILL",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &shell_SEND,
    &shell_REQUEST,
    &shell_CHECK,
	&shell_REFILL,
    &shell_help,
    &shell_exit
};

int shell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

/*
 Builtin function implementations.
 */

int shell_SEND(char **args) {
    if(args[1] == NULL) {
        printf("Need another parameter!! See `help` page!\n");
        return 1;
    }
    if(str2num(args[1]) > INVENTORY) {
        printf("Not enough! Current INVENTORY is %d\n", INVENTORY);
        return 1;
    } else if(str2num(args[1]) < 0) {
        printf("wtf?\n");
        return 1;
    }
    if(encrypt_send(args) != 1) {
        return -1;
    }
    return 1;
}

int shell_REQUEST(char **args) {
    if(args[1] == NULL) {
        printf("Need another parameter!! See `help` page!\n");
        return 1;
    }
    if(str2num(args[1]) < 0) {
        printf("wtf?\n");
        return 1;
    }
    if(encrypt_send(args) != 1) {
        return -1;
    }
    return 1;
}

int shell_CHECK(char **args) {
    if(encrypt_send(args) != 1) {
        return -1;
    }
    return 1;
}

int shell_REFILL(char **args) {
    if(encrypt_send(args) != 1) {
        return -1;
    }
    return 1;
}

int shell_help(char **args){
    int i;

    // printf("->%s\n", args[1]);
    if(args[1] == NULL) {
        printf("Type help `command` for more details.\n");
        printf("The following are built in:(case sensitive)\n");

        for (i = 0; i < shell_num_builtins(); i++) {
            printf("  %s\n", builtin_str[i]);
        }
    } else if(strcmp(args[1], "SEND") == 0) {
        printf("Usage: SEND <no-of-items>\n");
    } else if(strcmp(args[1], "REQUEST") == 0) {
        printf("Usage: REQUEST <no-of-items> \n");
    } else if(strcmp(args[1], "CHECK") == 0) {
        printf("Usage: CHECK \n");
    } else if(strcmp(args[1], "REFILL") == 0) {
        printf("Usage: REFILL \n");
    } else if(strcmp(args[1], "help") == 0) {
        printf("Usage: help \n");
    } else if(strcmp(args[1], "exit") == 0) {
        printf("Usage: exit \n");
    }


    return 1;
}

int shell_exit(char **args){
    printf("Bye-bye!\n");
    return 0;
}
