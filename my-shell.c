// my-shell.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>

#include "list.h"
#include "word.h"
#include "command.h"

//----------------------------------------------------------------
// Define
//----------------------------------------------------------------
#define INPUT_BUFSIZE 128
#define RESTART 1

/** ----------------------------------------------------------------
 * Global Variables
 * ----------------------------------------------------------------*/
static sigjmp_buf env;
/* Shell input buffer */
char buf[INPUT_BUFSIZE];        /* Shell input buffer */
list_t *cmd_head;
list_t *word_head;

/** ----------------------------------------------------------------
 * Methods
 * ----------------------------------------------------------------*/
void fatal(int val, int assert);
int scan_line(char *line);
void abort_line();
int prompt();                   // Print prompt
int execute_cmd(list_t* cmds);  // fork and exec the commands, and then wait for all of the commands
void INT_handler();
// TODO: print color controller

/** ----------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------*/
int main(int argc, char **argv)
{

    // TODO: preprocess e.t. read .src file
    /* Set INT handler */
    signal(SIGINT, INT_handler);

    /* Init list */
    cmd_head = LIST_NEW_NODE(cmd_t);
    word_head = LIST_NEW_NODE(word_t);

    init_cmd(LIST_DATA_PTR(cmd_head, cmd_t));
    init_word(LIST_DATA_PTR(word_head, word_t));

    if (cmd_head == NULL || word_head == NULL) {
        perror("malloc error");
        perror("Cannot allocate memory at the first time");
        return -1;
    }

    /* Restart point */
    if (sigsetjmp(env, 2) == RESTART) {
        printf("\033[0;31m");
        printf("\nSIGINT UwU\n");
        printf("\033[0;37m");
    }
    /* some conditional variables */
    int is_break = 0;
    int is_error = 0;

    /* Main Loop */
    while (!is_break) {
        /* reset */
        fflush(stdout);
        is_error = 0;

        if (prompt() != 0) {
            perror("prompt error");
            is_error = 1;
        }

        /* Read input from stdin */
        memset(buf, 0, INPUT_BUFSIZE);
        int size = scan_line(buf);

        if (size == EOF) {
            is_break = 1;
        } else if (size < 0) {
            // Error
            perror("read");
            puts("Something is wrong with the input.");
            is_error = 1;

        } else if (size >= INPUT_BUFSIZE) {
            // Buffer overflow
            puts("Input buffer too big."); 
            is_error = 1;

        } else if (size == 0) {
            /* Nothing to do */
            continue;
        }

        if (is_break) {
            break;
        }
        if (is_error) {
            /* abort this line and reset buf */
            abort_line();
            memset(buf, 0, INPUT_BUFSIZE);
            continue;
        }

        // printf("#%s#\n", buf);
        /* Prase word */
        if (parse_word(word_head, buf) != 0) {
            perror("parse_word error");
            list_free(word_head->next, word_head, &free_word_inside);   /* reset word list */
            continue;
        }

        /* Test */
        // LIST_FOREACH(_ptr, word_head) {
        //     word_t *word = LIST_DATA_PTR(_ptr, word_t);
        //     printf("(%s, %d) ", word->str, word->oper_id);
        // }
        // printf("\n");

        /* Parse command */
        if (parse_cmd(cmd_head, word_head) != 0) {
            perror("parse_cmd error");
            list_free(cmd_head->next, cmd_head, &free_cmd_inside);
            continue;
        }

        /* Test */
        #ifdef DEBUG
        LIST_FOREACH(_ptr, cmd_head) {
            cmd_t *cmd = LIST_DATA_PTR(_ptr, cmd_t);
            print_cmd(cmd);
        }
        printf("---END pre exec---\n");
        #endif

        /* execute */
        if (execute_cmd(cmd_head) != 0) {
            perror("execute_cmd error");
            return -1;
        }

        #ifdef DEBUG
        /* Test */
        LIST_FOREACH(_ptr, cmd_head) {
            cmd_t *cmd = LIST_DATA_PTR(_ptr, cmd_t);
            print_cmd(cmd);
        }
        #endif
        fflush(stdout);
    }

    list_free(word_head->next, word_head, &free_word_inside);
    list_free(cmd_head, cmd_head, &free_cmd_inside);
    return 0;
}

void fatal(int val, int assert)
{
    if (val != assert) {
        perror("Fatal Error");
        exit(-1);
    }
}

int prompt()
{
    printf("\033[0;36m");       /* Set to Cyan */
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != 0) {
        printf("%s", cwd);
    }
    printf("aaa:");
    printf("\033[0;37m");       /* Back to white */
    printf("$ ");

    return 0;
}


