CC=gcc
LIBS=-lpthread

snake: snake.c
	$(CC) -o snake snake.c $(LIBS)

