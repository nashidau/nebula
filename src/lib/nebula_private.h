#include <Eina.h> // FIXME: ugle
#include <lua.h>

struct nebula;
struct neb_character;

struct nebula {
	Eina_List *characters;

	lua_State *L;
};

struct elem_type {
	size_t size;
	int (*value_get)(struct neb_elem *el);
	int (*save)(struct neb_elem *el, FILE *fp);
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

	unsigned int lock;

	Eina_List *elems;
	/* FIXME: A hash */
	Eina_List *props;
};

struct neb_property {
	const char *prop;
	const char *value;
};


int nebula_charater_add(struct nebula*, struct neb_character *);
int neb_attribute_element_append(struct neb_attr *attr, struct neb_elem *el);

int luaneb_init(struct nebula *);
int luaneb_stackdump(lua_State *);
struct neb_character *luaneb_tocharacter(lua_State *L, int index);
