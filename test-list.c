#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "command.h"

#define ASSERT(a, b) \
    if (a != b) { \
        perror("assert failed"); \
        exit(-1); \
    }

int main(int argc, char **argv)
{
    printf("Start testing...\n");

    list_t *cmd_list = LIST_NEW_NODE(cmd_t);
    init_cmd(LIST_DATA_PTR(cmd_list, cmd_t));

    for (int i = 0; i < 5; i++) {
        list_t *node = LIST_NEW_NODE(cmd_t);
        LIST_DATA_PTR(node, cmd_t)->fd_r = i;

        if (list_push_back(cmd_list, node) != 0) {
            perror("list_push_back failed!");
            return -1;
        }
    }

    int cnt = 0;
    LIST_FOREACH(ptr, cmd_list) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
        ASSERT(LIST_DATA_PTR(ptr, cmd_t)->fd_r, cnt++) 
    }
    printf("\n");
    printf("list_push_back OK!\n");

    list_t *tmp = cmd_list->next;
    list_t *next = list_get_next(cmd_list, tmp, sizeof(cmd_t));
    ASSERT(tmp->next, next);

    list_t *tail = cmd_list->prev;
    next = list_get_next(cmd_list, tail, sizeof(cmd_list));
    ASSERT(tail->next, next);
    printf("list_get_next OK!\n");


    /* Use `valgrind` to test free methods */
    list_free(cmd_list->next,  cmd_list, &_free_cmd_inside);
    free(cmd_list);

    printf("Done! everything seems OK!\n");
    fflush(stdout);
    return 0;
}