// symbol.h
#ifndef SYMBOL_HEADERR
#define SYMBOL_HEADERR

#define NOT_A_SYMBOL_INIT {OPER_START, 0, 0}
#define PIPE_SYMBOL_INIT {OPER_PIPE, "|", 1}
#define REDIRECT_IN_SYMBOL_INIT {OPER_REDIRECT_IN, "<", 0}
#define REDIRECT_OUT_SYMBOL_INIT {OPER_REDIRECT_OUT, ">", 0}

#define IS_NOT_A_SYMBOL(symbol) (symbol.id == OPER_START)

enum OPERATOR_ID {
    OPER_START = 0,
    OPER_PIPE,
    OPER_REDIRECT_IN,
    OPER_REDIRECT_OUT,
    OPER_END,
};
const int OPER_SYMBOL_N = OPER_END - OPER_START - 1;    /* Auto calculate the size of enum */

typedef struct OPERATOR_SYMBOL_T {
    enum OPERATOR_ID id;
    char *str;
    int break_cmd;
}oper_symbol_t;

const static struct {
    struct OPERATOR_SYMBOL_T not_a_symbol;
    struct OPERATOR_SYMBOL_T pipe;
    struct OPERATOR_SYMBOL_T re_in;
    struct OPERATOR_SYMBOL_T re_out;
} oper_symbols = {
    .not_a_symbol = NOT_A_SYMBOL_INIT,
    .pipe = PIPE_SYMBOL_INIT,
    .re_in = REDIRECT_IN_SYMBOL_INIT,
    .re_out = REDIRECT_OUT_SYMBOL_INIT
};

static const struct OPERATOR_SYMBOL_T _OPERATOR_SYMBOL_ARR[] = {
    PIPE_SYMBOL_INIT,
    REDIRECT_IN_SYMBOL_INIT,
    REDIRECT_OUT_SYMBOL_INIT,
};

// TODO: can only handle the operator which symbol is a single character; cannot handle string
// TODO: many thing is hard-coded, BAD
/**
 * @return the operator which match the character. NULL if not matchs found
 */
static inline oper_symbol_t find_operator_symbol(const char *ch)
{
    for (int i = 0; i < OPER_SYMBOL_N; i++) {
        if (*ch == *_OPERATOR_SYMBOL_ARR[i].str) {
            return _OPERATOR_SYMBOL_ARR[i];
        }
    }
    return oper_symbols.not_a_symbol;
}

#endif  // ifndef SYMBOL_HEADERR