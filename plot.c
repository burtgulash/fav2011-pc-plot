#include "parser.h"

static double * stack;
static sp;

#define PUSH(x) stack[sp++] = (x)
#define POP()   stack[--sp]
#define PEEK()  stack[sp - 1]



void stack_init(int size)
{
	stack = (double*) calloc(size, sizeof(double));
	sp = 0;
}

double eval(parsed_expr p, double x)
{
    int i;
    symbol *sym;
	double a, b;

	sp = 0;
	for (i = 0; i < p.length; i++) {
		sym = p.expr[i];

		switch (sym->type) {
			case OP:
				a = POP();
				if (sym->op.binary) {
					b = POP();
					PUSH((sym->op.eval)(a, b));
				} else
					PUSH((sym->op.eval)(a, 0));
				break;

			case NUM:
				PUSH(sym->number);
				break;

			case VAR:
				PUSH(x);
				break;
		}
	}

	return sp[0]->number;
}

void plot(char * expr)
{
	parsed_expr parsed = parse(expr);
	stack_init(parsed.length);
}
