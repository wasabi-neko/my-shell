
// ! FUCK, I make a chaos here...

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
    // TODO: there's a bug I cannot find 
    list_t *head = LIST_NEW_NODE(cmd_t);
    list_t *curr = head;

    for (int i = 0; i < 3; i++) {
        list_t *node = LIST_NEW_NODE(cmd_t);
        init_cmd(LIST_DATA_PTR(node, cmd_t));
        LIST_DATA_PTR(node, cmd_t)->fd_r = i;
        
        list_push_back(head, node);
    }

    LIST_FOREACH(ptr, head) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
    }

    for (int i = 0; i < 3; i++) {
        curr = list_get_next(head, curr, sizeof(cmd_t));
        LIST_DATA_PTR(curr, cmd_t)->fd_r = i + 3;
    }

    LIST_FOREACH(ptr, head) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
    }

    list_free(head->next, head, &free_cmd_inside);
    free(head);

    // return 0;

    printf("Start testing...\n");

    list_t *cmd_list = LIST_NEW_NODE(cmd_t);
    init_cmd(LIST_DATA_PTR(cmd_list, cmd_t));

    for (int i = 0; i < 9; i++) {
        list_t *node = LIST_NEW_NODE(cmd_t);
        if (node == NULL) {
            perror("LIST_NEW_NODE malloc failed");
            return -1;
        }
        LIST_DATA_PTR(node, cmd_t)->fd_r = i;

        if (list_push_back(cmd_list, node) != 0) {
            perror("list_push_back failed!");
            return -1;
        }
    }

    int cnt = 0;
    printf("cmd_list: ");
    LIST_FOREACH(ptr, cmd_list) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
        ASSERT(LIST_DATA_PTR(ptr, cmd_t)->fd_r, cnt++) 
    }
    printf("\n");
    printf("list_push_back OK!\n");
    // * The size of cmd_list now is 5

    list_t *tmp = cmd_list->next;
    list_t *next = list_get_next(cmd_list, tmp, sizeof(cmd_t));
    ASSERT(tmp->next, next);

    list_t *tail = cmd_list->prev;
    //* This node is newed
    ASSERT(tail->next, next);

    list_t *new_node = LIST_NEW_NODE(cmd_t);
    init_cmd(LIST_DATA_PTR(new_node, cmd_t));
    LIST_DATA_PTR(new_node, cmd_t)->fd_r = 100;
    list_push_back(cmd_list, new_node);
    
    printf("cmd_list: ");
    LIST_FOREACH(ptr, cmd_list) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
        // ASSERT(LIST_DATA_PTR(ptr, cmd_t)->fd_r, cnt++) 
    }
    printf("\n");


    printf("cmd_list again: ");
    LIST_FOREACH(ptr, cmd_list) {
        printf("%d ", LIST_DATA_PTR(ptr, cmd_t)->fd_r);
    }
    printf("\n");


    /* Use `valgrind` to test free methods */
    // list_free(cmd_list2->next, cmd_list2, &free_cmd_inside);
    // free(cmd_list2);
    list_free(cmd_list->next,  cmd_list, &free_cmd_inside);
    free(cmd_list);

    printf("Done! everything seems OK!\n");
    fflush(stdout);
    return 0;
}