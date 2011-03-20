
# FIXME: Shoudl be nebula
PKGS=eina

CFLAGS+=`pkg-config --cflags ${PKGS}` 
LDFLAGS+=`pkg-config --libs ${PKGS}` -rdynamic -ldl -lnebula -L../lib

SOURCES=		\
	nebtest.c


nebtest: ${SOURCES}

.PHONY: clean
clean:
	rm -f nebtest core *.o

