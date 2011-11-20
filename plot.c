#include <stdio.h>
#include <stdlib.h>
#include <math.h> /* for fabs() */
#include "parser.h"
#include "plot.h"

/** SMOOTHING PARAMETERS **/
#define PLOT_COLOR "0 0.4 0.9"
/* higher -> more postscript lines, but smoother */
#define SMOOTHNESS 3000 
/* higher -> less postscript lines, but slower and occasionally will fail
   on smooth-then-dense plots (eg. sin(1/x^60) from -10 to 10) */
#define MAX_SMOOTHNESS_LVL 4
#define LIM .002


/** BOX PARAMETERS **/
/* bounding box LOWER-LEFT and UPPER-RIGHT corners */
#define LLX 18
#define LLY 18
#define URY 594
#define URX 774

#define BLANK 50


/** STACK for evaluating parsed expression **/
static double * stack;
static int sp = 0;

#define PUSH(x) stack[sp++] = (x)
#define POP()   stack[--sp]
#define PEEK()  stack[sp - 1]


/** PLOT x and y LIMITS **/
static double x_low = -10;
static double x_high = 10;
static double y_low = -10;
static double y_high = 10;

static double scale_x;
static double scale_y;


#define IS_NAN(x) (x != x)


double evaluate(parsed_expr p, double x)
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


int plot_init(int size, Limits * lims)
{
    if (lims) {
        x_low  = lims->x_low;
        x_high = lims->x_high;
        y_low   = lims->y_low;
        y_high  = lims->y_high;
    }

    stack = (double*) calloc(size, sizeof(double));
    sp = 0;

    scale_x = (URX - LLX - 2*BLANK) / (x_high - x_low);
    scale_y = (URY - LLY - 2*BLANK) / (y_high - y_low);
    return 1;
}



static void plot(FILE * out, parsed_expr p)
{
    double delta = (x_high - x_low) / SMOOTHNESS;
    double x_1 = x_low;
    double x_2 = x_low + delta;
    double y_1 = evaluate(p, x_1);
    double y_2 = evaluate(p, x_2);
    double old_x;
    double old_y;
    /* if y-value out of box, compute intersection of line with box */
    double x_intersect;
    int last_out = 0;
    int SMOOTHNESS_LVL = MAX_SMOOTHNESS_LVL;

    if (y_1 < y_low)
        y_1 = y_low;
    else if (y_1 > y_high)
        y_1 = y_high;



/** real coordinates to postscript plot coordinates transformation **/
#define COORD_X(x) (((x) - x_low) * scale_x + LLX + BLANK)
#define COORD_Y(y) (((y) - y_low) * scale_y + LLY + BLANK)

#define LINETO(x, y) fprintf(out, "%.3f %.3f lineto\n", \
                             COORD_X(x), COORD_Y(y)) 
#define MOVETO(x, y) fprintf(out, "%.3f %.3f moveto\n", \
                             COORD_X(x), COORD_Y(y)) 

/* find intersection with y-boundary */
#define INTERSECT(boundary) (x_1 + ((boundary) - y_1) * \
                            (x_2 - x_1) / (y_2 - y_1));

    fprintf(out, "newpath\n");
    MOVETO(x_1, y_1);


    old_x = x_1;
    old_y = y_1;

    /* plotting loop */
    /* x_intersect produces nans for functinos like sin(x^100) */
    while (x_2 <= x_high) {

        if (y_low <= y_2 && y_2 <= y_high) {
            if (last_out) {
                if (y_2 > y_1) {
                    x_intersect = INTERSECT(y_low);
                    if (!IS_NAN(x_intersect))
                        MOVETO(x_intersect, y_low);
                } else {
                    x_intersect = INTERSECT(y_high);
                    if (!IS_NAN(x_intersect))
                        MOVETO(x_intersect, y_high);
                }
            }
            LINETO(x_2, y_2);

            last_out = 0;
        } else {
            if (!last_out) {
                if (y_2 > y_1) {
                    x_intersect = INTERSECT(y_high);
                    if (!IS_NAN(x_intersect))
                        LINETO(x_intersect, y_high);
                } else {
                    x_intersect = INTERSECT(y_low);
                    if (!IS_NAN(x_intersect))
                        LINETO(x_intersect, y_low);
                }
            }

            last_out = 1;
        }

/** smoothing procedure **/
/* SLOPE_JUMP <- second derivative */
#define SLOPE_JUMP (y_2 - y_1 - ((y_1 - old_y)*(x_2 - x_1))/(x_1 - old_x))
#define TOO_SHARP() (fabs(SLOPE_JUMP) > LIM)
#define TOO_SMOOTH() (fabs(SLOPE_JUMP) < LIM / 4)

        old_x = x_1;
        old_y = y_1;
        x_1 = x_2;
        y_1 = y_2;
        x_2 = x_1 + delta;
        y_2 = evaluate(p, x_2);
        if (TOO_SHARP()) {
            while (SMOOTHNESS_LVL < MAX_SMOOTHNESS_LVL && TOO_SHARP()) {
                delta /= 2;
                x_2 = x_1 + delta;
                y_2 = evaluate(p, x_2);
                SMOOTHNESS_LVL ++;
            }
        } else {
            while (SMOOTHNESS_LVL > 0 && TOO_SMOOTH()) {
                delta *= 2;
                x_2 = x_1 + delta;
                y_2 = evaluate(p, x_2);
                SMOOTHNESS_LVL --;
            }
        }

        /* make sure the last point is on the right boundary */
        if (x_1 < x_high && x_1 + delta > x_high) {
            x_2 = x_high;
            y_2 = evaluate(p, x_2);
        }
    }

    /* stroke the path */
    fprintf(out, "%s setrgbcolor\n", PLOT_COLOR);
    fprintf(out, "0.5 setlinewidth\n");
    fprintf(out, "stroke\n\n\n");
}


