// word.h
#ifndef WORD_HEADER
#define WORD_HEADER

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

// struct OPERATOR_ALL_SYMBOL_T {
//     struct OPERATOR_SYMBOL_T pipe;
//     struct OPERATOR_SYMBOL_T re_in;
//     struct OPERATOR_SYMBOL_T re_out;
// } oper_symbols = {
//     .pipe = PIPE_SYMBOL_INIT,
//     .re_in = REDIRECT_IN_SYMBOL_INIT,
//     .re_out = REDIRECT_OUT_SYMBOL_INIT
// };
// TODO: try a better way to organize these operation symbol

/** 
 * If str is NULL, then this word is an operator, the operator string sotred in oper[]
 * @param {str} the word string. NULL if it's an operator
 * @param {oper} the operator ID. It also present as the numbers of continuous arguments
 */
typedef struct WORD_T {
    char *str;
    enum OPERATOR_ID *oper;
} word_t;


/**
 * Parse the words in inputs string
 * @return the length of the list 
 */
int parse_word(word_t *head, int max_arr_size, char *str);

#endif