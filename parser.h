#ifndef PARSER_H
#define PARSER_H

typedef struct {
    int prec, assoc;
    double (*eval) (double a, double b);
} Operator;

typedef struct {
    int type;
    double number;
    int prec;
    Operator op;
} symbol;

enum {NUM, VAR, OP, LPAREN, RPAREN};

#endif
