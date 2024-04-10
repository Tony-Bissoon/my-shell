CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address -O2

mysh: mysh.o 
	$(CC) $(CFLAGS) mysh.o -o mysh

clean:
	rm -f 