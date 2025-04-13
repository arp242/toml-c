CC     = cc
CFLAGS = -std=c99 -Wall -Wextra -Wimplicit-fallthrough -fPIC -O2 -g

.PHONY: all clean

all: array table

array: array.c
	${CC} ${CFLAGS} -o $@ $@.c

table: table.c
	${CC} ${CFLAGS} -o $@ $@.c

clean:
	rm -f array table
