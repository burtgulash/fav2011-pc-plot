#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "plot.h"


Limits * parse_limits(char * lim_string)
{
    token * tok;
    Limits * limits;
    char * tmp;
    int i, lim_pos = 0, sign = 1;

    limits = (Limits*) malloc(sizeof(Limits));            
    for (i = 0; i < 4; i++) {
        if (i > 0) {
            tok = next_tok(lim_string, lim_pos);
            if (tok->type != T_COLON) {
                (void) parse_error(tok, "Delimiter expected");
                free(tok);
                free(limits);
                return NULL;
            }
            lim_pos += tok->len;
            free(tok);
        }

        sign = 1;
        tok = next_tok(lim_string, lim_pos);
        if (tok->type == T_OP && lim_string[tok->pos] == '-') {
            sign = -1;
            lim_pos += tok->len;
            free(tok);
            tok = next_tok(lim_string, lim_pos);
        }

        tmp = (char*) malloc(sizeof(char) * (tok->len + 1));
        strncpy(tmp, lim_string + tok->pos, tok->len);
        tmp[tok->len] = '\0';

        switch(tok->type) {
            case T_HEX: case T_DEC: case T_OCT:
                *((double*) limits + i) = 
                            sign * (double) strtoul(tmp, NULL, 0);
                break;
            case T_FLOAT:
                *((double*) limits + i) = 
                            sign * (double) strtod(tmp, NULL);
                break;
            default:
                (void) parse_error(tok, "Number expected");
                free(tmp);
                free(tok);
                free(limits);
                return NULL;
        }
        lim_pos += tok->len;
        free(tmp);
        free(tok);
    }

    return limits;
}


int main(int argc, char ** argv)
{
    parsed_expr parsed;
    Limits * lims;
    int exit_code = EXIT_SUCCESS;

    if (argc == 2 || argc == 3) {
        parsed = parse(argv[1]);
        if (parsed.expr != NULL) {
            if (argc == 2)
                write_ps(stdout, parsed, argv[1], NULL);
            else if (argc == 3) {
                lims = parse_limits(argv[2]);
                if (lims) {
                    write_ps(stdout, parsed, argv[1], lims);
                    free(lims);
                } else
                    exit_code = EXIT_FAILURE;
            }
        } else
            exit_code = EXIT_FAILURE;
        dispose(parsed);
    } else
        exit_code = EXIT_FAILURE;

    return exit_code;
}
