
#include <Eina.h>
#include <stdio.h>

#include "nebula.h"
#include "nebula_private.h"

enum {
	CHARACTER_MAGIC = 0x543423fe
};

struct neb_character {
	int magic;

	char *name;

	Eina_List *attrs;
	Eina_List *notes;
};


struct neb_character *
neb_character_new(struct nebula *neb){
	struct neb_character *nc;
	int rv;

	if (!neb) return NULL;

	nc = calloc(1,sizeof(struct neb_character));
	if (!nc) return NULL;

	nc->magic = CHARACTER_MAGIC;

	rv = nebula_charater_add(neb,nc);
	if (rv < 0) {
		free(nc);
		return NULL;
	}
	return nc;
}

int
neb_character_name_set(struct neb_character *nc, const char *name){
	if (!nc || nc->magic != CHARACTER_MAGIC) return -1;

	if (nc->name) free(nc->name);
	nc->name = strdup(name);
	return 0;
}

const char *
neb_character_name_get(struct neb_character *nc){
	if (nc && nc->magic == CHARACTER_MAGIC) return nc->name;
	return NULL;
}


struct neb_character *
neb_character_get(struct nebula *neb, const char *name){
	Eina_List *l;
	struct neb_character *nc;

	if (!neb || !name) return NULL;

	EINA_LIST_FOREACH(neb->characters, l, nc){
		if (nc->name && strcmp(nc->name, name) == 0)
			return nc;
	}
	return NULL;
}


struct neb_attr *neb_character_attribuate_add(struct neb_character *,
		const char *name);
struct neb_note *neb_character_note_add(struct neb_character *,
		const char *key);


