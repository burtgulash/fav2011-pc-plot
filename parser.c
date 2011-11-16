#include <stdlib.h>
#include <string.h>
#include "lexer.h"


typedef struct {
    int precedence, associativity;
    double (*eval) (double a, double b);
} binary_op;

typedef struct {
    int precedence;
    double (*eval) (double a);
} unary_op;

typedef struct {
    int type;
    double number;
    binary_op bop;
    unary_op uop;
} symbol;

enum {NUM, VAR, BIN_OP, UN_OP};




symbol ** parse (char * expr)
{
    symbol ** queue = (symbol**) malloc(sizeof(symbol*) * (strlen(expr) + 1));
    int q_length = 0;
#define ENQUEUE(x) queue[q_length++] = (x)

    token * t = NULL;
    int i = 0;

    double number;
    char * tmp;
    symbol * sym;

    do {
        if (t)
            free(t);

        t = next_tok(expr, i);
        tmp = (char*) malloc(sizeof(char) * (t->len + 1));
        strncpy(tmp, expr + t->pos, t->len);
        tmp[t->len] = '\0';

        switch (t->type) {
            case T_HEX:
            case T_DEC:
            case T_OCT:
                number = (double) strtoul(tmp, NULL, 0);
                sym = (symbol*) malloc(sizeof(symbol));
                sym->type = NUM;
                sym->number = number;

                ENQUEUE(sym);
                break;
        }

        free(tmp);
        i += t->len;
    } while (t->type != T_EOF);

    ENQUEUE(NULL);
    return queue;
}
