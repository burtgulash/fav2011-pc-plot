#include <math.h>
#include <string.h>
#include "lexer.h"
#include "func.h"

char * functions[NUM_F] = {"sin", "cos", "tan", "exp", "log"};

double Sin(double a, double b)
{
    return sin(a);
}


Operator match_operator(char c, int last)
{
    Operator op;
    op.assoc = LEFT;
    op.binary = 1;

    switch (c) {
        case '-':
            op.prec = 1;
            if (last == T_LPAREN || last == T_OP) {
				op.prec   = 4;
				op.assoc  = RIGHT;
                op.binary = 0;
			}
            break;
        case '+':
            op.prec = 1;
            break;
        case '/':
		case '*':
            op.prec = 2;
            break;
        case '^':
            op.prec = 3;
            op.assoc = RIGHT;
            break;
    }

    return op;
}

Operator match_fun(char * fun_str)
{
    Operator op;
    op.prec = 5;
    op.assoc = RIGHT;
    op.binary = 0;

    if (strcmp(fun_str, "sin") == 0)
        op.eval = Sin;

    return op;
}

