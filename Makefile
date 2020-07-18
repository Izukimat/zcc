CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

zcc: $(OBJS)
		$(CC) -o zcc $(OBJS) $(LDFLAGS)

$(OBJS): zcc.h

test: zcc
		./test.sh

clean:
		rm -f zcc *.o *~ tmp*

.PHONY: test clean