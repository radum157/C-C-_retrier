CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2
LIBFLAGS :=

.PHONY: build clean

build:
	$(CC) $(CFLAGS) retrier.c -c $(LIBFLAGS)

clean:
	rm -f retrier.o
