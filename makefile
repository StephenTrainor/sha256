CC=gcc
REQ=sha256.c sha256.h main.c
CFLAGS=-Werror -Wextra -Wno-sign-compare -Wshadow
CDEBUG=-g
TXTFILE=texts/abc.txt

all: $(REQ)
	find . -type f -exec touch {} +
	$(CC) main.c -c -o main.o $(CFLAGS)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS)
	$(CC) main.o sha256.o -o sha256
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)

clang: $(REQ)
	find . -type f -exec touch {} +
	clang main.c -c -o main.o $(CFLAGS) -lm
	clang sha256.c -c -o sha256.o $(CFLAGS) -lm
	clang main.o sha256.o -o sha256
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)

debug: $(REQ)
	find . -type f -exec touch {} +
	$(CC) main.c -c -o main.o $(CFLAGS) $(CDEBUG)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS) $(CDEBUG)
	$(CC) main.o sha256.o -o sha256
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)