// command.h
#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#include <wait.h>

#include "list.h"
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
    char *filename_in;
    char *filename_out;
    pid_t pid;
    int status;
} cmd_t;

// return the length of the command, -1 if error
int init_cmd(cmd_t* cmd);

void free_cmd_inside(void* cmd);

int parse_cmd(list_t *cmd_head, list_t *word_head);
#endif /* END ifndef COMMAND_HEADER */