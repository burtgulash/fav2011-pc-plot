/*
 * func.c
 * Provides wrappers for math functions and utility functions for creating 
 * operator symbols.
*/

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include "lexer.h"
#include "func.h"

#define IS_NAN(x) ((x) != (x))

/* list of all functions that can be used in expression */
char *functions[NUM_F] = { "abs", "exp", "ln", "log",
    "sin", "cos", "tan",
    "asin", "acos", "atan",
    "sinh", "cosh", "tanh"
};

/* function wrappers. */
static double Add(double a, double b)
{
    return a + b;
}

static double Neg(double a, double b)
{
    return -a;
}

static double Sub(double a, double b)
{
    return a - b;
}

static double Mul(double a, double b)
{
    return a * b;
}

static double Div(double a, double b)
{
    return a / b;
}

static double Pow(double a, double b)
{
    return pow(a, b);
}

/* unary functions are made binary with the second argument dead */
static double Abs(double a, double b)
{
    return fabs(a);
}

static double Exp(double a, double b)
{
    return exp(a);
}

static double Ln(double a, double b)
{
    return log(a);
}

static double Log10(double a, double b)
{
    return log10(a);
}

static double Sin(double a, double b)
{
    return sin(a);
}

static double Cos(double a, double b)
{
    return cos(a);
}

static double Tan(double a, double b)
{
    return tan(a);
}

static double Asin(double a, double b)
{
    return asin(a);
}

static double Acos(double a, double b)
{
    return acos(a);
}

static double Atan(double a, double b)
{
    return atan(a);
}

static double Sinh(double a, double b)
{
    return sinh(a);
}

static double Cosh(double a, double b)
{
    return cosh(a);
}

static double Tanh(double a, double b)
{
    return tanh(a);
}


/* Creates Operator symbol from character c. 
 * 'last' token is used to detect unary minus.
 */
Operator *match_operator(char c, int last)
{
    Operator *op = (Operator *) malloc(sizeof(Operator));
    assert(op);

    op->assoc = LEFT;
    op->binary = 1;
    op->eval = NULL;
    op->prec = 0;

    switch (c) {
    case '-':
        /* find n-arity of minus operator */
        if (last == T_LPAREN || last == T_OP) {
            op->prec = 4;
            op->assoc = RIGHT;
            op->binary = 0;
            op->eval = Neg;
        } else {
            op->prec = 1;
            op->eval = Sub;
        }
        break;
    case '+':
        op->eval = Add;
        op->prec = 1;
        break;
    case '/':
        op->prec = 2;
        op->eval = Div;
        break;
    case '*':
        op->prec = 2;
        op->eval = Mul;
        break;
    case '^':
        op->prec = 3;
        op->assoc = RIGHT;
        op->eval = Pow;
        break;
    }

    assert(op->eval != NULL);
    return op;
}


/* Create Operator symbol given valid function string.
 * String checking should already be performed by lexer.
 */
Operator *match_fun(char *fun_str)
{
    Operator *op = (Operator *) malloc(sizeof(Operator));
    assert(op);

    op->prec = 5;
    op->assoc = RIGHT;
    op->binary = 0;
    op->eval = NULL;

    if (strcmp(fun_str, "abs") == 0)
        op->eval = Abs;
    else if (strcmp(fun_str, "exp") == 0)
        op->eval = Exp;
    else if (strcmp(fun_str, "ln") == 0)
        op->eval = Ln;
    else if (strcmp(fun_str, "log") == 0)
        op->eval = Log10;
    else if (strcmp(fun_str, "sin") == 0)
        op->eval = Sin;
    else if (strcmp(fun_str, "cos") == 0)
        op->eval = Cos;
    else if (strcmp(fun_str, "tan") == 0)
        op->eval = Tan;
    else if (strcmp(fun_str, "asin") == 0)
        op->eval = Asin;
    else if (strcmp(fun_str, "acos") == 0)
        op->eval = Acos;
    else if (strcmp(fun_str, "atan") == 0)
        op->eval = Atan;
    else if (strcmp(fun_str, "sinh") == 0)
        op->eval = Sinh;
    else if (strcmp(fun_str, "cosh") == 0)
        op->eval = Cosh;
    else if (strcmp(fun_str, "tanh") == 0)
        op->eval = Tanh;

    /* fail if string was not matched */
    assert(op->eval != NULL);
    return op;
}
