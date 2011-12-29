/*
 * parser.c
 *
 * Parser module. Implements Shunting yard algorithm to infix expression 
 * to postfix. Before returning parsed postfix expression, a blind evaluation
 * is performed on the result to detect remaining errors. Resulting 
 * expression is thus guaranteed to be evaluable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "func.h"
#include "parser.h"


/* stack holding symbol objects. Serves as data structure for shunting yard
 * and for blind evaluation.
 */
static symbol **stack;
/* stack pointer */
static int sp = 0;

/* stack operations for this stack */
#define PUSH(x) stack[sp++] = (x)
#define POP()   stack[--sp]
#define PEEK()  stack[sp - 1]


/* Helper data structures holding references to tokens and symbols.
 * Deallocation of every used object is then performed by simply 
 * running through these and deallocating each element.
 */
static token **tokens;
static symbol **symbols;

/* token and symbol counts */
static int num_tok = 0;
static int num_sym = 0;


/* initialize parser, that is allocate stack and helper data structures */
static void parser_init(int size)
{
    stack = (symbol **) calloc(size, sizeof(symbol *));
    symbols = (symbol **) calloc(size, sizeof(symbol *));
    /* eof token => size + 1 */
    tokens = (token **) calloc(size + 1, sizeof(token *));
}


/* deallocate all objects at once after parsed expression
 * was successfuly plotted.
 */
void dispose(parsed_expr p)
{
    int i;
    for (i = 0; i < num_tok; i++)
        free(tokens[i]);
    for (i = 0; i < num_sym; i++)
        free(symbols[i]);

    if (p.expr)
        free(p.expr);
    free(stack);
    free(tokens);
    free(symbols);

    stack = NULL;
    p.expr = NULL;
    tokens = NULL;
    symbols = NULL;
}


/* Prints error message pointing to position of error that occured.
 * Returns empty expression to indicate failure 
 */
parsed_expr parse_error(token * tok, const char *error_msg)
{
    parsed_expr error_expr = { 0, NULL };
    fprintf(stderr, "%s: %s\n", error_msg, tok->context);
    fprintf(stderr, "%*s^\n", (int) strlen(error_msg) + 2 + tok->pos, "");

    return error_expr;
}


/* Helper function for creating empty symbols */
static symbol *make_symbol(int type, token * tok, double number)
{
    symbol *sym = (symbol *) malloc(sizeof(symbol));

    sym->type = type;
    sym->tok = tok;
    sym->number = number;

    symbols[num_sym++] = sym;

    return sym;
}


/* Blind evaluation, detects function n-arity errors and
 * errors caused by wrong order of symbols. (consecutive numbers or operators)
 */
static parsed_expr check(int length, symbol ** queue)
{
    parsed_expr result;

    int i, qp = 0;
    symbol *sym, *tmp;
    symbol **output = (symbol **) calloc(length, sizeof(symbol *));

/* abstraction for adding finished symbol to output queue */
#define ENQUEUE(x) output[qp++] = (x)

/* error first deallocates data structures, then prints error message and
 * exits
 */
#define EVAL_ERROR(msg) { \
                    free(queue); \
                    free(output); \
                    return parse_error(sym->tok, msg); \
                        }

    sp = 0;
    for (i = 0; i < length; i++) {
        sym = queue[i];
        if (sym->type == OP) {
            if (sp <= 0)
                EVAL_ERROR("Missing first operand");

            tmp = POP();
            if (tmp->type != NUM && tmp->type != VAR)
                EVAL_ERROR("Number or variable expected");


            if (sym->op.binary) {
                if (sp <= 0)
                    EVAL_ERROR("Missing second operand");

                tmp = POP();
                if (tmp->type != NUM && tmp->type != VAR)
                    EVAL_ERROR("Number or variable expected");
            }

            PUSH(tmp);
        } else if (sym->type == NUM || sym->type == VAR) {
            PUSH(sym);
        }

        ENQUEUE(sym);
    }

#undef ENQUEUE

    if (sp != 1) {
        free(queue);
        free(output);
        return parse_error(sym->tok, "Missing operator");
    }

    free(queue);

    result.length = length;
    result.expr = output;
    return result;
}


/* Shunting yard implementation.
 * Takes string representation of function and returns a list (queue)
 * of symbols in postfix order.
 *
 * String expression is tokenized first and then the infix order of symbols
 * is converted to postfix using stack data structure.
 */
