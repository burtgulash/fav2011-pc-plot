#ifndef PARSER_H
#define PARSER_H

typedef struct {
    int prec, assoc, binary;
    double (*eval) (double a, double b);
} Operator;

typedef struct {
    int type;
    double number;
    Operator op;
} symbol;

enum {NUM, VAR, OP, LPAREN, RPAREN};

typedef struct {
    int length;
    symbol ** expr;
} parsed_expr;


parsed_expr parse (char * expr);

#endif
