/**
 * Element management for Nebula Attributes
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "nebula.h"
#include "nebula_private.h"

static int el_value_value_get(struct neb_elem *el);
static int el_ref_value_get(struct neb_elem *el);
static int el_value_save(struct neb_elem *el, FILE *fp);
static int el_ref_save(struct neb_elem *el, FILE *fp);

enum {
	/* FIXME: Unify nebula magics */
	ELEM_MAGIC = 0x5feb4543
};

struct neb_elem_value {
	struct neb_elem elem;
	int value;
};

struct neb_elem_ref {
	struct neb_elem elem;

	struct neb_character *ch;
	const char *ref;
	char *transform;
};

static const struct elem_type types[] = {
	[NEB_ELEM_VALUE] = {
		.type = NEB_ELEM_VALUE,
		.size = sizeof(struct neb_elem_value),
		.value_get = el_value_value_get,
		.save = el_value_save,
	},
	[NEB_ELEM_REFERENCE] = {
		.type = NEB_ELEM_REFERENCE,
		.size = sizeof(struct neb_elem_ref),
		.value_get = el_ref_value_get,
		.save = el_ref_save,
	},
};

static void *
el_add(struct neb_attr *at, const char *note,  enum neb_elem_type type){
	struct neb_elem *el;
	if (!at) return NULL;

	el = calloc(1,types[type].size);
	el->type = types + type;
	el->magic = ELEM_MAGIC;
	el->note = note ? strdup(note) : NULL;

	neb_attribute_element_append(at, el);

	return el;
}

/**
 * Create a new value elem.
 *
 * Value elements simply contain a numerical value.  Depending on system these
 * may or may not change over the life of the character.  Non-changing
 * elems are used in systems that include history.
 *
 * @param at Attribute to append element to
 * @param value Initial value of element.
 * @return New element.
 */
struct neb_elem *
neb_attr_elem_value_add(struct neb_attr *at, const char *note, int value){
	struct neb_elem_value *el;

	el = el_add(at, note, NEB_ELEM_VALUE);
	if (!el) return NULL;

	el->value = value;
	return &el->elem;
}

/**
 * Create a reference element.
 *
 * A reference element references the value of another attribute.  Often this
 * will include some sort of transform to convert the value, but this is not
 * always so.  For instance a reference can be used to add a bonus to a skill
 * based on your strength directly, or it may calculate our strength bonus
 * based on your raw strength skill (strength 17 -> +3 bonus for instance)
 * using a transform.
 *
 * Generally the check flag should be set to true.  The only reason not to is
 * if a large group of attributes is being added at once, and ordering may
 * mean they are out of order.  Even then it is recommended to set check true
 * and retry any that fail.  If check is true on all reference adds, it is
 * impossible to get a either dangling reference attributes or circular
 * references.
 *
 * @param at Attribute to add reference too.
 * @param ref Name of attribute to reference.
 * @param check Check reference is valid (Recommended)
 */
struct neb_elem *
neb_attr_elem_reference_add(struct neb_attr *at, const char *note,
		const char *ref, bool check){
	struct neb_elem_ref *el;
	struct neb_attr *at2;

	if (!at || !ref) return NULL;

	if (check){
		at2 = neb_character_attr_get(at->ch, ref);
		/* FIXME: Err msg */
		if (!at2) return NULL;
	}

	el = el_add(at, note, NEB_ELEM_REFERENCE);
	if (!el) return NULL;

	el->ref = strdup(ref);
	el->ch = at->ch;

	return &el->elem;
}

/**
 * Delete an element
 *
 * @todo Document this
 * FIXME: Document this
 */
int
neb_attr_elem_del(struct neb_attr *at, struct neb_elem *el){
	if (!at) return -1;
	if (!el) return 0;

	at->elems = eina_list_remove(at->elems, el);

	return 0;
}

/**
 * Get the type of an element.
 * Useful to be able to inspect it.
 *
 * @param el Element to get type of.
 * @return -1 on error, else either a REF or VALUE element.
 */
enum neb_elem_type
neb_elem_type_get(struct neb_elem *el){
	if (!el) return -1;
	return el->type->type;
}

/**
 * If the element is a reference, returns the name of the refernced attribute.
 *
 * The returned data should not be freed.
 * @param el Element.
 * @return NULL on error, or referenced element.
 */
