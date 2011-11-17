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
                    switch (s->op.prec) {
                        case 1:
                            printf(" -");
                            break;
                        case 2:
                            printf(" +");
                            break;
                        case 3:
                            printf(" /");
                            break;
                        case 4:
                            printf(" *");
                            break;
                        case 5:
                            printf(" ^");
                            break;
                    }
                    break;
            }
        }
        printf("\n");

        delete(parsed);
    }

    return 0;
}
