#include <Eina.h> // FIXME: ugle
#include <lua.h>

struct nebula;
struct neb_character;

struct nebula {
	Eina_List *characters;

	lua_State *L;
};

struct neb_character {
	int magic;

	char *name;

	struct nebula *neb;

	Eina_List *attrs;
	Eina_List *notes;
};

/** @todo: Use string shares for at least the tags */
struct neb_note {
	char *name;
	char *note;

	Eina_List *tags;
};


struct elem_type {
	enum neb_elem_type type;
	size_t size;
	int (*value_get)(struct neb_elem *el);
	int (*save)(struct neb_elem *el, FILE *fp);
	int (*free)(struct neb_elem *el);
};

struct neb_elem {
	int magic;
	const struct elem_type *type;
	char *note;
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

struct neb_prop {
	const char *prop;
	const char *value;
};


#define LEMPTY(_L)                                                        \
        do {                                                            \
                if (lua_gettop(_L) < 0)                                  \
                        printf("%s: %d: Stack underflow: %d\n",         \
                                        __FUNCTION__,__LINE__,          \
                                        lua_gettop(_L));                 \
                if (lua_gettop(_L) > 0)                                  \
                        luaneb_stack_dump(_L, "%s:%d End of function",   \
                                __FUNCTION__, __LINE__);                \
        } while (0)


int nebula_character_add(struct nebula*, struct neb_character *);
int neb_attribute_element_append(struct neb_attr *attr, struct neb_elem *el);

int luaneb_init(struct nebula *);
int luaneb_stackdump(lua_State *);
struct neb_character *luaneb_tocharacter(lua_State *L, int index);

char *luaneb_quote_str(const char *str);

void luaneb_stack_dump(lua_State *L, const char *,...);
