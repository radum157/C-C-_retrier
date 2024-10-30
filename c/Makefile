CC := gcc
CFLAGS := -std=c11 -Wall -Wextra
OLEVEL := -O2
LIBFLAGS :=

.PHONY: build clean build_demo clean_demo

build:
	$(CC) $(CFLAGS) $(OLEVEL) retrier.c -c $(LIBFLAGS)

build_demo: build
	$(CC) $(CFLAGS) main.c retrier.o -o main

clean_demo: clean
	rm -f log.txt main

clean:
	rm -f retrier.o
