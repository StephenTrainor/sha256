CC=clang
REQ=sha256.c sha256.h main.c
CFLAGS=-Werror -Wextra -Wno-sign-compare -Wshadow -Ofast -O3
CDEBUG=-g
LIBS=-lm
TXTFILE=texts/bible.txt

all: $(REQ)
	find . -type f -exec touch {} +
	$(CC) main.c -c -o main.o $(CFLAGS)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS)
	$(CC) main.o sha256.o -o sha256 $(CLFAGS)
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)

tests: tests.c sha256.c sha256.h
	find . -type f -exec touch {} +
	$(CC) tests.c -c -o tests.o $(CFLAGS)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS)
	$(CC) tests.o sha256.o -o tests $(CLFAGS)
	rm tests.o
	rm sha256.o
	./tests

tests_windows: tests.c sha256.c sha256.h
	$(CC) tests.c -c -o tests.o $(CFLAGS)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS)
	$(CC) tests.o sha256.o -o tests $(CLFAGS)
	del tests.o
	del sha256.o
	tests.exe

clang: $(REQ)
	find . -type f -exec touch {} +
	clang main.c -c -o main.o $(CFLAGS)
	clang sha256.c -c -o sha256.o $(CFLAGS)
	clang main.o sha256.o -o sha256 $(LIBS) $(CFLAGS)
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)

windows: $(REQ)
	$(CC) main.c -c -o main.o $(CFLAGS)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS)
	$(CC) main.o sha256.o -o sha256 $(CLFAGS)
	del main.o
	del sha256.o
	sha256.exe $(TXTFILE)

asm: $(REQ)
	$(CC) main.c -S -fverbose-asm -o main.s $(CFLAGS)
	$(CC) sha256.c -S -fverbose-asm -o sha256.s $(CFLAGS)

asm_macro: macro.c macro_test.c macro.h
	clang macro.c -S -fverbose-asm -o macro.s $(CFLAGS)

so: $(REQ)
	gcc -shared -Wl,-soname,sha256 -o sha256.so -fPIC sha256.c

so_mac: $(REQ)
	gcc -shared -Wl,-install_name,sha256.so -o sha256.so -fPIC sha256.c

debug: $(REQ)
	find . -type f -exec touch {} +
	$(CC) main.c -c -o main.o $(CFLAGS) $(CDEBUG)
	$(CC) sha256.c -c -o sha256.o $(CFLAGS) $(CDEBUG)
	$(CC) main.o sha256.o -o sha256 $(CFLAGS) $(CDEBUG)
	rm main.o
	rm sha256.o
	./sha256 $(TXTFILE)
