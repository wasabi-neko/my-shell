#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>

#include "list.h"

#define FD_READ_END 0
#define FD_WRITE_END 1

#define INPUT_BUFSIZE 10 
#define TEMP_BUFSIZE 10
#define CMD_NUM 3
#define CMD_NAMESIZE 100
#define CMD_ARG_ARR_SIZE 10
#define CMD_ARG_STR_SIZE 10

#define PIPE_SYMBOL_INIT {OPER_PIPE, "|"}
#define REDIRECT_IN_SYMBOL_INIT {OPER_PIPE, "<"}
#define REDIRECT_OUT_SYMBOL_INIT {OPER_PIPE, ">"}

enum OPERATOR_ID {
    OPER_PIPE = 0,
    OPER_REDIRECT_IN,
    OPER_REDIRECT_OUT
};

struct OPERATOR_SYMBOL_T {
    enum OPERATOR_ID id;
    char *str;
};

struct OPERATOR_ALL_SYMBOL_T {
    struct OPERATOR_SYMBOL_T pipe;
    struct OPERATOR_SYMBOL_T re_in;
    struct OPERATOR_SYMBOL_T re_out;
} oper_symbols = {
    .pipe = PIPE_SYMBOL_INIT,
    .re_in = REDIRECT_IN_SYMBOL_INIT,
    .re_out = REDIRECT_OUT_SYMBOL_INIT
};

typedef struct COMMAND_T {
    char *name;
    char **argv;
    int fd_r;
    int fd_w;
    int fd_err;
    pid_t pid;
    int status;
} cmd_t;

// If str is NULL, then this word is an operator, the operator string sotred in oper[]
typedef struct WORD_T {
    char *str;
    enum OPERATOR_ID oper;
} word_t;

// Shell input buffer
char buf[INPUT_BUFSIZE];
cmd_t cmd_arr[CMD_NUM];
// TODO: word_t list & cmd_t list

int INT_handler();

// return the length of the command, -1 if error
int init_cmd(cmd_t* cmd);

// Print prompt
int prompt();

// Parse words from input string
// @return the real word array size, if error then return -1
int parse_word(word_t *arr, int max_arr_size, char *str);

// parse commands from input words
// @return the real command array size, if error then return -1
int parse_cmd(cmd_t* cmds, int max_cmd_size, word_t *words, int len);

// fork and exec the commands, and then wait for all of the commands
int execute_cmd(cmd_t* cmds, int len);  // TODO: execute_cmd

int main(int argc, char **argv)
{
    // TODO: preprocess e.t. read .src file


    while (1) {
        // TODO: prompt

        // Read input from stdin
        memset(buf, 0, INPUT_BUFSIZE);
        ssize_t size = read(STDIN_FILENO, buf, INPUT_BUFSIZE);
        if (size < 0) {
            // Error
            perror("read");
            puts("Something is wrong with the input.");
            memset(buf, 0, INPUT_BUFSIZE);
            continue;

        } else if (size >= INPUT_BUFSIZE) {
            // Buffer overflow
            puts("Input buffer too big."); 
            // TODO: abort this line when string len to big
            memset(buf, 0, INPUT_BUFSIZE);
            continue;

        } else if (size == 0) {
            // EOF
            break;
        }

        printf("#%s#\n", basename(buf));
        fflush(stdout);


    }

    return 0;
}

/**
 * Init the input cmd_t command
 * allocate memory for `name` and `argv pointers`
 */
int init_cmd(cmd_t* cmd)
{
    if (cmd != NULL) {
        return 0;
    }

    cmd = malloc(sizeof(cmd));
    if (cmd == NULL) {
        perror("malloc Error");
        return -1;
    }

    cmd->name = malloc(CMD_NAMESIZE);
    cmd->argv = malloc(CMD_ARG_ARR_SIZE);    // only allocate the space for pointer
    
    if (cmd->name == NULL || cmd->argv == NULL) {
        perror("malloc error");
        return -1;
    }

    cmd->fd_r = STDIN_FILENO;
    cmd->fd_w = STDOUT_FILENO;
    cmd->fd_err = STDERR_FILENO;

    cmd->pid = -1;
    cmd->status = -1;

    return 0;
}

int parse_word(word_t *arr, int max_arr_size, char *str)
{
    // TODO: parse word from input string
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
