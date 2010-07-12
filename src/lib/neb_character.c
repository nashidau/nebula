
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

/**
 * Locks and attribute to prevent modification.
 *
 * Generally this is used to prevent aliases from being modified, or to lock
 * down system related items.  So if you have an attribute 'st' to reference
 * 'Strength' you can lock down 'st' to make sure noone accidently modifies
 * 'st' instead of strength.
 *
 * Locks can be applied multiple times, which allows constructs to pass an
 * attribute locked to another function for instance.  This function returns
 * the current lock level.
 *
 * A locked attribute can be locked again.
 *
 * @param attr Attribute to lock.
 * @return -1 on error, or the current lock level (> 0).
 */
int
neb_attr_lock(struct neb_attr *attr){
	if (!attr) return -1;

	attr->lock ++;
	return attr->lock;
}

/**
 * Reduce lock level on an attribute.
 * 
 * Note that to fully unlock an attribute you need to unlock it as many times
 * as you lock it.
 *
 * A return of 0 indicates the attribute is unlocked.
 * It is an error to unlock an attribute that is currently unlocked.
 *
 * @param attr Attribute to reduce lock level on.
 * @return -1 on error, or current lock level (0 or more).
 */
int
neb_attr_unlock(struct neb_attr *attr){
	if (!attr) return -1;

	if (attr->lock == 0) return -1;
	return --attr->lock;
}

/**
 * Get the current lock level.
 *
 * Any value greater then 0 indicates the attribute is locked.
 *
 * @param attr Attribute to check
 * @return -1 on error, 0 if unlocked, lock level (>0) otherwise.
 */
int
neb_attr_lock_get(struct neb_attr *attr){
	if (!attr) return -1;

	return (int)(attr->lock);
}

/**
 * Get the name of an attribute.
 *
 * Gets the name of an attribute.  The returned value should not be freed or
 * modified by the caller.
 *
 * @param attr Attribute to get the name of.
 * @return Attribute name or NULL on error.
 */
const char *
neb_attr_name_get(struct neb_attr *attr){
	if (!attr) return NULL;
	return attr->name;
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



