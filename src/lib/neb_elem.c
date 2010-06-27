#include <stdbool.h>


#include "nebula.h"
#include "nebula_private.h"

static int el_value_value_get(struct neb_elem *el);
static int el_ref_value_get(struct neb_elem *el);

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
};

static const struct elem_type types[] = {
	[NEB_ELEM_VALUE] = {
		.size = sizeof(struct neb_elem_value),
		.value_get = el_value_value_get,
	},
	[NEB_ELEM_REFERENCE] = {
		.size = sizeof(struct neb_elem_ref),
		.value_get = el_ref_value_get,
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


struct neb_elem *
neb_attr_elem_value_add(struct neb_attr *at, int value){
	struct neb_elem_value *el;

	el = el_add(at, NEB_ELEM_VALUE);
	if (!el) return NULL;

	el->value = value;
	return &el->elem;
}


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

	ref = (struct neb_elem_ref *)el;
	at = neb_character_attr_get(ref->ch, ref->ref);

	return neb_attr_value_get(at);
}
