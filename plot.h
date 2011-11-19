#ifndef PLOT_H
#define PLOT_H

typedef struct {
    double x_low, x_high, y_low, y_high;
} Limits;

void write_ps(FILE * out, parsed_expr parsed, char * expr, Limits * lim);

#endif
