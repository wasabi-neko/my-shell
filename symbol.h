// symbol.h
#ifndef SYMBOL_HEADERR
#define SYMBOL_HEADERR

#define NOT_A_SYMBOL_INIT {OPER_START, 0, 0}
#define PIPE_SYMBOL_INIT {OPER_PIPE, "|", 1}
#define REDIRECT_IN_SYMBOL_INIT {OPER_REDIRECT_IN, "<", 0}
#define REDIRECT_OUT_SYMBOL_INIT {OPER_REDIRECT_OUT, ">", 0}

#define IS_NOT_A_SYMBOL(symbol) (symbol.id == OPER_START)

#define OPER_SYMBOL_N (OPER_END - OPER_START - 1)    /* Auto calculate the size of enum */

enum OPERATOR_ID {
    OPER_START = 0,
    OPER_PIPE,
    OPER_REDIRECT_IN,
    OPER_REDIRECT_OUT,
    OPER_END,
};

typedef struct OPERATOR_SYMBOL_T {
    enum OPERATOR_ID id;
    char *str;
    int break_cmd;
}oper_symbol_t;

typedef struct _ALL_OPERATOR_SYMBOLS_t{
    struct OPERATOR_SYMBOL_T not_a_symbol;
    struct OPERATOR_SYMBOL_T pipe;
    struct OPERATOR_SYMBOL_T re_in;
    struct OPERATOR_SYMBOL_T re_out;
} _all_operator_symbols_t;

const _all_operator_symbols_t oper_symbols;

static const struct OPERATOR_SYMBOL_T _OPERATOR_SYMBOL_ARR[OPER_END - OPER_START - 1];

// TODO: can only handle the operator which symbol is a single character; cannot handle string
// TODO: many thing is hard-coded, BAD
/**
 * @return the operator which match the character. NULL if not matchs found
 */
oper_symbol_t find_operator_symbol(const char *ch);

#endif  // ifndef SYMBOL_HEADERR