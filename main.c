/*
 * main.c
 * 
 * Entry point for plotting utility.
 * Serves mainly as file handler and catches basic input errors.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "plot.h"

#define USAGE "\nusage: graph.exe FUNCTION FILE [LIMITS]\n"
#define FILE_OPENING_ERROR "error opening file\n"

/* 
 * Parses string containing encoded plot limits and writes then to 'limits' 
 * returns 1 on success, 0 if parse error.
 */
static int parse_limits(Limits * limits, char *lim_string)
{
    token *tok;
    char *tmp;
    int i, lim_pos = 0, sign = 1;

    if (!limits)
        return 0;

    /* parse all 4 limits */
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
        /* minus does not bind to a number, it behaves like an unary operator */
        if (tok->type == T_OP && lim_string[tok->pos] == '-') {
            sign = -1;
            lim_pos += tok->len;
            free(tok);
            tok = next_tok(lim_string, lim_pos);
        }

        tmp = (char *) malloc(sizeof(char) * (tok->len + 1));
        assert(tmp);

        strncpy(tmp, lim_string + tok->pos, tok->len);
        tmp[tok->len] = '\0';

        /* parse number according to its type */
        switch (tok->type) {
        case T_HEX:
        case T_DEC:
        case T_OCT:
            *((double *) limits + i) =
                sign * (double) strtoul(tmp, NULL, 0);
            break;
        case T_FLOAT:
            *((double *) limits + i) = sign * (double) strtod(tmp, NULL);
            break;
            /* handle errors by the same function that is used by main parser */
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


    /* Perform check on limits, do not allow reversed x-axis */
    if (limits->x_low >= limits->x_high || limits->y_low >= limits->y_high) {
        fprintf(stderr, "Low limit must be less than high limit\n");
        return 0;
    }

    return 1;
}


/* Tries to parse input parameters
 * and write a valid postscript file on success.
 * Returns to OS with exit code indicating success or failure.
 */
int main(int argc, char **argv)
{
    parsed_expr parsed;
    FILE *plot_file;
    Limits *lims;
    int exit_code = EXIT_SUCCESS;

    /* if argument count is not valid, fail immediately */
    if (argc == 3 || argc == 4) {
        /* parse function expression */
        parsed = parse(argv[1]);
        /* parsing error is indicated by null expression */
        if (parsed.expr != NULL) {
            /* check if argument 'limits' was provided */
            if (argc == 3) {
                plot_file = fopen(argv[2], "w");
                if (plot_file != NULL) {
                    write_ps(plot_file, parsed, argv[1], NULL);
                    (void) fclose(plot_file);
                } else {
                    perror(FILE_OPENING_ERROR);
                    exit_code = EXIT_FAILURE;
                }
            } else {
                /* try to parse limits */
                lims = (Limits *) malloc(sizeof(Limits));
                assert(lims);

                if (parse_limits(lims, argv[3])) {
                    plot_file = fopen(argv[2], "w");
                    if (plot_file != NULL) {
                        write_ps(plot_file, parsed, argv[1], lims);
                        (void) fclose(plot_file);
                    } else {
                        perror(FILE_OPENING_ERROR);
                        exit_code = EXIT_FAILURE;
                    }
                } else
                    exit_code = EXIT_FAILURE;
                free(lims);
            }
        } else
            exit_code = EXIT_FAILURE;

        /* deallocate all memory held by parsed expression */
        dispose(parsed);
    } else
        exit_code = EXIT_FAILURE;

    /* print usage string in case of any error */
    if (exit_code == EXIT_FAILURE)
        fprintf(stderr, USAGE);

    return exit_code;
}
