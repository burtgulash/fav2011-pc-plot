#include <stdlib.h>
#include <string.h>
#include "lexer.h"


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
	union {
		double number;
		binary_op bop;
		unary_op uop;
	};
} symbol;

enum {NUM, VAR, BIN_OP, UN_OP};




symbol ** parse (char * expr)
{
	symbol ** queue = (symbol**) malloc(sizeof(symbol*) * (strlen(expr) + 1));
	int q_length = 0;
#define ENQUEUE(x) queue[q_length++] = (x)
	
	token t;
	int i = 0;
	char * tmp;
	do {
		t = next_tok(expr, i);
		tmp = (char*) malloc(sizeof(char) * (t.len + 1));
		strncpy(tmp, expr + t.pos, t.len);
		tmp[t.len] = '\0';

		switch (t.type) {
			case T_HEX:
			case T_DEC:
			case T_OCT:;
				double number = (double) strtoul(tmp, NULL, 0);
				symbol * integer = (symbol*) malloc(sizeof(symbol));
				integer->type = NUM;
				integer->number = number;

				ENQUEUE(integer);
				break;
		}

		free(tmp);
		i += t.len;
	} while (t.type != T_EOF);
	
	ENQUEUE(NULL);
	return queue;
}
