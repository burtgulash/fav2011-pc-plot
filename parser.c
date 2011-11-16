#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "func.h"
#include "parser.h"


symbol ** parse_error(token * tok, const char * error_msg)
{
    fprintf(stderr, "%s: \n", error_msg);
    return NULL;
}

symbol * make_symbol(int type, double number)
{
    symbol * sym = (symbol*) malloc(sizeof(symbol));

    sym->type = type;
    sym->number = number;

    return sym;
}


symbol ** parse (char * expr)
{
    int expr_len = strlen(expr);
    symbol ** output = (symbol**) malloc(sizeof(symbol*) * (expr_len + 1));
    symbol ** op_stack = (symbol**) malloc(sizeof(symbol*) * (expr_len + 1));

    int q_length = 0;
    int sp       = 0;
#define ENQUEUE(x) output[q_length++] = (x)
#define PUSH(x) op_stack[sp++] = (x)
#define PEEK() op_stack[sp - 1]
#define POP()  op_stack[--sp]

    token * t = NULL;
    int i = 0, eof = 0;

    double number;
    char * tmp;
    symbol * sym;

    do {
        t = next_tok(expr, i);
        tmp = (char*) malloc(sizeof(char) * (t->len + 1));
        strncpy(tmp, expr + t->pos, t->len);
        tmp[t->len] = '\0';

        switch (t->type) {
            case T_HEX:
            case T_DEC:
            case T_OCT:
                number = (double) strtoul(tmp, NULL, 0);
                sym = make_symbol(NUM, number);
                ENQUEUE(sym);
                break;

            case T_FLOAT:
                number = (double) strtod(tmp, NULL);
                sym = make_symbol(NUM, number);
                ENQUEUE(sym);
                break;

            case T_VAR:
                sym = make_symbol(VAR, 0);
                ENQUEUE(sym);
                break;

            case T_FUN:
                sym = make_symbol(OP, 0);
                sym->op = match_fun(tmp);

                while (sp > 0)
                    ENQUEUE(POP());

                PUSH(sym);
                break;

            case T_OP:
                sym = make_symbol(OP, 0);
                sym->op = match_operator(tmp[0]);

                while (sp > 0 && PEEK()->type == OP) {
                    if (PEEK()->op.assoc == LEFT && 
                                            PEEK()->op.prec > sym->op.prec)
                        ENQUEUE(POP());
                    else if (PEEK()->op.assoc == RIGHT && 
                                            PEEK()->op.prec >= sym->op.prec)
                        ENQUEUE(POP());
                    else
                        break;
                }

                PUSH(sym);
                break;

            case T_LPAREN:
                sym = make_symbol(LPAREN, 0);
                PUSH(sym);
                break;

            case T_RPAREN:
                while (sp > 0 && PEEK()->type != LPAREN)
                    ENQUEUE(POP());
                if (sp == 0)
                    return parse_error(t, "Missing parenthesis");
                /* pop LPAREN */
                sp--;    
                break;

            case T_ERROR:
                return parse_error(t, "Token error");
                break;

            case T_EOF:
                eof = 1;
                break;
        }

        free(tmp);
        i += t->len;
        free(t);
    } while (!eof);

    /* clear the stack */
    while (sp > 0) {
        if (PEEK()->type == LPAREN || PEEK()->type == RPAREN)
            return parse_error(t, "Mismatched parenthesis");
        ENQUEUE(POP());
    }

    /* append parse EOF */
    ENQUEUE(NULL);
    return output;
}
