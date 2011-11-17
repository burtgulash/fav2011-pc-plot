#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef struct {
    int prec, assoc, binary;
    double (*eval) (double a, double b);
} Operator;

typedef struct {
    int type;
    token * tok;
    double number;
    Operator op;
} symbol;

enum {NUM, VAR, OP, LPAREN, RPAREN};

typedef struct {
    int length;
    symbol ** expr;
} parsed_expr;


parsed_expr parse (char * expr);
void delete(parsed_expr p);

#endif
