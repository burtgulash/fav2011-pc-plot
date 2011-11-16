#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define END -1

typedef enum {T_HEX, T_OCT, T_DEC, T_FLOAT, T_VAR, T_FUN, T_OP, 
              T_LPAREN, T_RPAREN, T_ERROR, T_EOF, T_SPACE} t_type;

static char * var_literal = "x";

#define NUM_F 5
static char * functions[NUM_F]  = {"sin", "cos", "tan", "exp", "log"};

typedef struct {
    t_type type;
    int pos, len;
    char * expr;
} token;

static char * operators = "+-*/^";
int isoperator(char c) 
{
    int i;
    for (i = 0; i < 5; i++)
        if (c == operators[i])
            return 1;
    return 0;
}

typedef enum {ZERO, DOT, FRAC, DEC, XX, EE, OCT, HEX, EXP, EXP_SIGN, 
              DEC_MATCHED, OCT_MATCHED, HEX_MATCHED, FLOAT_MATCHED} state;

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
    } if (isoperator(c)) {
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
        state s;

#define CURR expr[i + len]
#define isoctal(x) ('0' <= (x) && (x) < '8')

        if (CURR == '.')
            s = FRAC;
        else if (CURR == '0')
            s = ZERO;
        else
            s = DEC;

        int j;
        while (1) {
            len ++;

            switch (s) {
                case ZERO:
                    if (CURR == 'x' || CURR == 'X')
                        s = XX;
                    else if (isoctal(CURR))
                        s = OCT;
                    else if (CURR == '.')
                        s = DOT;
                    else
                        s = DEC_MATCHED;
                    break;
                case DEC:
                    if (isdigit(CURR))
                        s = DEC;
                    else if (CURR == '.')
                        s = DOT;
                    else if (CURR == 'e' || CURR == 'E')
                        s = EE;
                    else
                        s = DEC_MATCHED;
                    break;
                case XX:
                    if (isxdigit(CURR))
                        s = HEX;
                    else {
                        token error = {T_EOF, i, len, expr};
                        return error;
                    }
                    break;
                case HEX:
                    if (isxdigit(CURR))
                        s = HEX;
                    else 
                        s = HEX_MATCHED;
                    break;
                case OCT:
                    if (isoctal(CURR))
                        s = OCT;
                    else if (CURR == '.')
                        s = DOT;
                    else
                        s = OCT_MATCHED;
                    break;
                case DOT:
                    if (isdigit(CURR))
                        s = FRAC;
                    else
                        s = FLOAT_MATCHED;
                    break;
                case FRAC:
                    if (isdigit(CURR))
                        s = FRAC;
                    else if (len == 1) {
                        token error = {T_EOF, i, len, expr};
                        return error;
                    } else if (CURR == 'e' || CURR == 'E')
                        s = EE;
                    else
                        s = FLOAT_MATCHED;
                    break;
                case EE:
                    if (CURR == '-' || CURR == '+')
                        s = EXP_SIGN;
                    else if (isdigit(CURR))
                        s = EXP;
                    else {
                        token error = {T_EOF, i, len, expr};
                        return error;
                    } 
                    break;
                case EXP_SIGN:
                    if (isdigit(CURR))
                        s = EXP;
                    else {
                        token error = {T_EOF, i, len, expr};
                        return error;
                    }
                    break;
                case EXP:
                    if (isdigit(CURR))
                        s = EXP;
                    else
                        s = FLOAT_MATCHED;
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

#undef isoctal
#undef CURR
    }

    token error = {T_EOF, i, 1, expr};
    return error;
}

int main(int argc, char ** argv)
{
    if (argc == 2) {
        token t;
        int i = 0;
        do {
            t = next_tok(argv[1], i);
            i += t.len;

            switch (t.type) {
                case T_ERROR:
                    printf("token error"); break;
                case T_FLOAT:
                    printf("float"); break;
                case T_DEC:
                    printf("decimal"); break;
                case T_SPACE:
                    printf("space"); break;
                case T_EOF:
                    printf("eof"); break;
                case T_VAR:
                    printf("var"); break;
                case T_FUN:
                    printf("fun"); break;
                case T_OP:
                    printf("operator"); break;
                case T_LPAREN:
                    printf("left parenthesis"); break;
                case T_RPAREN:
                    printf("right parenthesis"); break;
            }
            printf (" at %d, %s\n", t.pos, t.expr + t.pos);
        } while (t.type != T_EOF);
    }
    return 0;
}
