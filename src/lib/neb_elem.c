#include <stdbool.h>


#include "nebula.h"
#include "nebula_private.h"

struct neb_elem *el_value_add(void);
int el_value_value_get(struct neb_elem *el);

struct neb_elem *el_ref_add(void);
int el_ref_value_get(struct neb_elem *el);

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


struct elem *
neb_attr_elem_reference_add(struct neb_attr *at, const char *ref, bool check){
	struct neb_elem_ref *el;

	if (!at || !ref) return NULL;

	if (check){
		/* FIXME:Implement */
	}

	el = el_add(at, NEB_ELEM_REFERENCE);
	if (!el) return NULL;

	el->ref = strdup(ref);

	return &el->elem;
}


