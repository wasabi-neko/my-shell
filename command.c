// command.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "command.h"
#include "symbol.h"

int strcpy_malloc(char **dst, const char *src) 
{   
    if (src == NULL) {
        return -1;
    }
    int len = strlen(src) + 1;

    *dst = malloc(len);
    if (*dst == NULL) {
        perror("malloc failed");
        return -1;
    }

    strncpy(*dst, src, len);
    (*dst)[len-1] = '\0';  // add an '\0' at end

    return 0;
}

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
    cmd->argc = 0;
    cmd->argv = NULL;
    cmd->fd_r = STDIN_FILENO;
    cmd->fd_w = STDOUT_FILENO;
    cmd->fd_err = STDERR_FILENO;
    cmd->filein_name = NULL;
    cmd->fileout_name = NULL;
    cmd->pid = -1;
    cmd->status = -1;

    return 0;
}

void free_cmd_inside(void* cmd)
{
    cmd_t *_cmd = (cmd_t *)(cmd);
    free(_cmd->name);

    for (int i = 0; i < _cmd->argc; i++) {
        free(_cmd->argv[i]);
    }
    free(_cmd->argv);
    free(_cmd->filein_name);
    free(_cmd->fileout_name);

    if (_cmd->fd_r != STDIN_FILENO) {
        close(_cmd->fd_r);
    }
    if (_cmd->fd_w != STDOUT_FILENO) {
        close(_cmd->fd_w);
    }
    if (_cmd->fd_err != STDERR_FILENO) {
        close(_cmd->fd_err);
    }
}

void print_cmd(const cmd_t* cmd)
{
    printf("-----Print cmd-----\n");
    printf("pid: %d\n", cmd->pid);
    printf("name: %s\n", cmd->name);
    printf("arg: ");
    for (int i = 0; i < cmd->argc; i++) {
        printf("%s, ", cmd->argv[i]);
    }
    printf("\n");
    printf("fd(r:%d, w:%d)\n", cmd->fd_r, cmd->fd_w);
    printf("file(in: %s, out: %s)\n", cmd->filein_name, cmd->fileout_name);
    printf("\n");
}

int parse_cmd(list_t *cmd_head, list_t *word_head)
{
    if (cmd_head == NULL) {
        return -1;
    }
    if (word_head == NULL || word_head->next == NULL) {
        return -1;
    }
    /* Free all, just for avoid potentail bugs.... */
    if (list_free(cmd_head->next, cmd_head, free_cmd_inside) != 0) {
        return -1;
    }

    /* Init */
    // int cmd_cnt = 0;
    int fd_r = STDIN_FILENO;
    int fd_w = STDOUT_FILENO;
    int fd_err = STDERR_FILENO;
    list_t *cur_cmd = cmd_head;
    list_t *cur_word = word_head->next;

    // For each commands
    while (cur_word != word_head) {
        /* Get new command */
        cur_cmd = list_get_next(cmd_head, cur_cmd, sizeof(cmd_t));
        cmd_t *cmd_ptr = LIST_DATA_PTR(cur_cmd, cmd_t);


        /* init command and set the fds */
        init_cmd(cmd_ptr);
        cmd_ptr->fd_r = fd_r;
        cmd_ptr->fd_w = fd_w;
        cmd_ptr->fd_err = fd_err;

        /* Set command from word0 */
        word_t *arg0 = LIST_DATA_PTR(cur_word, word_t);    /* Firs arg */
        if (arg0->str == NULL) {
            perror("syntax error, use operator as first arg of command");
            return -1;
        }
        
        if (strcpy_malloc(&cmd_ptr->name, arg0->str) != 0) {
            return -1;
        }
        cmd_ptr->argc = arg0->oper_id;
        cmd_ptr->argv = malloc(sizeof(char*) * cmd_ptr->argc);
        
        if (cmd_ptr->argv == NULL) {
            perror("malloc failed");
            return -1;
        }
        // char *base_name = basename(arg0->str);
        strcpy_malloc(&(cmd_ptr->argv[0]), arg0->str);  /* TODO: parse the path to file name */
        cur_word = cur_word->next;      /* start from arg1 */
        /* End set command */

        /* For each args */
        for (int i = 1; i < cmd_ptr->argc; i++) {
            char *str = LIST_DATA_PTR(cur_word, word_t)->str;
            int len = strlen(str) + 1;

            cmd_ptr->argv[i] = malloc(len);
            strncpy(cmd_ptr->argv[i], str, len);
            cmd_ptr->argv[i][len - 1] = '\0';
            
            cur_word = cur_word->next;      /* next word */
        }

        /* Read file redirect until pipe */
        char **filename_ptr = NULL;
        int next_cmd = 0;
        while (cur_word != word_head && !next_cmd) {
            word_t *word = LIST_DATA_PTR(cur_word, word_t);

            if (word->str == NULL) {
                /* operator */
                if (word->oper_id == oper_symbols.pipe.id) {
                    int fd[2];
                    if (pipe(fd) != 0) {
                        perror("pipe failed");
                        return -1;
                    }
                    cmd_ptr->fd_w = fd[FD_WRITE_END];   /* Set this command fd to write fd */
                    fd_r = fd[FD_READ_END];             /* Set next command fd to read fd */
                    next_cmd = 1;

                } else if (word->oper_id == oper_symbols.re_in.id) {
                    filename_ptr = &cmd_ptr->filein_name;
                } else if (word->oper_id == oper_symbols.re_out.id) {
                    filename_ptr = &cmd_ptr->fileout_name;
                } else {
                    perror("syntax error. operator not found");
                    return -1;
                }
            } else {
                /* file name */
                if (filename_ptr == NULL) {
                    perror("some unknown error");
                    return -1;
                }
                
                int len = strlen(word->str) + 1;
                if (*filename_ptr != NULL) {
                    free(*filename_ptr);
                }
                *filename_ptr = malloc(len);
                strncpy(*filename_ptr, word->str, len);
                (*filename_ptr)[len - 1] = '\0';
            }

            cur_word = cur_word->next;
        }
    }
    /* End for each commands */

    return 0;
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