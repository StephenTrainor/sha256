CC=gcc
CFLAGS=-Werror -Wextra -Wno-sign-compare -Wshadow
CDEBUG=-g
TXTFILE=abc.txt

all: sha256.c
	find . -type f -exec touch {} +
	$(CC) sha256.c -o sha256 $(CFLAGS)
	./sha256 $(TXTFILE)

clang: sha256.c
	find . -type f -exec touch {} +
	clang sha256.c -o sha256 $(CFLAGS) -lm
	./sha256 $(TXTFILE)

debug: sha256.c
	find . -type f -exec touch {} +
	$(CC) sha256.c -o sha256 $(CFLAGS) $(CDEBUG)
	./sha256 $(TXTFILE)

