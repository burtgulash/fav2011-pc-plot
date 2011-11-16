CC = gcc
CFLAGS = -Wall -pedantic -ansi
EXEC = parse
SRCS = main.c lexer.c parser.c func.c
OBJS = $(SRCS:.c=.o)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) -lm

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(EXEC) $(OBJS)
