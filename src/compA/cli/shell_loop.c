#include "../include/include_util.h"
#include "header/shell_loop.h"
#include "header/input.h"
#include "header/tokenize.h"
#include "header/execute.h"

int shell_loop(int argc, const char *argv[]){
    char *line;
    char **args;
    int status;

    line = shell_read_line();       // input
    args = shell_split_line(line);  // split into token
    status = shell_execute(args);   // execute commands

    free(line);
    free(args);

    return status;
}
