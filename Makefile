CC = gcc
CFLAGS = -Wall -pedantic -ansi
EXEC = graph
SRCS = main.c lexer.c parser.c func.c plot.c
OBJS = $(SRCS:.c=.o)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -lm

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