const char *
neb_elem_reference_get(struct neb_elem *el){
	struct neb_elem_ref *elr;
	if (!el || el->type->type != NEB_ELEM_REFERENCE) return NULL;

	elr = (void *)el;
	return elr->ref;
}

/**
 * If the element is a value element, return it's value.
 *
 * @param el Element.
 * @return INT_MAX on error, or the value other.
 */
int
neb_elem_value_get(struct neb_elem *el){
	struct neb_elem_value *elv;
	if (!el || el->type->type != NEB_ELEM_VALUE) return INT_MAX;

	elv = (void *)el;
	return elv->value;
}

/**
 * Add a transform to a reference.
 *
 * If a transform is already set, it will be deleted.
 * A NULL transform, clears the existing transform.
 *
 * @todo Note on what transforms do.
 * @todo Fix check on transform get.
 *
 * @param el Reference element to free.
 * @param transform Filter to set.
 * @return 0 on success, -1 on error.
 */
int
neb_elem_ref_transform_set(struct neb_elem *el, const char *transform){
	struct neb_elem_ref *ref;

	if (!el || el->type->type != NEB_ELEM_REFERENCE) return -1;

	/* FIXME: Magic check */
	ref = (struct neb_elem_ref *)el;

	if (ref->transform){
		free(ref->transform);
		ref->transform = NULL;
	}

	if (!transform) return 0;

	ref->transform = strdup(transform);
	return ref->transform ? 0 : -1;
}

/**
 * Get the transform for a reference if set.
 *
 * @param el Element.
 * @return Transform, or NULL if no transform or error.
 */
const char *
neb_elem_ref_transform_get(struct neb_elem *el){
	struct neb_elem_ref *elr;

	if (!el || el->type->type != NEB_ELEM_REFERENCE) return NULL;

	elr = (void *)el;

	return elr->transform;
}

/**
 * Get the note field (if any) from an element.
 *
 * If there is no note, returns NULL.
 *
 * The string returned should not be freed.
 *
 * @param el Element
 * @return The note, or NULL if no note or error.
 */
const char *
neb_elem_note_get(const struct neb_elem *el){
	if (!el) return NULL;
	return el->note;
}


static int
el_value_value_get(struct neb_elem *el){
	struct neb_elem_value *eval;

	eval = (struct neb_elem_value *)el;
	return eval->value;
}

static int
el_ref_value_get(struct neb_elem *el){
	struct neb_elem_ref *ref;
	struct neb_attr *at;
	struct nebula *neb;
	int val;

	ref = (struct neb_elem_ref *)el;
	/* FIXME: Error handling */
	at = neb_character_attr_get(ref->ch, ref->ref);

	val = neb_attr_value_get(at);

	if (ref->transform){
		/* FIXME: Look up system */
		neb = neb_character_neb_get(ref->ch);

		lua_getglobal(neb->L,ref->transform);
		if (lua_isfunction(neb->L,-1)){
			/* FIXME: Make this a little more robust */
			lua_pushnumber(neb->L, val);
			lua_pcall(neb->L,1,1,0);
			val = lua_tonumber(neb->L,-1);
			lua_pop(neb->L,1);
		} else {
			printf("Didn't get transform %s\n",ref->transform);
		}
	}

	return val;
}


static int
el_value_save(struct neb_elem *el, FILE *fp){
	struct neb_elem_value *val;
	char *str;
	if (!el || !fp) return -1;

	val = (struct neb_elem_value *)el;

	if (el->note){
		str = luaneb_quote_str(el->note);
		fprintf(fp,"        { type='value', note=%s, value=%d },\n",
				str, val->value);
		free(str);
	} else {
		fprintf(fp,"        { type='value', value = %d },\n",
				val->value);
	}

	return 0;
}

static int
el_ref_save(struct neb_elem *el, FILE *fp){
	struct neb_elem_ref *ref;
	char *nstr = NULL, *rstr;

	ref = (struct neb_elem_ref *)el;

	rstr = luaneb_quote_str(ref->ref);

	if (el->note){
		nstr = luaneb_quote_str(el->note);
		fprintf(fp,"        { type='ref', note=%s, ref=%s", rstr, nstr);
	} else {
		fprintf(fp,"        { type = 'ref', ref=%s",rstr);
	}


	if (ref->transform)
		fprintf(fp,", transform = [[%s]]",ref->transform);
	fprintf(fp," },\n");

	if (nstr) free(nstr);
	free(rstr);

	return 0;
}



