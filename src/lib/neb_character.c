
#include <Eina.h>

#include <stdbool.h>
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

struct neb_note {
	int magic;

	char *key;

	const char *markup;
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


struct neb_attr *
neb_character_attr_add(struct neb_character *nc, const char *name){
	struct neb_attr *attr;

	if (!nc || !name) return NULL;

	/* FIXME: Check name is unique */
	attr = neb_character_attr_get(nc,name);
	if (attr) return NULL;

	attr = calloc(1,sizeof(struct neb_attr));
	if (!attr) return NULL;
	attr->name = strdup(name);
	if (!attr->name) {
		free(attr);
		return NULL;
	}
	attr->ch = nc;

	nc->attrs = eina_list_append(nc->attrs, attr);

	return attr;
}

struct neb_attr *
neb_character_attr_get(struct neb_character *nc, const char *name){
	struct neb_attr *attr;
	Eina_List *l;

	if (!nc || !name) return NULL;

	EINA_LIST_FOREACH(nc->attrs, l, attr){
		if (strcmp(name, attr->name) == 0){
			return attr;
		}
	}
	return NULL;
}

int
neb_attr_value_get(struct neb_attr *attr){
	int value = 0;
	struct neb_elem *el;
	Eina_List *l;

	if (!attr) return -1;

	EINA_LIST_FOREACH(attr->elems, l, el){
		value += el->type->value_get(el);
	}
	return value;
}

/* Internal call */
int
neb_attribute_element_append(struct neb_attr *attr, struct neb_elem *el){
	/* FIXME: check magic */

	attr->elems = eina_list_append(attr->elems, el);
	return 0;
}




struct neb_note *
neb_character_note_add(struct neb_character *nc, const char *key){
	struct neb_note *note;

	if (!nc || !key) return NULL;

	note = calloc(1,sizeof(struct neb_note));
	if (!note) return NULL;

	note->key = strdup(key);
	if (!note->key){
		free(note);
		return NULL;
	}

	nc->notes = eina_list_append(nc->notes, note);
	return note;
}



