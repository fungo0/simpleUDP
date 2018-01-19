#include "../include/include_util.h"
#include "header/shell.h"

int shell_execute(char **args){
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    printf("No such command! Type `help` to see what is available\n");
    return 1;
}
