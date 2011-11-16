#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


int main(int argc, char ** argv)
{
    if (argc == 2) {
        token * t = NULL;
        int i = 0;
        do {
            if (t)
                free(t);
            t = next_tok(argv[1], i);
            i += t->len;

            switch (t->type) {
                case T_ERROR:
                    printf("token error"); break;
                case T_FLOAT:
                    printf("float"); break;
                case T_DEC:
                    printf("decimal"); break;
                case T_SPACE:
                    printf("space"); break;
                case T_EOF:
                    printf("eof"); break;
                case T_VAR:
                    printf("var"); break;
                case T_FUN:
                    printf("fun"); break;
                case T_OP:
                    printf("operator"); break;
                case T_LPAREN:
                    printf("left parenthesis"); break;
                case T_RPAREN:
                    printf("right parenthesis"); break;
            }
            printf (" at %d, %s\n", t->pos, t->expr + t->pos);
        } while (t->type != T_EOF);
    }
    return 0;
}
