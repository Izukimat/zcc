CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

zcc: $(OBJS)
		$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): zcc.h

test: zcc
		./test.sh

clean:
		rm -f zcc *.o *~ tmp*

.PHONY: test clean