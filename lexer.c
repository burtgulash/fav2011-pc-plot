#include <string.h>
#include "lexer.h"


static char * var_literal = "x";

#define NUM_F 5
static char * functions[NUM_F]  = {"sin", "cos", "tan", "exp", "log"};


static char * operators = "+-*/^";
static int isoperator(char c) 
{
    int i;
    for (i = 0; i < 5; i++)
        if (c == operators[i])
            return 1;
    return 0;
}

static int isoctal(char c)
{
    return '0' <= c && c < '8';
}


enum {ZERO, DOT, FRAC, DEC, XX, EE, OCT, HEX, EXP, EXP_SIGN, 
      DEC_MATCHED, OCT_MATCHED, HEX_MATCHED, FLOAT_MATCHED};

token next_tok(char * expr, int i)
{
    char c = expr[i];
    int len;

    if (!c) {
        token eof = {T_EOF, i, 1, expr};
        return eof;
    }

    if (c == '(') {
        token lparen = {T_LPAREN, i, 1, expr};
        return lparen;
    } else if (c == ')') {
        token rparen = {T_RPAREN, i, 1, expr};
        return rparen;
    } else if (isoperator(c)) {
        token op = {T_OP, i, 1, expr};
        return op;
    } else if (isspace(c)) { 
        len = 1;
        while (isspace(expr[i + len]))
            len++;
        token space = {T_SPACE, i, len, expr};
        return space;
    } else if (isalpha(c)) {
        len = 1;
        while (isalpha(expr[i + len]))
            len++;

        int var_len = strlen(var_literal);
        if (len == var_len && strncmp(expr + i, var_literal, var_len) == 0) {
            token var = {T_VAR, i, len, expr};
            return var;
        } 

        size_t f;
        for (f = 0; f < NUM_F; f++) {
            int f_len = strlen(functions[f]);
            if (len == f_len && strncmp(expr + i, functions[f], f_len) == 0) {
                token fun = {T_FUN, i, len, expr};
                return fun;
            }
        }

        token error = {T_ERROR, i, len, expr};
        return error;
    } else if (isdigit(c) || c == '.') {
        len = 0;
        int state;

#define CURR expr[i + len]

        if (CURR == '.')
            state = FRAC;
        else if (CURR == '0')
            state = ZERO;
        else
            state = DEC;

        int j;
        while (1) {
            len ++;

            switch (state) {
                case ZERO:
                    if (CURR == 'x' || CURR == 'X')
                        state = XX;
                    else if (isoctal(CURR))
                        state = OCT;
                    else if (CURR == '.')
                        state = DOT;
                    else
                        state = DEC_MATCHED;
                    break;
                case DEC:
                    if (isdigit(CURR))
                        state = DEC;
                    else if (CURR == '.')
                        state = DOT;
                    else if (CURR == 'e' || CURR == 'E')
                        state = EE;
                    else
                        state = DEC_MATCHED;
                    break;
                case XX:
                    if (isxdigit(CURR))
                        state = HEX;
                    else {
                        token error = {T_ERROR, i, len, expr};
                        return error;
                    }
                    break;
                case HEX:
                    if (isxdigit(CURR))
                        state = HEX;
                    else 
                        state = HEX_MATCHED;
                    break;
                case OCT:
                    if (isoctal(CURR))
                        state = OCT;
                    else if (CURR == '.')
                        state = DOT;
                    else
                        state = OCT_MATCHED;
                    break;
                case DOT:
                    if (isdigit(CURR))
                        state = FRAC;
                    else
                        state = FLOAT_MATCHED;
                    break;
                case FRAC:
                    if (isdigit(CURR))
                        state = FRAC;
                    else if (len == 1) {
                        token error = {T_ERROR, i, len, expr};
                        return error;
                    } else if (CURR == 'e' || CURR == 'E')
                        state = EE;
                    else
                        state = FLOAT_MATCHED;
                    break;
                case EE:
                    if (CURR == '-' || CURR == '+')
                        state = EXP_SIGN;
                    else if (isdigit(CURR))
                        state = EXP;
                    else {
                        token error = {T_ERROR, i, len, expr};
                        return error;
                    } 
                    break;
                case EXP_SIGN:
                    if (isdigit(CURR))
                        state = EXP;
                    else {
                        token error = {T_ERROR, i, len, expr};
                        return error;
                    }
                    break;
                case EXP:
                    if (isdigit(CURR))
                        state = EXP;
                    else
                        state = FLOAT_MATCHED;
                    break;

                case DEC_MATCHED:;
                    token dec = {T_DEC, i, len - 1, expr};
                    return dec;

                case OCT_MATCHED:;
                    token oct = {T_OCT, i, len - 1, expr};
                    return oct;

                case HEX_MATCHED:;
                    token hex = {T_HEX, i, len - 1, expr};
                    return hex;

                case FLOAT_MATCHED:;
                    token flo = {T_FLOAT, i, len - 1, expr};
                    return flo;
            }
        }

#undef CURR
    }

    token error = {T_ERROR, i, 1, expr};
    return error;
}
