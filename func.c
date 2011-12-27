/*
 * func.c
 * Provides wrappers for math functions and utility functions for creating 
 * operator symbols.
*/

#include <assert.h>
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
double add(double a, double b)
{
    return a + b;
}

double neg(double a, double b)
{
    return -a;
}

double sub(double a, double b)
{
    return a - b;
}

double mul(double a, double b)
{
    return a * b;
}

double div(double a, double b)
{
    return a / b;
}

double Pow(double a, double b)
{
    return pow(a, b);
}

/* unary functions are made binary with the second argument dead */
double Abs(double a, double b)
{
    return fabs(a);
}

double Exp(double a, double b)
{
    return exp(a);
}

double Ln(double a, double b)
{
    return log(a);
}

double Log10(double a, double b)
{
    return log10(a);
}

double Sin(double a, double b)
{
    return sin(a);
}

double Cos(double a, double b)
{
    return cos(a);
}

double Tan(double a, double b)
{
    return tan(a);
}

double Asin(double a, double b)
{
    return asin(a);
}

double Acos(double a, double b)
{
    return acos(a);
}

double Atan(double a, double b)
{
    return atan(a);
}

double Sinh(double a, double b)
{
    return sinh(a);
}

double Cosh(double a, double b)
{
    return cosh(a);
}

double Tanh(double a, double b)
{
    return tanh(a);
}


/* Creates Operator symbol from character c. 
 * 'last' token is used to detect unary minus.
 */
Operator match_operator(char c, int last)
{
    Operator op;
    op.assoc = LEFT;
    op.binary = 1;
    op.eval = NULL;

    switch (c) {
    case '-':
        /* find n-arity of minus operator */
        if (last == T_LPAREN || last == T_OP) {
            op.prec = 4;
            op.assoc = RIGHT;
            op.binary = 0;
            op.eval = neg;
        } else {
            op.prec = 1;
            op.eval = sub;
        }
        break;
    case '+':
        op.eval = add;
        op.prec = 1;
        break;
    case '/':
        op.prec = 2;
        op.eval = div;
        break;
    case '*':
        op.prec = 2;
        op.eval = mul;
        break;
    case '^':
        op.prec = 3;
        op.assoc = RIGHT;
        op.eval = Pow;
        break;
    }

    assert(op.eval);
    return op;
}


/* Create Operator symbol given valid function string.
 * String checking should already be performed by lexer.
 */
Operator match_fun(char *fun_str)
{
    Operator op;
    op.prec = 5;
    op.assoc = RIGHT;
    op.binary = 0;
    op.eval = NULL;

    if (strcmp(fun_str, "abs") == 0)
        op.eval = Abs;
    else if (strcmp(fun_str, "exp") == 0)
        op.eval = Exp;
    else if (strcmp(fun_str, "ln") == 0)
        op.eval = Ln;
    else if (strcmp(fun_str, "log") == 0)
        op.eval = Log10;
    else if (strcmp(fun_str, "sin") == 0)
        op.eval = Sin;
    else if (strcmp(fun_str, "cos") == 0)
        op.eval = Cos;
    else if (strcmp(fun_str, "tan") == 0)
        op.eval = Tan;
    else if (strcmp(fun_str, "asin") == 0)
        op.eval = Asin;
    else if (strcmp(fun_str, "acos") == 0)
        op.eval = Acos;
    else if (strcmp(fun_str, "atan") == 0)
        op.eval = Atan;
    else if (strcmp(fun_str, "sinh") == 0)
        op.eval = Sinh;
    else if (strcmp(fun_str, "cosh") == 0)
        op.eval = Cosh;
    else if (strcmp(fun_str, "tanh") == 0)
        op.eval = Tanh;

    /* fail if string was not matched */
    assert(op.eval);
    return op;
}
