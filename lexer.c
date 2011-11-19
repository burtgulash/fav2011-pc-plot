#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "func.h"

#define isoctal(c) ('0' <= (c) && (c) < '8')

extern char * functions[NUM_F];
char * var_literal = "x";


static char * operators = "+-*/^";
static int isoperator(char c) 
{
    int i;
    for (i = 0; i < 5; i++)
        if (c == operators[i])
            return 1;
    return 0;
}

/* lexer states */
enum {ZERO, DOT, FRAC, DEC, XX, EE, OCT, HEX, EXP, EXP_SIGN, 
      DEC_MATCHED, OCT_MATCHED, HEX_MATCHED, FLOAT_MATCHED};


/* construct a token */
static token * make_tok(int type, int pos, int len, char * context)
{
    token * tok = (token*) malloc(sizeof(token));

    tok->type   = type;
    tok->pos    = pos;
    tok->len    = len;
    tok->context   = context;

    return tok;
}


token * next_tok(char * expr, int i)
{
    char c = expr[i];
    int f, f_len, len, var_len, state;

    if (!c)
        return make_tok(T_EOF, i, 1, expr);

    if (c == '(') {
        return make_tok(T_LPAREN, i, 1, expr);
    } else if (c == ')') {
        return make_tok(T_RPAREN, i, 1, expr);
    } else if (isoperator(c)) {
        return make_tok(T_OP, i, 1, expr);
    } else if (isspace(c)) { 
        len = 1;
        while (isspace(expr[i + len]))
            len++;
        return make_tok(T_SPACE, i, len, expr);
    } else if (isalpha(c)) {
        len = 1;
        while (isalpha(expr[i + len]))
            len++;

        var_len = strlen(var_literal);
        if (len == var_len && strncmp(expr + i, var_literal, var_len) == 0)
            return make_tok(T_VAR, i, len, expr);

        for (f = 0; f < NUM_F; f++) {
            f_len = strlen(functions[f]);
            if (len == f_len && strncmp(expr + i, functions[f], f_len) == 0)
                return make_tok(T_FUN, i, len, expr);
        }

        return make_tok(T_ERROR, i, len, expr);
    } else if (isdigit(c) || c == '.') {
        len = 0;

#define CURR expr[i + len]

        if (CURR == '.')
            state = FRAC;
        else if (CURR == '0')
            state = ZERO;
        else
            state = DEC;

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
                    else
                        return make_tok(T_ERROR, i, len, expr);
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
                    else if (len == 1)
                        return make_tok(T_ERROR, i, len, expr);
                    else if (CURR == 'e' || CURR == 'E')
                        state = EE;
                    else
                        state = FLOAT_MATCHED;
                    break;
                case EE:
                    if (CURR == '-' || CURR == '+')
                        state = EXP_SIGN;
                    else if (isdigit(CURR))
                        state = EXP;
                    else
                        return make_tok(T_ERROR, i, len, expr);
                    break;
                case EXP_SIGN:
                    if (isdigit(CURR))
                        state = EXP;
                    else
                        return make_tok(T_ERROR, i, len, expr);
                    break;
                case EXP:
                    if (isdigit(CURR))
                        state = EXP;
                    else
                        state = FLOAT_MATCHED;
                    break;

                case DEC_MATCHED:
                    return make_tok(T_DEC, i, len - 1, expr);

                case OCT_MATCHED:
                    return make_tok(T_OCT, i, len - 1, expr);

                case HEX_MATCHED:
                    return make_tok(T_HEX, i, len - 1, expr);

                case FLOAT_MATCHED:
                    return make_tok(T_FLOAT, i, len - 1, expr);
            }
        }

#undef CURR
    }

    return make_tok(T_ERROR, i, 1, expr);
}
