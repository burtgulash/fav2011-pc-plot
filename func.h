#ifndef FUNC_H
#define FUNC_H
#include "parser.h"

#define NUM_F 5

Operator match_operator(char c, int last);
Operator match_fun(char * fun_str);

enum {LEFT, RIGHT};

#endif
