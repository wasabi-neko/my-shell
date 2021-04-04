// command.h

#include <wait.h>

#include "word.h"

#define CMD_NUM 3
#define CMD_NAMESIZE 100
#define CMD_ARG_ARR_SIZE 10
#define CMD_ARG_STR_SIZE 10

typedef struct COMMAND_T {
    char *name;
    int argc;
    char **argv;
    int fd_r;
    int fd_w;
    int fd_err;
    pid_t pid;
    int status;
} cmd_t;

// return the length of the command, -1 if error
int init_cmd(cmd_t* cmd);

void _free_cmd_inside(void* cmd);

// parse commands from input words
// @return the real command array size, if error then return -1
int parse_cmd(cmd_t* cmds, int max_cmd_size, word_t *words, int len);