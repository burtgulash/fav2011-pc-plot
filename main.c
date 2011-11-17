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
                    printf(" %.2f", s->number);
                    break;
                case VAR:
                    printf(" x");
                    break;
                case OP:
					printf(" op");
					break;
            }
        }
        printf("\n");

        delete(parsed);
    }

    return 0;
}
