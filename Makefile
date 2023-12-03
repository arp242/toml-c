CC     = cc
PREFIX = /usr/local
CFLAGS = -std=c99 -Wall -Wextra -Wimplicit-fallthrough -fPIC -O2 -g

HDRS   = toml.h
SRCS   = toml.c
OBJS   = ${SRCS:.c=.o}
PCFILE = libtoml.pc
PROG   = toml2json
LIB    = libtoml.a
SOLIB  = libtoml.so.1.0

.PHONY: all clean install check

all: ${LIB} ${SOLIB} ${PROG} header/toml-c.h

header/toml-c.h: ${HDRS} ${SRCS}
	:                                    >header/toml-c.h
	sed  's!#endif // TOML_H!!'  toml.h >>header/toml-c.h
	sed  's!#include "toml.h"!!' toml.c >>header/toml-c.h
	echo '#endif // TOML_H'             >>header/toml-c.h

*.o: ${HDRS}
	${CC} ${CFLAGS} -c $<

libtoml.a: ${OBJS}
	ar -rcs $@ $^

libtoml.so.1.0: ${OBJS}
	${CC} ${CFLAGS} -shared -o $@ $^

${PROG}: ${LIB}

install: all
	install -d ${DESTDIR}${PREFIX}/include ${DESTDIR}${PREFIX}/lib ${DESTDIR}${PREFIX}/lib/pkgconfig
	install toml.h   ${DESTDIR}${PREFIX}/include
	install ${LIB}   ${DESTDIR}${PREFIX}/lib
	install ${SOLIB} ${DESTDIR}${PREFIX}/lib
	sed 's!%%PREFIX%%!${PREFIX}!' ${PCFILE} >${DESTDIR}${PREFIX}lib/pkgconfig

check: ${PROG}
	@./test.bash

clean:
	rm -f *.o ${PROG} ${LIB} ${SOLIB}
