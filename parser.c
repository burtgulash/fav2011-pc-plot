#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "func.h"
#include "parser.h"


static symbol ** stack;

#define PUSH(x) stack[sp++] = (x)
#define POP()   stack[--sp]
#define PEEK()  stack[sp - 1]


void delete(int length, symbol ** structure)
{
	int i;
	for (i = 0; i < length; i++)
		if (structure[i] != NULL) {
			if (structure[i]->tok != NULL) {
				free(structure[i]->tok);
				structure[i]->tok = NULL;
			}
			free(structure[i]);
			structure[i] = NULL;
		}
	free(structure);
}

parsed_expr parse_error(token * tok, const char * error_msg)
{
    parsed_expr error_expr = {0, NULL};
    fprintf(stderr, "%s: \n", error_msg);

    return error_expr;
}

symbol * make_symbol(int type, token * tok, double number)
{
    symbol * sym = (symbol*) malloc(sizeof(symbol));

    sym->type = type;
    sym->number = number;

    return sym;
}

parsed_expr check(int length, symbol ** queue)
{
	parsed_expr result;

	int i, qp = 0, sp = 0;
	symbol *sym, *tmp;
    symbol ** output = (symbol**) calloc(length, sizeof(symbol*));
#define ENQUEUE(x) output[qp++] = (x)
	
	for (i = 0; i < length; i++) {
		sym = queue[i];
		if (sym->type == OP) {
			if (sp <= 0) {
				result = parse_error(sym->tok, "Missing first operand"); 
				delete(length, output);
				delete(sp, stack);
				free(output);	
				return result;
			}
			tmp = POP();
			if (tmp->type != NUM && tmp->type != VAR) {
				result = parse_error(sym->tok, "Number or variable expected"); 
				delete(length, queue);
				delete(sp, stack);
				free(output);
				return result;
			}

			if (sym->op.binary) {
				if (sp <= 0) {
					result = parse_error(sym->tok, "Missing second operand"); 
					delete(length, queue);
					delete(sp, stack);
					free(output);
					return result;
				}
				tmp = POP();
				if (tmp->type != NUM && tmp->type != VAR) {
					result = parse_error(sym->tok, 
                                        "Number or variable expected"); 
					delete(length, queue);
					delete(sp, stack);
					free(output);
					return result;
				}
			}
			
			PUSH(tmp);
		} else if (sym->type == NUM || sym->type == VAR) {
			PUSH(sym);
		}

		ENQUEUE(sym);
	}

#undef ENQUEUE

	/* there must be one number left on the stack */
	if (sp != 1) {
		printf("sp: %d\n", sp);
		result = parse_error(sym->tok, "Missing operator"); 
		delete(length, queue);
		delete(sp, stack);
		free(output);
		return result;
	}
	

	free(stack);
	free(queue);

	result.length = length;
	result.expr   = output;
	return result;
}


parsed_expr parse (char * expr)
{
    int expr_len = strlen(expr);
    symbol ** queue = (symbol**) calloc(expr_len, sizeof(symbol*));
#define ENQUEUE(x) queue[qp++] = (x)

    int qp = 0, sp = 0;
    int i = 0, eof = 0, last = T_LPAREN;

    token * t;
	parsed_expr err;

    double number;
    char *tmp;
    symbol *sym;

	stack = (symbol**) calloc(expr_len, sizeof(symbol*));

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
                sym = make_symbol(NUM, t, number);
                ENQUEUE(sym);
                break;

            case T_FLOAT:
                number = (double) strtod(tmp, NULL);
                sym = make_symbol(NUM, t, number);
                ENQUEUE(sym);
                break;

            case T_VAR:
                sym = make_symbol(VAR, t, 0);
                ENQUEUE(sym);
                break;

            case T_FUN:
                sym = make_symbol(OP, t, 0);
                sym->op = match_fun(tmp);
                PUSH(sym);
                break;

            case T_OP:
                sym = make_symbol(OP, t, 0);
                sym->op = match_operator(tmp[0], last);

                if (sym->op.binary) {
                    while (sp > 0 && PEEK()->type == OP) {
                        if (PEEK()->op.assoc == LEFT && 
                                PEEK()->op.prec >= sym->op.prec)
                            ENQUEUE(POP());
                        else if (PEEK()->op.assoc == RIGHT && 
                                PEEK()->op.prec > sym->op.prec)
                            ENQUEUE(POP());
                        else
                            break;
                    }
                }

                PUSH(sym);
                break;

            case T_LPAREN:
                sym = make_symbol(LPAREN, t, 0);
                PUSH(sym);
                break;

            case T_RPAREN:
                while (sp > 0 && PEEK()->type != LPAREN)
                    ENQUEUE(POP());
                if (sp == 0) {
                    err =  parse_error(t, "Missing parenthesis");
					delete(expr_len, queue);
					delete(sp, stack);
					return err;
				}

                /* pop LPAREN */
                sp--;    
                break;

            case T_ERROR:
                err = parse_error(t, "Token error");
				delete(expr_len, queue);
				delete(sp, stack);
				return err;

            case T_EOF:
                eof = 1;
                break;
        }

        i += t->len;
        if (t->type != T_SPACE)
            last = t->type;

        free(tmp);
    } while (!eof);

    /* clear the stack */
    while (sp > 0) {
        if (PEEK()->type == LPAREN || PEEK()->type == RPAREN) {
            err = parse_error(t, "Missing parenthesis");
			delete(expr_len, queue);
			delete(sp, stack);
			return err;
		}
        ENQUEUE(POP());
    }

#undef ENQUEUE

    return check(qp, queue);
}
