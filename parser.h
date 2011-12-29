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
    Operator op;
} symbol;

/* reduced set of symbol types (subset of token types) */
enum { NUM, VAR, OP, LPAREN, RPAREN };

/* parsed data structure that is returned by parser. */
typedef struct {
    int length;
    symbol **expr;
} parsed_expr;


parsed_expr parse(char *expr);
parsed_expr parse_error(token * tok, const char *error_msg);
void dispose(parsed_expr p);

#endif
