#ifndef FUNC_H
#define FUNC_H
#include "parser.h"

/* number of built in functions */
#define NUM_F 13


/* Creates Operator symbol from character c. 
 * 'last' token is used to detect unary minus.
 */
Operator *match_operator(char c, int last);

/* Create Operator symbol given valid function string.
 * String checking should already be performed by lexer.
 */
Operator *match_fun(char *fun_str);

/* associativity of operator */
enum { LEFT, RIGHT };

#endif
