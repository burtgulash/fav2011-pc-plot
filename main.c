#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


int main(int argc, char ** argv)
{
    symbol * s;
    int i = 0;
    if (argc == 2) {
        symbol ** parsed = parse(argv[1]);
        if (!parsed)
            return 1;
        while (1) {
            s = parsed[i++];
            if (!s)
                break;

            switch (s->type) {
                case NUM:
                    printf("number: %f\n", s->number);
                    break;
                case VAR:
                    printf("var   : %s\n", "x");
                    break;
                case OP:
                    printf("opera : %d\n", s->op.prec);
                    break;
            }
        }
    }
    return 0;
}