/**
 * @return the size of the line '\n' not including. EOF if EOF countered
 */
int scan_line(char *line)
{
    char ch;
    int size = 0;
    while (1) {
        if (size >= INPUT_BUFSIZE) {
            break;
        }
        if (scanf("%c", &ch) == EOF)  {
            size = EOF;
            break;
        }

       if (ch == '\n') {
           line[size] = '\0';
           break;
       }
       line[size++] = ch;
    }

    return size;
}

void abort_line()
{   
    char ch;
    while (1) {
        if (scanf("%c", &ch) == EOF) {
            break;
        }
        if (ch == '\n') {
            break;
        }
    }
}

int execute_cmd(list_t* cmd_list) 
{   
    /* init fd */
    int fd_r = STDIN_FILENO;
    int fd_w = STDOUT_FILENO;

    /* Fork and exec all */
    LIST_FOREACH(ptr, cmd_list) {
        cmd_t *cmd = LIST_DATA_PTR(ptr, cmd_t);
        cmd->fd_r = fd_r;   /* the fd from last command */
        cmd->fd_w = fd_w;

        /* Redirect file if there's any */
        if (cmd->filein_name != NULL) {
            cmd->fd_r = open(cmd->filein_name, O_RDONLY);
            if (cmd->fd_r < 0) {
                printf("shell: file open error:%s\n", cmd->filein_name);
                continue;
            }
        }
        if (cmd->fileout_name != NULL) {
            cmd->fd_w = open(cmd->fileout_name, O_WRONLY | O_CREAT, 0666);
            if (cmd->fd_w < 0) {
                printf("shell: file open error: %s\n", cmd->fileout_name);
                continue;
            }
        }

        /* Pipe if there's a next cmd*/
        if (ptr->next != cmd_list) {
            if (cmd->fileout_name != NULL) {
                /* If last command redirect to file, then read the input from the file */
                fd_r  = open(cmd->fileout_name, O_RDONLY);
            } else {
                int fd[2];
                fatal(pipe(fd), 0);
                cmd->fd_w = fd[FD_WRITE_END];
                fd_r = fd[FD_READ_END];
            }
        }

        /* Frok Time! */
        pid_t my_child = fork();
        if (my_child == -1) {
            /* Parent on error */
            perror("fork failed"); exit(-1);
        } else if (my_child == 0) {
            /* Child */
            if (cmd->fd_r != STDIN_FILENO) {
                dup2(cmd->fd_r, STDIN_FILENO);      /* change stdin to pipe */
                close(cmd->fd_r);                   /* No longer needed */
            }
            if (cmd->fd_w != STDOUT_FILENO) {
                dup2(cmd->fd_w, STDOUT_FILENO);
                close(cmd->fd_w);
            }

            if (execvp(cmd->name, cmd->argv) != 0) {
                fprintf(stderr, "shell: command not found: %s\n", cmd->name);
                if (cmd->fd_r != STDIN_FILENO)
                    close(cmd->fd_r);
                if (cmd->fd_w != STDOUT_FILENO)
                    close(cmd->fd_w);
                exit(-1);
            }
            /* NEVER REACH */
        } else {
            /* Parent */
            cmd->pid = my_child;
            /* The pipe is no longer needed in parent process */
            if (cmd->fd_r != STDIN_FILENO) close(cmd->fd_r);
            if (cmd->fd_w != STDOUT_FILENO) close(cmd->fd_w);
        }
    }

    #ifdef DEBUG
    /* Test */
    LIST_FOREACH(_ptr, cmd_head) {
        cmd_t *cmd = LIST_DATA_PTR(_ptr, cmd_t);
        print_cmd(cmd);
    }
    printf("---before wait----\n");
    #endif

    /* Wait all */
    LIST_FOREACH(ptr, cmd_list) {
        cmd_t *cmd = LIST_DATA_PTR(ptr, cmd_t);
        if (cmd->pid <= 0)
            continue;
        if (waitpid(cmd->pid, &cmd->status, 0) < 0) {
            perror("waitpid error");
            return -1;
        }
    }

    return 0;
}

void INT_handler()
{
    /* Kill all of the children now */
    LIST_FOREACH(_ptr, word_head) {
        cmd_t *cmd = LIST_DATA_PTR(_ptr, cmd_t);
        if (cmd->pid > 0) {
            kill(cmd->pid, SIGKILL);
        }
    }
    siglongjmp(env, RESTART);
}