parsed_expr parse(char *expr)
{
    int expr_len = strlen(expr);
    int qp = 0;
    /* init queue */
    symbol **queue = (symbol **) calloc(expr_len, sizeof(symbol *));
#define ENQUEUE(x) queue[qp++] = (x)

    int i = 0, eof = 0, last = T_LPAREN;
    double number;
    char *tmp;
    symbol *sym;
    token *t;


    /* init stack and trash data structures */
    parser_init(expr_len);


    /* Draw a token, give it a semantic meaning 
     * and perform shunting yard operations on it.
     */
    do {
        t = next_tok(expr, i);
        tokens[num_tok++] = t;

        tmp = (char *) malloc(sizeof(char) * (t->len + 1));
        strncpy(tmp, expr + t->pos, t->len);
        tmp[t->len] = '\0';


/* In case of syntax error, these steps are always the same */
#define SYNTAX_ERROR() { \
                           free(queue); \
                           free(tmp); \
                           return parse_error(t, "Syntax error"); \
                       }

/* Macro to detect if token is of numeric type */
#define NUMERIC(x) ((x) == T_VAR || (x) == T_HEX || (x) == T_OCT || \
                    (x) == T_DEC || (x) == T_FLOAT)

        switch (t->type) {
		/* tokens holding numbers are handled first */
        case T_HEX:
        case T_DEC:
        case T_OCT:
            if (NUMERIC(last) || last == T_FUN || last == T_RPAREN)
                SYNTAX_ERROR();

            number = (double) strtoul(tmp, NULL, 0);
            sym = make_symbol(NUM, t, number);
            ENQUEUE(sym);
            break;

        case T_FLOAT:
            if (NUMERIC(last) || last == T_FUN || last == T_RPAREN)
                SYNTAX_ERROR();

            number = (double) strtod(tmp, NULL);
            sym = make_symbol(NUM, t, number);
            ENQUEUE(sym);
            break;

        case T_VAR:
            if (NUMERIC(last) || last == T_FUN || last == T_RPAREN)
                SYNTAX_ERROR();

            sym = make_symbol(VAR, t, 0);
            ENQUEUE(sym);
            break;

		/* function token is already checked to correspond to a function by
         * lexer. math_fun() gives this token its eval() function. */
        case T_FUN:
            if (NUMERIC(last) || last == T_FUN || last == T_RPAREN)
                SYNTAX_ERROR();

            sym = make_symbol(OP, t, 0);
            sym->op = match_fun(tmp);
            PUSH(sym);
            break;

		/* case of operator */
        case T_OP:
            if (last == T_FUN)
                SYNTAX_ERROR();

            sym = make_symbol(OP, t, 0);
            sym->op = match_operator(tmp[0], last);

            /* exclude unary minus */
            if (sym->op.binary && (last == T_OP || last == T_LPAREN))
                SYNTAX_ERROR();


			/* Shunting yard algorithm loop, holds operators until they
             * are ready to be popped off based on their precedence and
             * associativity. */
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

            PUSH(sym);
            break;

        case T_LPAREN:
            if (NUMERIC(last) || last == T_RPAREN)
                SYNTAX_ERROR();

            sym = make_symbol(LPAREN, t, 0);
            PUSH(sym);
            break;

        case T_RPAREN:
            if (last == T_OP || last == T_FUN)
                SYNTAX_ERROR();
            if (last == T_LPAREN) {
                free(queue);
                free(tmp);
                return parse_error(t, "Empty subexpression");
            }

			/* pop off every symbol that is not left parenthesis */
            while (sp > 0 && PEEK()->type != LPAREN)
                ENQUEUE(POP());

			/* closing parenthesis was expected, but is missing */
            if (sp == 0) {
                free(queue);
                free(tmp);
                return parse_error(t, "Missing parenthesis");
            }

            sym = POP();
            break;

        /* Colon is never used here, its purpose is as separator in 
         * 'limits' string. Treat is as an error. */
        case T_COLON:
        case T_ERROR:
            free(queue);
            free(tmp);
            return parse_error(t, "Unknown symbol");

        /* Ignore all whitespaces. */
        case T_SPACE:
            i += t->len;
            free(tmp);
            continue;

        /* Break out of loop if done. */
        case T_EOF:
            eof = 1;
            break;
        }

        i += t->len;
        last = t->type;

        free(tmp);
    } while (!eof);

    /* Clear the stack of remaining symbols. */
    while (sp > 0) {
        if (PEEK()->type == LPAREN || PEEK()->type == RPAREN) {
            free(queue);
            return parse_error(t, "Missing parenthesis");
        }
        ENQUEUE(POP());
    }

#undef ENQUEUE

	/* perform last check and catch remaining errors before returning result */
    return check(qp, queue);
}
