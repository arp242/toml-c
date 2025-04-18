CC               = cc
PREFIX           = /usr/local
FPIC             = -fPIC
CFLAGS           = -std=c99 -Wall -Wextra -Wimplicit-fallthrough ${FPIC} -O2 -g
SANITIZER_CFLAGS = -fsanitize=address -fsanitize=undefined

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
	@echo 'create $@'
	@: >header/toml-c.h
	@sed  '/#endif \/\/ TOML_H/d; /#define TOML_H/a#ifndef _POSIX_C_SOURCE\n#define _POSIX_C_SOURCE 200809L\n#endif' toml.h >>header/toml-c.h
	@sed  '/#include "toml.h"/d; /_POSIX_C_SOURCE/d' toml.c >>header/toml-c.h
	@echo '#endif // TOML_H' >>header/toml-c.h

toml.o: toml.c ${HDRS}
	${CC} ${CFLAGS} -c $<

libtoml.a: ${OBJS}
	ar -rcs $@ $^

libtoml.so.1.0: ${OBJS}
	${CC} ${CFLAGS} -shared -o $@ $^

${PROG}: ${LIB}
	${CC} ${CFLAGS} -o ${PROG} ${SANITIZER_CFLAGS} toml.c toml2json.c

toml-c-test: ${LIB}
	${CC} ${CFLAGS} -o toml-c-test ${SANITIZER_CFLAGS} toml.c toml-c-test.c

install: all
	install -d ${DESTDIR}${PREFIX}/include ${DESTDIR}${PREFIX}/lib ${DESTDIR}${PREFIX}/lib/pkgconfig
	install toml.h   ${DESTDIR}${PREFIX}/include
	install ${LIB}   ${DESTDIR}${PREFIX}/lib
	install ${SOLIB} ${DESTDIR}${PREFIX}/lib
	sed 's!%%PREFIX%%!${PREFIX}!' ${PCFILE} >${DESTDIR}${PREFIX}/lib/pkgconfig/${PCFILE}

check: ${PROG} toml-c-test
	@./test.bash
	@echo
	@./toml-c-test


clean:
	rm -f *.o ${PROG} toml-c-test ${LIB} ${SOLIB}
