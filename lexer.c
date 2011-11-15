#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NIL -2
#define END -1

static char * operators = "+-*/^";

int isoperator(char c) 
{
	int i;
	for (i = 0; i < 5; i++)
		if (c == operators[i])
			return 1;
	return 0;
}


int next_tok(char * expr, int i)
{
	char c = expr[i];
	char * sub = (char*) malloc(strlen(expr) + 1);
	int len;

	if (!c)
		return END;

	if (c == '(') {
		printf("%s\n", "lparen   : (");
	} else if (c == ')') {
		printf("%s\n", "rparen   : )");
	} if (isoperator(c)) {
		printf("operator : %c\n", c);
	} else if (isalpha(c)) {
		len = 1;
		while (isalpha(expr[i + len]))
			len++;
		
		strncpy(sub, expr + i, len);
		sub[len] = '\0';
		printf("symbol   : %s\n", sub);
		return len;
	} else if (isdigit(c) || c == '.') {
		len = 1;
		if (c == '.')
			goto frac; 
		else if (c == '0')
			goto zero;
		else
			goto dec;

#define CURR expr[i + len]
#define isoctal(x) ('0' <= (x) && (x) < '8')

		zero:
			if (CURR == 'x' || CURR == 'X') {
				len ++;
				goto xx;
			} else if (isoctal(CURR)) {
				len ++;
				goto oct;
			} else if (CURR == '.') {
				len ++;
				goto dot;
			} else
				goto dec_matched;

		dec:
			if (isdigit(CURR)) {
				len ++;
				goto dec;
			} else if (CURR == '.') {
				len ++;
				goto dot;
			} else
				goto dec_matched;

		xx:
			if (isxdigit(CURR)) {
				len ++;
				goto hex;
			} else
				return NIL;

		hex:
			if (isxdigit(CURR)) {
				len ++;
				goto hex;
			} else 
				goto hex_matched;

		oct:
			if (isoctal(CURR)) {
				len ++;
				goto oct;
			} else if (CURR == '.') {
				len ++;
				goto dot;
			} else
				goto oct_matched;

		dot:
			if (isdigit(CURR)) {
				len ++;
				goto frac;
			} else
				goto float_matched;

		frac:
			if (isdigit(CURR)) {
				len ++;
				goto frac;
			} else if (len == 1) {
				return NIL;
			} else if (CURR == 'e' || CURR == 'E') {
				len ++;
				goto ee;
			} else
				goto float_matched;

		ee:
			if (CURR == '-' || CURR == '+') {
				len ++;
				goto exp_sign;
			} else if (isdigit(CURR)) {
				len ++;
				goto exp;
			 } else
				return NIL;

		exp_sign:
			if (isdigit(CURR)) {
				len ++;
				goto exp;
			} else
				return NIL;

		exp:
			if (isdigit(CURR)) {
				len ++;
				goto exp;
			} else
				goto float_matched;


		dec_matched:
			strncpy(sub, expr + i, len);
			sub[len] = '\0';
			printf("decimal  : %s\n", sub);
			return len;

		oct_matched:
			strncpy(sub, expr + i, len);
			sub[len] = '\0';
			printf("octal    : %s\n", sub);
			return len;

		hex_matched:
			strncpy(sub, expr + i, len);
			sub[len] = '\0';
			printf("hex      : %s\n", sub);
			return len;

		float_matched:
			strncpy(sub, expr + i, len);
			sub[len] = '\0';
			printf("float    : %s\n", sub);
			return len;

#undef isoctal
#undef CURR

	} else {
		/* error return */
	}

	if (sub)
		free(sub);

	return 1;
}

int main(int argc, char ** argv)
{
	if (argc == 2) {
		int i = 0, len = 0;
		do {
			i += len;
			len = next_tok(argv[1], i);
			if (len == NIL) {
				fprintf(stderr, "token error\n");
				return 1;
			}
		} while (len != END);
	}
	return 0;
}
