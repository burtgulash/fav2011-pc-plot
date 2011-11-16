#include <math.h>
#include <string.h>
#include "lexer.h"
#include "func.h"

char * functions[NUM_F] = {"sin", "cos", "tan", "exp", "log"};

double Sin(double a, double b)
{
    return sin(a);
}


Operator match_operator(char c)
{
    Operator op;

    switch (c) {
        case '-':
            op.prec = 1;
            op.assoc = LEFT;
            break;
        case '+':
            op.prec = 2;
            op.assoc = LEFT;
            break;
        case '/':
            op.prec = 3;
            op.assoc = LEFT;
            break;
        case '*':
            op.prec = 4;
            op.assoc = LEFT;
            break;
        case '^':
            op.prec = 5;
            op.assoc = RIGHT;
            break;
    }

    return op;
}

Operator match_fun(char * fun_str)
{
    Operator op;
    op.prec = 6;

    if (strcmp(fun_str, "sin") == 0)
        op.eval = Sin;

    return op;
}

