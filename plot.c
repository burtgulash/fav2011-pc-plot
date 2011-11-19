#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "plot.h"

#define PLOT_COLOR "0 0.4 0.9"

/* bounding box LOWER-LEFT and UPPER-RIGHT corners */
#define LLX 18
#define LLY 18
#define URX 594
#define URY 774

#define BLANK 50

static double * stack;
static int sp = 0;

#define PUSH(x) stack[sp++] = (x)
#define POP()   stack[--sp]
#define PEEK()  stack[sp - 1]


double eval(parsed_expr p, double x)
{
    int i;
    symbol *sym;
    double a, b;

    sp = 0;
    for (i = 0; i < p.length; i++) {
        sym = p.expr[i];

        switch (sym->type) {
            case OP:
                a = POP();
                if (sym->op.binary) {
                    b = POP();
                    PUSH((sym->op.eval)(b, a));
                } else
                    PUSH((sym->op.eval)(a, 0));
                break;

            case NUM:
                PUSH(sym->number);
                break;

            case VAR:
                PUSH(x);
                break;
        }
    }

    return stack[0];
}



static double x_left = -10;
static double x_right = 10;
static double y_low = -10;
static double y_high = 10;
static double smoothness = 300;

static double scale_x;
static double scale_y;

static double coord_x(double x)
{
    return (x - x_left) * scale_x + LLX + BLANK;
}

static double coord_y(double y)
{
    return (y - y_low) * scale_y + LLY + BLANK;
}


int plot_init(int size, Limits * lims)
{
    if (lims) {
        x_left  = lims->x_low;
        x_right = lims->x_high;
        y_low   = lims->y_low;
        y_high  = lims->y_high;
    }

    stack = (double*) calloc(size, sizeof(double));
    sp = 0;

    scale_x = (URX - LLX - 2*BLANK) / (x_right - x_left);
    scale_y = (URY - LLY - 2*BLANK) / (y_high - y_low);
    return 1;
}



static void plot(FILE * out, parsed_expr p)
{
    double delta = (x_right - x_left) / smoothness;
    double x_1 = x_left;
    double x_2 = x_left + delta;
    double y_1 = eval(p, x_1);
    double y_2;
    /* if y-value out of box, compute intersection of line with box */
    double x_intersect;
    int last_out = 0;

    if (y_1 < y_low)
        y_1 = y_low;
    else if (y_1 > y_high)
        y_1 = y_high;

    fprintf(out, "newpath\n");
    fprintf(out, "%.3f %.3f moveto\n", coord_x(x_1), coord_y(y_1));

#define INTERSECT(boundary) (x_1 + ((boundary) - y_1) * \
                            (x_2 - x_1) / (y_2 - y_1));

#define LINETO(x, y) fprintf(out, "%.3f %.3f lineto\n", \
                             coord_x((x)), coord_y((y))) 
#define MOVETO(x, y) fprintf(out, "%.3f %.3f moveto\n", \
                             coord_x((x)), coord_y((y))) 

    /* flip order of x and y and sign of y in Landscape mode */
    while (x_2 <= x_right) {
        y_2 = eval(p, x_2);

        if (y_low <= y_2 && y_2 <= y_high) {
            if (last_out) {
                if (y_2 > y_1) {
                    x_intersect = INTERSECT(y_low);
					MOVETO(x_intersect, y_low);
                } else {
                    x_intersect = INTERSECT(y_high);
					MOVETO(x_intersect, y_high);
                }
            }
			LINETO(x_2, y_2);

            last_out = 0;
        } else {
            if (!last_out) {
                if (y_2 > y_1) {
                    x_intersect = INTERSECT(y_high);
					LINETO(x_intersect, y_high);
                } else {
                    x_intersect = INTERSECT(y_low);
					LINETO(x_intersect, y_low);
                }
            }

            last_out = 1;
        }


        x_1 = x_2;
        x_2 = x_2 + delta;
        y_1 = y_2;
    }

    fprintf(out, "%s setrgbcolor\n", PLOT_COLOR);
    fprintf(out, "0.5 setlinewidth\n");
    fprintf(out, "stroke\n");
}


static void write_header(FILE * out, char * expression)
{
    fprintf(out, "%%!PS-Adobe-3.0 EPSF 3.0\n");
    fprintf(out, "%%%%DocumentMedia: Letter %d %d 0 () ()\n", 
                                                  LLX + URX, LLY + URY);
    fprintf(out, "%%%%Title: Plot %s\n", expression);
    fprintf(out, "%%%%Creator: Plot utility\n");
    fprintf(out, "%%%%Pages: 1\n");
    /* fprintf(out, "%%%%Orientation: Landscape\n"); */
    fprintf(out, "%%%%BoundingBox: %d %d %d %d\n", LLX, LLY, URX, URY);
    fprintf(out, "%%%%EndComments\n\n");
    fprintf(out, "%%%%Page: 1 1\n");
}

static void write_box(FILE * out)
{
    fprintf(out, "newpath\n");
    fprintf(out, "%d %d moveto\n", LLX + BLANK, LLY + BLANK);
    fprintf(out, "%d %d lineto\n", URX - BLANK, LLY + BLANK);
    fprintf(out, "%d %d lineto\n", URX - BLANK, URY - BLANK);
    fprintf(out, "%d %d lineto\n", LLX + BLANK, URY - BLANK);
    fprintf(out, "closepath\n");
    fprintf(out, "0 setgray\n");
    fprintf(out, "1 setlinewidth\n");
    fprintf(out, "stroke\n");
}

static void write_footer(FILE * out)
{
    fprintf(out, "%%%%PageTrailer\n");
    fprintf(out, "%%%%Trailer\n");
    fprintf(out, "%%%%EOF\n\n");
}


void write_ps(FILE * out, parsed_expr parsed, char * expr, Limits * lim)
{
    plot_init(parsed.length, lim);

    write_header(out, expr);
    plot(out, parsed);
    write_box(out);
    write_footer(out);

    free(stack);
}
