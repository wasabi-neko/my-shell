// command.h
#ifndef COMMAND_HEADER
#define COMMAND_HEADER

#include "list.h"
#include "word.h"

#define FD_READ_END 0
#define FD_WRITE_END 1

typedef struct COMMAND_T {
    char *name;
    int argc;
    char **argv;
    int fd_r;           /* file descriptor */
    int fd_w;
    int fd_err;
    /* Just use the char data[] to store the string */
    char *filein_name;   /* file in */
    char *fileout_name;  /* file out */
    pid_t pid;
    int status;
} cmd_t;

// return the length of the command, -1 if error
int init_cmd(cmd_t* cmd);

void free_cmd_inside(void* cmd);

int parse_cmd(list_t *cmd_head, list_t *word_head);
#endif /* END ifndef COMMAND_HEADER */