/**
 * @file neb_character.h
 *
 * Character management for nebula.
 */
#include <Eina.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "nebula.h"
#include "nebula_private.h"

enum {
	CHARACTER_MAGIC = 0x543423fe
};

static Eina_Bool attr_save(const void *list, void *attrv, void *fpv);
static Eina_Bool note_save(const void *list, void *notev, void *fpv);

/**
 * Create a new character and add it to nebula.
 *
 * This character will initially have no attributes and the name will be set
 * to NULL.
 *
 * @param neb Nebula instance to add character to to.
 * @return New character.
 */
struct neb_character *
neb_character_new(struct nebula *neb){
	struct neb_character *nc;
	int rv;

	if (!neb) return NULL;

	nc = calloc(1,sizeof(struct neb_character));
	if (!nc) return NULL;

	nc->magic = CHARACTER_MAGIC;

	rv = nebula_character_add(neb,nc);
	if (rv < 0) {
		free(nc);
		return NULL;
	}
	return nc;
}

/**
 * Set the name of a character.
 *
 * Any existing name will be removed.  All characters should have a name, as
 * it is the only way of accessing a character unless you have a reference to
 * it directly.  Also names should be unique.
 *
 * You may wish to use a note to keep a real name, with a sequence number of
 * similar as the name in this field.
 *
 * Setting a NULL name is not allowed.
 *
 * @param nc Nebula character.
 * @param name Name to set on character.
 * @return 0 on success, or -1 on error.
 */
int
neb_character_name_set(struct neb_character *nc, const char *name){
	if (!nc || nc->magic != CHARACTER_MAGIC) return -1;
	if (!name) return -1;

	if (nc->name) free(nc->name);
	nc->name = strdup(name);
	return 0;
}

/**
 * Get a characters name.
 *
 * @param nc Character to get name of.
 * @return Name or NULL on error.
 */
const char *
neb_character_name_get(struct neb_character *nc){
	if (nc && nc->magic == CHARACTER_MAGIC) return nc->name;
	return NULL;
}

/**
 * Find a character by name.
 *
 * Note that this finds the first character with a given name.  If there are
 * multiple characters with the same name, there is no guarantee the correct
 * one will be loaded.
 *
 * Names are case sensitive, and must be non-NULL.
 *
 * @param neb Nebula instance.
 * @param name Name to search for.
 * @return character, or NULL on error or not found.
 */
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

/**
 *
 */
struct nebula *
neb_character_neb_get(struct neb_character *nc){
	if (!nc) return NULL;

	return nc->neb;
}

/**
 * Adds a new attribute to the given character.
 *
 * The name given for the attribute must be unique, if not this call will
 * fail.
 *
 * Attribute names are persistent and can not be changed once they are created
 * (otherwise references would be lost).
 *
 * @param nc Nebula character.
 * @param name Name of attribute.
 * @return New attribute, or NULL on error.
 */
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

/**
 * Get a particular attribute.
 *
 * The attribute handle is normally used to add elements, filters or tags to.
 * Alternatively the value can be got by calling neb_attr_value_get().
 * 
 * @param nc Character to get attribute off
 * @param name Name of attribute.
 * @return Attribute or NULL on error.
 */
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

/**
 * Gets the value of an attribute.
 * 
 * Note that on error this function returns -1.  Currently the only error
 * possible is a NULL attribute, or broken reference.
 *
 * The API will probably change in future to allow an error return value.
 *
 * @param attr Attribute to get
 * @return Attribute value, or -1 on error.
 * @todo Handle errors in a sane manner
 */
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




/**
 * Add a property to a character.
 */
int
neb_attr_prop_add(struct neb_attr *attr, const char *prop,
		const char *value){
	struct neb_prop *p;

	if (!attr || !prop) return -1;

	p = malloc(sizeof(struct neb_prop));
	p->prop = strdup(prop);
	if (value) p->value = strdup(value);

	attr->props = eina_list_append(attr->props, p);
	return 0;
}

const char *
neb_attr_prop_get(struct neb_attr *attr, const char *prop){
	struct neb_prop *p;
	Eina_List *l;

	if (!attr || !prop) return NULL;

	for (l = attr->props ; l ; l = l->next){
		p = l->data;
		if (strcmp(prop,p->prop) == 0)
			return p->value ? : "";
	}
	return NULL;
}


