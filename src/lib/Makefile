PKGS=eina-0 lua5.1

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl

CHECKFLAGS=`pkg-config --cflags --libs check nebula`


LIBTARGET=libnebula.so.0

SOURCES=			\
	nebula.c		\
	neb_character.c		\
	neb_elem.c		\
	neb_note.c		\
	luaneb.c

HEADERS=			\
	nebula.h

CHECK=				\
	check/check.o		\
	check/check_attr.o	\
	check/check_character.o \
	check/check_note.o

${LIBTARGET}: ${SOURCES} helpers.o
	${CC} -fPIC -Os -Wall -shared -Wl,-soname,${LIBTARGET} -o ${LIBTARGET} \
		${CFLAGS} ${LDFLAGS}	\
                ${SOURCES} helpers.o


nebula.c: nebula.h

helpers.o: helpers.lua
	ld -r -b binary -o helpers.o helpers.lua

# Check statically links in library for now
check: install ${CHECK} 
	${CC} ${CFLAGS} -o check/check ${CHECK} ${CHECKFLAGS} ${LDFLAGS}
	./check/check

install: ${LIBTARGET}
	cp ${LIBTARGET} /usr/local/lib
	cp ${HEADERS} /usr/local/include/
	cp nebula.pc /usr/local/lib/pkgconfig/
	sudo ln -sf /usr/local/lib/libnebula.so.0 /usr/local/lib/libnebula.so
	sudo ln -sf /usr/local/lib/libnebula.so.0 /usr/local/lib/lua/5.1/nebula.so
	sudo ldconfig

.PHONY: clean tags doc scan
scan: clean
	scan-build make

doc:
	doxygen

tags:
	ctags -R . & cscope -R -b

clean:
	rm -f libneb*
	rm -f core *.o
