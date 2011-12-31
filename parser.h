#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

/* Operator data structure, containing information about operator properties
   that are needed by shunting yard */
typedef struct {
    /* precedence, associativity, (binary|unary) flag */
    int prec, assoc, binary;
    /* function pointer to corresponding evaluation function */
    double (*eval) (double a, double b);
} Operator;

/* Polymorphic data structure used by after parsing. It is basically
   lexical token with semantic meaning */
typedef struct {
    int type;
    /* token from which this symbol was created */
    token *tok;
    /* symbol can hold either number or operator */
    double number;
    Operator *op;
} symbol;

/* reduced set of symbol types (subset of token types) */
enum { NUM, VAR, OP, LPAREN, RPAREN };

/* parsed data structure that is returned by parser. */
typedef struct {
    int length;
    symbol **expr;
} parsed_expr;


/* Parse arithmetic expression given as a string.
 * Heavily checked, resulting parsed expression is guaranteed to be evaluable
 */
parsed_expr parse(char *expr);

/* Prints description of lexical or parsing error */
parsed_expr parse_error(token * tok, const char *error_msg);

/* deallocate all memory held by parser */
void dispose(parsed_expr p);

#endif
