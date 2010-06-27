#include <Eina.h> // FIXME: ugle

struct nebula;
struct neb_character;

struct nebula {
	Eina_List *characters;
};

struct elem_type {
	size_t size;
	int (*value_get)(struct neb_elem *el);
};

struct neb_elem {
	int magic;
	const struct elem_type *type;
	const char *name;
};

struct neb_attr {
	int magic;

	struct neb_character *ch;
	char *name;

	Eina_List *elems;
};



int nebula_charater_add(struct nebula*, struct neb_character *);
int neb_attribute_element_append(struct neb_attr *attr, struct neb_elem *el);
