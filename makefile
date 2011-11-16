CC = gcc
CFLAGS = -Wall -pedantic -ansi
EXEC = scan
SRCS = main.c lexer.c parser.c
OBJS = $(SRCS:.c=.o)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(EXEC) $(OBJS)
