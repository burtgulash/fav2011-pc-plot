#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "func.h"
#include "parser.h"



parsed_expr parse_error(token * tok, const char * error_msg)
{
    parsed_expr error_expr = {0, NULL};

    fprintf(stderr, "%s: \n", error_msg);
    free(tok);

    return error_expr;
}

symbol * make_symbol(int type, double number)
{
    symbol * sym = (symbol*) malloc(sizeof(symbol));

    sym->type = type;
    sym->number = number;

    return sym;
}


parsed_expr parse (char * expr)
{
    int expr_len = strlen(expr);
    symbol ** queue = (symbol**) malloc(sizeof(symbol*) * expr_len);
    symbol ** parse_stack  = (symbol**) malloc(sizeof(symbol*) * expr_len);
    symbol ** eval_stack  = (symbol**) malloc(sizeof(symbol*) * expr_len);

#define ENQUEUE(x) queue[qp++] = (x)

#define P_PUSH(x) parse_stack[sp++] = (x)
#define P_POP()   parse_stack[--sp]
#define P_PEEK()  parse_stack[sp - 1]

    int qp = 0, sp = 0;
    int i = 0, eof = 0, last = T_LPAREN;

    parsed_expr result;
    token * t = NULL;

    double number;
    char *tmp;
    symbol *sym;

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
                P_PUSH(sym);
                break;

            case T_OP:
                sym = make_symbol(OP, 0);
                sym->op = match_operator(tmp[0], last);

                if (sym->op.binary) {
                    while (sp > 0 && P_PEEK()->type == OP) {
                        if (P_PEEK()->op.assoc == LEFT && 
                                P_PEEK()->op.prec >= sym->op.prec)
                            ENQUEUE(P_POP());
                        else if (P_PEEK()->op.assoc == RIGHT && 
                                P_PEEK()->op.prec > sym->op.prec)
                            ENQUEUE(P_POP());
                        else
                            break;
                    }
                }

                P_PUSH(sym);
                break;

            case T_LPAREN:
                sym = make_symbol(LPAREN, 0);
                P_PUSH(sym);
                break;

            case T_RPAREN:
                while (sp > 0 && P_PEEK()->type != LPAREN)
                    ENQUEUE(P_POP());
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

        i += t->len;
        if (t->type != T_SPACE)
            last = t->type;

        free(tmp);
        free(t);
    } while (!eof);

    /* clear the stack */
    while (sp > 0) {
        if (P_PEEK()->type == LPAREN || P_PEEK()->type == RPAREN)
            return parse_error(t, "Missing parenthesis");
        ENQUEUE(P_POP());
    }

    free(parse_stack);
    free(eval_stack);

    result.length = qp;
    result.expr   = queue;
    return result;
}
