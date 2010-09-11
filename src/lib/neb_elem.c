/**
 * Element management for Nebula Attributes
 */

#include <stdbool.h>
#include <stdio.h>


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
	char *filter;
};

static const struct elem_type types[] = {
	[NEB_ELEM_VALUE] = {
		.size = sizeof(struct neb_elem_value),
		.value_get = el_value_value_get,
		.save = el_value_save,
	},
	[NEB_ELEM_REFERENCE] = {
		.size = sizeof(struct neb_elem_ref),
		.value_get = el_ref_value_get,
		.save = el_ref_save,
	},
};

static void *
el_add(struct neb_attr *at, enum neb_elem_type type){
	struct neb_elem *el;
	if (!at) return NULL;

	el = calloc(1,types[type].size);
	el->type = types + type;
	el->magic = ELEM_MAGIC;

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
neb_attr_elem_value_add(struct neb_attr *at, int value){
	struct neb_elem_value *el;

	el = el_add(at, NEB_ELEM_VALUE);
	if (!el) return NULL;

	el->value = value;
	return &el->elem;
}

/**
 * Create a reference element.
 *
 * A reference element references the value of another attribute.  Often this
 * will include some sort of filter to convert the value, but this is not
 * always so.  For instance a reference can be used to add a bonus to a skill
 * based on your strength directly, or it may calculate our strength bonus
 * based on your raw strength skill (strength 17 -> +3 bonus for instance)
 * using a filter.
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
neb_attr_elem_reference_add(struct neb_attr *at, const char *ref, bool check){
	struct neb_elem_ref *el;
	struct neb_attr *at2;

	if (!at || !ref) return NULL;

	if (check){
		at2 = neb_character_attr_get(at->ch, ref);
		/* FIXME: Err msg */
		if (!at2) return NULL;
	}

	el = el_add(at, NEB_ELEM_REFERENCE);
	if (!el) return NULL;

	el->ref = strdup(ref);
	el->ch = at->ch;

	return &el->elem;
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

	if (ref->filter){
		/* FIXME: Look up system */
		neb = neb_character_neb_get(ref->ch);

		lua_getglobal(neb->L,ref->filter);
		if (lua_isfunction(neb->L,-1)){
			/* FIXME: Make this a little more robust */
			lua_pushnumber(neb->L, val);
			lua_pcall(neb->L,1,1,0);
			val = lua_tonumber(neb->L,-1);
			lua_pop(neb->L,1);
		} else {
			printf("Didn't get filter %s\n",ref->filter);
		}
	}

	return val;
}


static int
el_value_save(struct neb_elem *el, FILE *fp){
	struct neb_elem_value *val;
	if (!el || !fp) return -1;

	val = (struct neb_elem_value *)el;

	fprintf(fp,"        { type='value', value = %d },\n", val->value);
	/* Common save here */

	return 0;
}

static int
el_ref_save(struct neb_elem *el, FILE *fp){
	struct neb_elem_ref *ref;

	ref = (struct neb_elem_ref *)el;

	/* FIXME: Esacpe */
	fprintf(fp,"        { type = 'ref', ref = [[%s]]",ref->ref);
	if (ref->filter)
		fprintf(fp,", filter = [[%s]]",ref->filter);
	fprintf(fp," },\n");
	/* Common save here */

	return 0;
}


/**
 * Add a filter to a reference.
 *
 * If a filter is already set, it will be deleted.
 * A NULL filter, clears the existing filter.
 *
 * @todo Note on what filters do
 *
 * @param el Reference element to free.
 * @param filter Filter to set.
 * @param 0 on success, -1 on error.
 */
int
neb_elem_ref_filter_set(struct neb_elem *el, const char *filter){
	struct neb_elem_ref *ref;

	/* FIXME: Magic check */
	ref = (struct neb_elem_ref *)el;

	if (ref->filter){
		free(ref->filter);
		ref->filter = NULL;
	}

	if (!filter) return 0;

	ref->filter = strdup(filter);
	return ref->filter ? 0 : -1;
}
