CC = gcc
CFLAGS = -Wall -pedantic -ansi
EXEC = graph.exe
SRCS = main.c lexer.c parser.c func.c plot.c
OBJS = $(SRCS:.c=.o)

.PHONY: clean

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -lm

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(EXEC) $(OBJS)