// word.h
#ifndef WORD_HEADER
#define WORD_HEADER

#define WORD_READ_BUFFER 64

/** 
 * If str is NULL, then this word is an operator, the operator string sotred in oper[]
 * @param {str} the word string. NULL if it's an operator
 * @param {oper} the operator ID. It also present as the numbers of continuous arguments in the first argument
 */
typedef struct WORD_T {
    char *str;
    int oper_id;    /* enum OPERATOR_ID */
} word_t;

int init_word(word_t *word);
void free_word_inside(void* word);

/**
 * Parse the words in inputs string
 * @return the length of the list 
 */
int parse_word(list_t *head, char *str);

#endif