static void write_header(FILE * out, char * expression)
{
    fprintf(out, "%%!PS-Adobe-3.0\n");
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


#define MAX_UNITS 8
#define LINE_LEN  8

static void write_axis_units(FILE * out, int horizontal) 
{
    double size;
    double axis_scale;
    double power;
    double unit_size;
    double unit_position;
    int print_precision = 0;

    /** x axis **/
    if (horizontal)
        size = x_high - x_low;
    else 
        size = y_high - y_low;

    power = ceil(log10(size / MAX_UNITS) - 1);
    if (size / (2 * pow(10, power)) <= MAX_UNITS)
        axis_scale = 2;
    else if (size / (5 * pow(10, power)) <= MAX_UNITS)
        axis_scale = 5;
    else {
        axis_scale = 1;
        power ++;
    }

    unit_size = axis_scale * pow(10, power);
    /* TODO doesn't work properly  when x_low negative */
    if (horizontal) {
        unit_position = x_low - fmod(x_low, unit_size);
        if (x_low >= 0)
            unit_position += unit_size;
    } else {
        unit_position = y_low - fmod(y_low, unit_size);
        if (y_low >= 0)
            unit_position += unit_size;
    }

    if (power < 0)
        print_precision = - (int) power;


    fprintf(out, "newpath\n");

    if (horizontal) {
        do {
            MOVETO(unit_position, y_low);
            fprintf(out, "%d %d rlineto\n", 0, -LINE_LEN);
            fprintf(out, "%d %d rmoveto\n", 0, - 2 * LINE_LEN);
            fprintf(out, "(%.*f) show\n", print_precision, unit_position);

            unit_position += unit_size;
        } while (unit_position <= x_high);
    } else {
        do {
            MOVETO(x_low, unit_position);
            fprintf(out, "%d %d rlineto\n", -LINE_LEN, 0);
            fprintf(out, "%d %d rmoveto\n", - 2 * LINE_LEN, 0);
            fprintf(out, "90 rotate\n");
            fprintf(out, "(%.*f) show\n", print_precision, unit_position);
            fprintf(out, "-90 rotate\n");

            unit_position += unit_size;
        } while (unit_position <= y_high);
    }


    fprintf(out, "0 setgray\n");
    fprintf(out, "0.4 setlinewidth\n");
    fprintf(out, "stroke\n\n");
}


/* Draws box around plot and units */
static void write_box(FILE * out)
{
    fprintf(out, "newpath\n");
    MOVETO(x_low, y_low);
    LINETO(x_high, y_low);
    LINETO(x_high, y_high);
    LINETO(x_low, y_high);
    fprintf(out, "closepath\n");
    fprintf(out, "0 setgray\n");
    fprintf(out, "1 setlinewidth\n");
    fprintf(out, "stroke\n\n");

    fprintf(out, "/Helvetica findfont\n");
    fprintf(out, "11 scalefont\n");
    fprintf(out, "setfont\n");

    write_axis_units(out, 0);
    write_axis_units(out, 1);
}


/* write ending postscript comments */
static void write_footer(FILE * out)
{
    fprintf(out, "%%%%PageTrailer\n");
    fprintf(out, "%%%%Trailer\n");
    fprintf(out, "%%%%EOF\n\n");
}


/* plot parsed expression and write it to postscript file */
void write_ps(FILE * out, parsed_expr parsed, char * expr, Limits * lim)
{
    plot_init(parsed.length, lim);

    write_header(out, expr);
    plot(out, parsed);
    write_box(out);
    write_footer(out);

    free(stack);
}
