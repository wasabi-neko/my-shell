// word.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbol.h"
#include "list.h"
#include "word.h"

int init_word(word_t *word)
{
    word->str = NULL;
    word->oper_id = -1;
    return 0;
}

void free_word_inside(void* word)
{
    if (((word_t *)word)->str != NULL) {
        free(((word_t *)word)->str);
    }
}

/**
 * If the head has a list already, the function will not malloc new node for it
 * !Attantion! this function will change the value of the input `str`
 * @param head {list_t{word_t}}
 * @param str {char*} input string
 * @return the length of the list. -1 if the method failed
 */
int parse_word(list_t *head, char *str)
{
    if (head == NULL) {
        return -1;
    }
    if (str == NULL || strlen(str) <= 0 || *str == '\n') {
        return -1;
    }
        
    /* For each argument */
    char *read_ptr = str;
    int end_of_str = 0;
    list_t *cur_word = head;
    int *argc = NULL;           /* will point to the `oper_id` of the first word(argument) */

    while (!end_of_str) {
        // #ifdef DEBUG
        // LIST_FOREACH(ptr, head) {
        //     word_t *_ptr = LIST_DATA_PTR(ptr, word_t);
        //     printf("(%s %d),", _ptr->str, _ptr->oper_id);
        // }
        // printf("\n");
        // #endif


        /* Read one word until space or operator symbol */
        char *word_str_ptr = read_ptr;      /* init the staring position */
        oper_symbol_t symbol = oper_symbols.not_a_symbol;
        int word_len = 0;
        while (1) {
            if (*read_ptr == '\n' || *read_ptr == '\0') {
                end_of_str = 1;
                *read_ptr = '\0';
                read_ptr++;
                break;
            } if (*read_ptr == ' ') {
                *read_ptr = '\0';
                read_ptr++;
                break;
            } else {
                symbol = find_operator_symbol(read_ptr);
                if (!IS_NOT_A_SYMBOL(symbol)) {
                    *read_ptr = '\0';
                    read_ptr++;
                    break;
                }
            }
            read_ptr++;
        } /* END read one word */
        word_len = strlen(word_str_ptr) + 1;    /* !Attantion the `word_len` will be 0 when encountered an operator*/

        if (word_len > 1) {
            /* Get next word_node(list_t) */
            cur_word = list_get_next(head, cur_word, sizeof(word_t));
            if (cur_word == NULL) {
                return -1;
            }

            /* Set new argc target(the first argument of the command) */
            if (argc == NULL) {
                argc = &(LIST_DATA_PTR(cur_word, word_t)->oper_id);
                *argc = 0;
            }

            /* Store one word into current cur_word,
                if the space for string is inefficient then allocate a new space */
            word_t *word_ptr = LIST_DATA_PTR(cur_word, word_t);
            if (word_ptr->str == NULL) {
                word_ptr->str = malloc(word_len);
                if (word_ptr->str == NULL) {
                    perror("malloc error");
                    return -1;
                }

            } else if (word_len > strlen(word_ptr->str)) {
                word_ptr->str = realloc(word_ptr->str, word_len);
                if (word_ptr->str == NULL) {
                    perror("malloc error");
                    return -1;
                }
            }
            /* Copy the string */
            strncpy(word_ptr->str, word_str_ptr, word_len);
            word_ptr->str[word_len - 1] = '\0';
            /* Increse the arguments count */
            (*argc)++;
            word_ptr = NULL;
        }
        /* End of saveing a word */

        /* Deal with the operator if there's any*/
        if (!IS_NOT_A_SYMBOL(symbol)) {
            /* the next word may be a old used memroy from last command */
            cur_word = list_get_next(head, cur_word, sizeof(word_t));

            if (cur_word == NULL) {
                return -1;
            }

            if (LIST_DATA_PTR(cur_word, word_t)->str != NULL) {
                free(LIST_DATA_PTR(cur_word, word_t)->str);
            }
            LIST_DATA_PTR(cur_word, word_t)->str = NULL;
            LIST_DATA_PTR(cur_word, word_t)->oper_id = symbol.id;

            // /* break the continuous arguments, reset the counter */
            argc = NULL;
        }

    } /* END for each argument */

    /* Free the node in the linked-list that is not needed */
    if (list_free(cur_word->next, head, &free_word_inside) != 0) {
        return -1;
    }
    
    return 0;
}
