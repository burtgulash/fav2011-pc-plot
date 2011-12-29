#ifndef PLOT_H
#define PLOT_H

/* Data structure containing limits for plot */
typedef struct {
    double x_low, x_high, y_low, y_high;
} Limits;

/* Evaluates and plots parsed arithmetic expression and writes all
 * PostScript commands to file out
 */
void write_ps(FILE * out, parsed_expr parsed, char *expr, Limits * lim);

#endif