Eina_Iterator *
neb_attr_elem_iter_new(struct neb_attr *attr){
	if (!attr) return NULL;
	return eina_list_iterator_new(attr->elems);
}



/**
 * Add a character to be managed by nebula.
 *
 * If the character is already managed by another nebula instance, it should
 * be removed first.
 *
 * Note the neb_character_new() automatically adds the character to a nebula
 * instance.
 *
 * @param neb Nebula instance.
 * @param ch Character to add.
 * @return -1 on error, 0 otherwise.
 */
int
nebula_character_add(struct nebula *neb, struct neb_character *ch){
	if (!neb) return -1;
	if (!ch) return -1;

	neb->characters = eina_list_append(neb->characters, ch);
	ch->neb = neb;
	return 0;
}




/**
 * Load a character.
 */
struct neb_character *
neb_character_load(struct nebula *neb, const char *name){
	struct neb_character *ch;
	int rv;
	if (!neb) return NULL;
	if (!name) return NULL;

	if (!neb->L)
		luaneb_init(neb);

	rv = luaL_dofile(neb->L, name);
printf("%d rv\n",rv);
luaneb_stackdump(neb->L);

	ch = luaneb_tocharacter(neb->L,1);
	lua_pop(neb->L,1);

	rv = nebula_character_add(neb,ch);

	return ch;
}



/**
 * Save a character.
 */
int
nebula_character_save(struct neb_character *ch, const char *file){
	FILE *fp;
	Eina_Iterator *iter;

	printf("Save character\n");

	if (!ch) return -1;

	fp = fopen(file,"w");

	fprintf(fp,"return nebload{\n");
	fprintf(fp,"  name=[[%s]],\n",ch->name);

	/* FIXME: Notes */
	fprintf(fp,  "notes = {\n");
	iter = eina_list_iterator_new(ch->notes);
	eina_iterator_foreach(iter, note_save, fp);
	eina_iterator_free(iter);
	fprintf(fp, "},\n");


	/* Attrs */
	fprintf(fp,"  attrs = {\n");
	iter = eina_list_iterator_new(ch->attrs);
	eina_iterator_foreach(iter, attr_save, fp);
	fprintf(fp,"  },\n");

	/* FIXME: escape lua magics */

	fprintf(fp,"}\n");
	fclose(fp);

	return 0;
}



static Eina_Bool
attr_save(const void *list, void *attrv, void *fpv){
	struct neb_attr *attr = attrv;
	struct neb_elem *elem;
	struct neb_prop *prop;
	Eina_List *l;

	fprintf(fpv,"    {\n      name = [[%s]],\n",attr->name);

	if (attr->props){
		fprintf(fpv,"      props = {\n");
		EINA_LIST_FOREACH(attr->props, l, prop){
			fprintf(fpv,"        [\"%s\"] = [[%s]],\n",
					prop->prop,prop->value ? : "");

		}
		fprintf(fpv,"      },\n");
	}

	fprintf(fpv,"      elems = {\n");
	EINA_LIST_FOREACH(attr->elems, l, elem){
		elem->type->save(elem, fpv);
	}
	fprintf(fpv,"      }\n");

	fprintf(fpv,"    },\n");
	return true;
}

static Eina_Bool
note_save(const void *list, void *notev, void *fpv){
	struct neb_note *note = notev;
	char *equals = strdup("");
	int i;

	fprintf(fpv,"    {\n      name = [[%s]],\n",note->name);

	if (strstr(note->note,"]]")){
		equals = strdup("=");
		for (i = 0 ; strstr(note->note,equals) ; i ++){
			equals = realloc(equals, i + 1);
			memset(equals,'=',i);
			equals[i] = 0;
		}
	}

	fprintf(fpv,"      note = [%s[%s]%s],\n",equals,note->note,equals);
	fprintf(fpv,"    },\n");
	free(equals);

	/* FIXME: tags */

	return true;
}



Eina_Iterator *
neb_character_attr_iter_new(struct neb_character *nch){
	if (!nch) return NULL;

	return eina_list_iterator_new(nch->attrs);
}
