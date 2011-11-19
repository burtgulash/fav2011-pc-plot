#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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



void stack_init(int size)
{
    stack = (double*) calloc(size, sizeof(double));
    sp = 0;
}

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
static double smoothness = 500;

static double coord_x(double x)
{
	double scale_x = (URX - LLX) / (x_right - x_left);
	return (x - x_left) * scale_x + LLX;
}

static double coord_y(double y)
{
	double scale_y = (URY - LLY) / (y_high - y_low);
	return (y - y_low) * scale_y + LLY;
}

static void plot(FILE * out, parsed_expr p)
{
	double delta = (x_right - x_left) / smoothness;
	double x_1 = x_left;
	double x_2 = x_left + delta;
	double y_1, y_2;
	int last_out = 0;

    stack_init(p.length);
	y_1 = eval(p, x_1);

	fprintf(out, "newpath\n");
	fprintf(out, "%.2f %.2f moveto\n", x_1, y_1);

	while (x_2 <= x_right) {
		y_2 = eval(p, x_2);

		if (y_low <= y_2 && y_2 <= y_high) {
			if (last_out)
				fprintf(out, "%.2f %.2f moveto\n", 
                         coord_x(x_2), coord_y(y_2));
			else
				fprintf(out, "%.2f %.2f lineto\n", 
                         coord_x(x_2), coord_y(y_2));

			last_out = 0;
		} else 
			last_out = 1;
			

		x_1 = x_2;
		x_2 = x_2 + delta;
		y_1 = y_2;
	}

	fprintf(out, "%s setrgbcolor\n", PLOT_COLOR);
	fprintf(out, "stroke\n");
	fprintf(out, "%%%%PageTrailer\n\n");
}


static void write_header(FILE * out, char * expression)
{
	fprintf(out, "%%!PS-Adobe-3.0 EPSF 3.0\n");
	fprintf(out, "%%%%DocumentMedia: Letter %d %d 0 () ()\n", 
                                                  LLX + URX, LLY + URY);
	fprintf(out, "%%%%Title: Plot %s\n", expression);
	fprintf(out, "%%%%Creator: Plot utility\n");
	fprintf(out, "%%%%Pages: 1\n");
	fprintf(out, "%%%%Orientation: Landscape\n");
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
	fprintf(out, "stroke\n");
}

static void write_footer(FILE * out)
{
	fprintf(out, "%%%%Trailer\n");
	fprintf(out, "%%%%EOF\n\n");
}


void write_ps(FILE * out, char * expression)
{
	parsed_expr parsed = parse(expression);
	if (parsed.expr == NULL)
		return;

	write_header(out, expression);
	write_box(out);
	plot(out, parsed);
	write_footer(out);

	dispose(parsed);
}
