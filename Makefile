CFLAGS=-std=c11 -g -static

zcc: zcc.c11

test: zcc
		./test.sh

clean:
		rm -f zcc *.o *~ tmp*

.PHONY: test clean