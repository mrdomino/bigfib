CFLAGS=-O3 -DNDEBUG -flto
LDFLAGS=-lgmp
WFLAGS=-std=c2x -Wall -Wextra

all: fib

fib: fib.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) $< -o $@

clean:
	rm fib

.PHONY: all clean
