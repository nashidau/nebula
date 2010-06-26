
#include <Eina.h>
#include <stdio.h>

#include "nebula.h"

struct nebula {
	Eina_List *characters;
};


struct nebula *
nebula_init(void){
	struct nebula *neb;

	eina_init();

	neb = malloc(sizeof(struct nebula));
	neb->characters = NULL;

	return neb;
}

int
nebula_free(struct nebula *neb){
	free(neb);
	return 0;
}


int
nebula_charater_add(struct nebula *neb, struct neb_character *ch){
	if (!neb) return -1;
	if (!ch) return -1;

	neb->characters = eina_list_append(neb->characters, ch);
	return 0;
}


