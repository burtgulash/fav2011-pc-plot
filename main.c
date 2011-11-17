#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


int main(int argc, char ** argv)
{
    symbol * s;
    int i;
    if (argc == 2) {
        parsed_expr parsed = parse(argv[1]);
        for (i = 0; i < parsed.length; i++) {
            s = parsed.expr[i];

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

		delete(parsed);
    }

    return 0;
}
