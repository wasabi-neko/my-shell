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
#include "command.h"
#include "word.h"

//----------------------------------------------------------------
// Define
//----------------------------------------------------------------
#define FD_READ_END 0
#define FD_WRITE_END 1

#define INPUT_BUFSIZE 10 
#define TEMP_BUFSIZE 10

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

// fork and exec the commands, and then wait for all of the commands
int execute_cmd(cmd_t* cmds, int len);  // TODO: execute_cmd

/** ----------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // TODO: preprocess e.t. read .src file

    /* Init list */
    list_t *cmd_list_head = new_list();
    list_t *word_list_head = new_list();
    if (cmd_list_head == NULL || word_list_head == NULL) {
        perror("malloc error");
        perror("Cannot allocate memory at the first time");
        return -1;
    }

    /* Main Loop */
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
