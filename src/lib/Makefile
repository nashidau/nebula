PKGS=eina-0 lua5.1

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl


LIBTARGET=libnebula.so.0

SOURCES=			\
	nebula.c		\
	neb_character.c		\
	neb_elem.c		\
	luaneb.c

${LIBTARGET}: ${SOURCES}
	${CC} -fPIC -Os -Wall -shared -Wl,-soname,${LIBTARGET} -o ${LIBTARGET} \
		${CFLAGS} ${LDFLAGS}	\
                ${SOURCES}


nebula.c: nebula.h

install: ${LIBTARGET}
	cp ${LIBTARGET} /usr/local/lib
	sudo ln -sf /usr/local/lib/libnebula.so.0 /usr/local/lib/libnebula.so
	sudo ln -sf /usr/local/lib/libnebula.so.0 /usr/local/lib/lua/5.1/nebula.so
	sudo ldconfig

.PHONY: clean tags
tags:
	ctags -R . & cscope -R -b

clean:
	rm -f libneb*
	rm -f core *.o
