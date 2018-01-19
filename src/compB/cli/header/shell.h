#ifndef shell_h
#define shell_h
/*
 Function Declarations for builtin shell commands:
 */
int shell_SEND(char **args);
int shell_REQUEST(char **args);
int shell_CHECK(char **args);
int shell_REFILL(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/*
 List of builtin commands, followed by their corresponding functions.
 */
extern char *builtin_str[];

extern int (*builtin_func[]) (char **);

int shell_num_builtins();


#endif /* shell_h */
