#include <math.h>
#include <string.h>
#include "lexer.h"
#include "func.h"

char * functions[NUM_F] = {"sin", "cos", "tan", "exp", "log"};

unary_op match_unary(char * fun_str)
{
	unary_op u_op;
	u_op.precedence = 6;

	if (strcmp(fun_str, "sin") == 0)
		u_op.eval = sin;

	return u_op;
}

