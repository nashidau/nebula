
# FIXME: Shoudl be nebula
PKGS=eina-0

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl -lnebula -L../lib

SOURCES=		\
	nebtest.c


nebtest: ${SOURCES}

