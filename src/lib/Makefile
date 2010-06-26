PKGS=eina

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl


LIBTARGET=libnebula.so.0

SOURCES=			\
	nebula.c		\
	neb_character.c

${LIBTARGET}: ${SOURCES}
	${CC} -fPIC -Os -Wall -shared -Wl,-soname,${LIBTARGET} -o ${LIBTARGET} \
                `pkg-config --cflags --libs ecore evas` \
                ${SOURCES}


nebula.c: nebula.h
