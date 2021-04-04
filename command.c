// command.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

/**
 * Init the input cmd_t command
 * allocate memory for `name` and `argv pointers`
 */
int init_cmd(cmd_t* cmd)
{
    if (cmd == NULL) {
        return -1;
    }

    cmd->name = NULL;
    cmd->argv = NULL;
    cmd->fd_r = STDIN_FILENO;
    cmd->fd_w = STDOUT_FILENO;
    cmd->fd_err = STDERR_FILENO;
    cmd->pid = -1;
    cmd->status = -1;

    return 0;
}

void _free_cmd_inside(void* cmd)
{
    cmd_t *_cmd = (cmd_t *)(cmd);
    if (_cmd->name != NULL) {
        free(_cmd->name);
    }

    if (_cmd->argv != NULL) {
        /* Free every string in the argv */
        for (int i = 0; i < _cmd->argc; i++) {
            if (_cmd->argv[i] != NULL) {
                free(_cmd->argv[i]);
            }
        }
    }

    if (_cmd->argv != NULL) {
        free(_cmd->argv);
    }
}

/**
 * Parse the string and put the result into cmds array
 */
// int parse_cmd(cmd_t *cmds, char* str)
// {
//     // read cmd until spacial:" " "|" ">" "<"
//     int cmd_cnt = 0;
//     int next_cmd = 0;
//     int next_fd_r = STDIN_FILENO;
//     int next_fd_w = STDOUT_FILENO;
//     int next_fd_err = STDERR_FILENO;

//     //* Loop for all commands
//     while (1) {

//         // Check if too many commands
//         if (cmd_cnt >= CMD_ARG_ARR_SIZE) {
//             perror("too many commands");
//             return -1;
//         }

//         // Set command
//         if (cmds[cmd_cnt].name == NULL) {
//             if (init_cmd(&cmds[cmd_cnt]) != 0) {
//                 return -1;
//             }
//         }
//         cmds[cmd_cnt].fd_r = next_fd_r;
//         cmds[cmd_cnt].fd_w = next_fd_w;
//         cmds[cmd_cnt].fd_err = next_fd_err;

//         //* Loop for one command
//         int arg_cnt = 0;
//         int next_arg = 0;
//         while (!next_cmd) {
//             // Set one argument string
//             if (cmds[cmd_cnt].argv[arg_cnt] == NULL) {
//                 cmds[cmd_cnt].argv[arg_cnt] = malloc(CMD_ARG_STR_SIZE);
//                 if (cmds[cmd_cnt].argv[arg_cnt] == NULL) {
//                     perror("malloc error");
//                     return -1;
//                 }
//             }


//             //* Loop for arguments in current command
//             // The first arguments will be the executable name
//             char *cur_arg = &cmds[cmd_cnt].argv[arg_cnt];

//             while (!next_arg) {
//                 // if argument stering too long
//                 if (cur_arg >= (&cmds[cmd_cnt].argv[arg_cnt] + CMD_ARG_STR_SIZE - 2)) {
//                     perror("arg string too long");
//                     return -1;
//                 }

//                 sscanf(str, "%c", cur_arg);

//                 if (*cur_arg == ' ') {
//                     // next argument
//                     next_arg = 1;

//                 } else if (*cur_arg == '<') {
//                     // Redirect in
//                     next_arg = 1;
//                     // TODO: read file name

//                 } else if (*cur_arg == '>') {
//                     // Redirect out
//                     next_arg = 1;

//                 } else if (*cur_arg == '|') {
//                     // Piping
//                     next_arg = 1;
//                     next_cmd = 1;

//                     int fd[2];
//                     pipe(fd);
//                     cmds[cmd_cnt].fd_w = fd[FD_WRITE_END];
//                     next_fd_r = fd[FD_READ_END];
//                 }
                
//                 if (!next_arg) {
//                     cur_arg++;
//                 }
//             }
//             // END of argument Loop
//             cur_arg = '\0';         // Set end of string
//         }
//         // END of command loop
//         cmds[cmd_cnt].argv[arg_cnt + 1] = NULL;     // Set end of array
//     }
//     // END of All commands loop
// }