CC        = cc
PREFIX    = /usr/local
FPIC      = -fPIC
CFLAGS    = -std=c99 -Wall -Wextra -Wimplicit-fallthrough ${FPIC} -O2 -g
SANITIZER = -fsanitize=address -fsanitize=undefined
#COVERAGE  = -g -O0 --coverage -lgcov -fprofile-arcs -ftest-coverage

HDRS   = toml.h
SRCS   = toml.c
OBJS   = ${SRCS:.c=.o}
PCFILE = libtoml.pc
LIB    = libtoml.a
SOLIB  = libtoml.so.1.0

.PHONY: all clean install check

all: ${LIB} ${SOLIB} toml2json toml-c-test header/toml-c.h

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

toml2json: toml2json.c ${HDRS} ${SRCS}
	${CC} ${CFLAGS} -o toml2json ${SANITIZER} ${COVERAGE} toml.c toml2json.c

toml-c-test: toml-c-test.c ${HDRS} ${SRCS}
	${CC} ${CFLAGS} -o toml-c-test ${SANITIZER} ${COVERAGE} toml.c toml-c-test.c

install: all
	install -d ${DESTDIR}${PREFIX}/include ${DESTDIR}${PREFIX}/lib ${DESTDIR}${PREFIX}/lib/pkgconfig
	install toml.h   ${DESTDIR}${PREFIX}/include
	install ${LIB}   ${DESTDIR}${PREFIX}/lib
	install ${SOLIB} ${DESTDIR}${PREFIX}/lib
	sed 's!%%PREFIX%%!${PREFIX}!' ${PCFILE} >${DESTDIR}${PREFIX}/lib/pkgconfig/${PCFILE}

check: toml2json toml-c-test
	@./test.bash
	@echo
	@./toml-c-test

report: check
	@[ -f toml2json-toml.gcda ] && mv toml2json-toml.gcda toml.gcda ||:
	@[ -f toml2json-toml.gcno ] && mv toml2json-toml.gcno toml.gcno ||:
	gcov -kt toml.c

clean:
	rm -f *.o *.gcov *.gcda *.gcno toml2json toml-c-test ${LIB} ${SOLIB}
