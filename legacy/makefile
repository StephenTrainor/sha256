CC=gcc
CFLAGS=-Werror -O3
TXTFILE=texts/abc.txt

%: %.c
	$(CC) $< -o $@ $(CFLAGS)
	$@.exe ../$(TXTFILE)