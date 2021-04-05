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

#include "list.h"
#include "word.h"
#include "command.h"

//----------------------------------------------------------------
// Define
//----------------------------------------------------------------
#define FD_READ_END 0
#define FD_WRITE_END 1
#define INPUT_BUFSIZE 128

/** ----------------------------------------------------------------
 * Global Variables
 * ----------------------------------------------------------------*/
/* Shell input buffer */
char buf[INPUT_BUFSIZE];        /* Shell input buffer */

/** ----------------------------------------------------------------
 * Methods
 * ----------------------------------------------------------------*/
int INT_handler();              //TODO: INT handler
int prompt();                   // Print prompt
inline int scan_line(char *line);
inline void abort_line();

// fork and exec the commands, and then wait for all of the commands
int execute_cmd(cmd_t* cmds, int len);  // TODO: execute_cmd

/** ----------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // TODO: preprocess e.t. read .src file
    /* Init list */
    list_t *cmd_head = LIST_NEW_NODE(cmd_t);
    list_t *word_head = LIST_NEW_NODE(word_t);

    init_cmd(LIST_DATA_PTR(cmd_head, cmd_t));
    init_word(LIST_DATA_PTR(word_head, word_t));

    if (cmd_head == NULL || word_head == NULL) {
        perror("malloc error");
        perror("Cannot allocate memory at the first time");
        return -1;
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

        printf("#%s#\n", buf);
        if (parse_word(word_head, buf) != 0) {
            perror("parse_word error");
            list_free(word_head->next, word_head, &free_word_inside);   /* reset word list */
            continue;
        }
        LIST_FOREACH(_ptr, word_head) {
            word_t *word = LIST_DATA_PTR(_ptr, word_t);
            printf("(%s, %d) ", word->str, word->oper_id);
        }
        printf("\n");

        // printf("#%s#\n", basename(buf));
        fflush(stdout);
    }

    list_free(word_head->next, word_head, &free_word_inside);
    list_free(cmd_head, cmd_head, &free_cmd_inside);
    return 0;
}

int prompt()
{
    // TODO: more
    printf("I'm prompt: ");
    return 0;
}

/**
 * @return the size of the line '\n' not including. EOF if EOF countered
 */
inline int scan_line(char *line)
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

inline void abort_line()
{   
    char ch;
    while (1) {
        scanf("%c", &ch);
        if (ch == '\n') {
            break;
        }
    }
}