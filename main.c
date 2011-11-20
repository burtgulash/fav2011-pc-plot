#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "plot.h"


/* 
 * Parses string containing encoded plot limits and writes then to 'limits' 
 * returns 1 on success, 0 if parse error.
 */
int parse_limits(Limits * limits, char * lim_string)
{
    token * tok;
    char * tmp;
    int i, lim_pos = 0, sign = 1;

    if (!limits)
        return 0;

    for (i = 0; i < 4; i++) {
        if (i > 0) {
            tok = next_tok(lim_string, lim_pos);
            if (tok->type != T_COLON) {
                (void) parse_error(tok, "Delimiter expected");
                free(tok);
                return 0;
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
                return 0;
        }
        lim_pos += tok->len;
        free(tmp);
        free(tok);
    }

    /* check if last token is eof */
    tok = next_tok(lim_string, lim_pos);
    if (tok->type != T_EOF) {
        (void) parse_error(tok, "Redundant");
        free(tok);
        return 0;
    }
    free(tok);



    if (limits->x_low >= limits->x_high || limits->y_low >= limits->y_high) {
        fprintf(stderr, "Low limit must be less than high limit\n");
        return 0;
    }

    return 1;
}


int main(int argc, char ** argv)
{
    parsed_expr parsed;
    FILE * plot_file;
    Limits * lims;
    int exit_code = EXIT_SUCCESS;

    if (argc == 3 || argc == 4) {
        parsed = parse(argv[1]);
        if (parsed.expr != NULL) {
            plot_file = fopen(argv[2], "w");
            if (plot_file != NULL) {
                if (argc == 3)
                    write_ps(plot_file, parsed, argv[1], NULL);
                else {
                    lims = (Limits*) malloc(sizeof(Limits));            
                    if(parse_limits(lims, argv[3]))
                        write_ps(plot_file, parsed, argv[1], lims);
                    else
                        exit_code = EXIT_FAILURE;
                    free(lims);
                }

                fclose(plot_file);
            } else {
                perror("Error opening file");
                exit_code = EXIT_FAILURE;
            }
        } else
            exit_code = EXIT_FAILURE;

        dispose(parsed);
    } else
        exit_code = EXIT_FAILURE;

    return exit_code;
}
