#include "symbol.h"


const _all_operator_symbols_t oper_symbols = {
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

oper_symbol_t find_operator_symbol(const char *ch)
{
    for (int i = 0; i < OPER_SYMBOL_N; i++) {
        if (*ch == *_OPERATOR_SYMBOL_ARR[i].str) {
            return _OPERATOR_SYMBOL_ARR[i];
        }
    }
    return oper_symbols.not_a_symbol;
}