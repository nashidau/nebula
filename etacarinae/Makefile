
PKGS='elementary nebula'
CFLAGS+=`pkg-config --cflags ${PKGS}`
LDFLAGS+=`pkg-config --libs ${PKGS}`

etac: etac.c etac_chr.c

clean:
	rm -f *.o etac core


