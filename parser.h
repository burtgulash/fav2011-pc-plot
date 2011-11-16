#ifndef PARSER_H
#define PARSER_H

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
    binary_op b_op;
    unary_op u_op;
} symbol;

enum {NUM, VAR, BIN_OP, UN_OP};

#endif
