#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "nebula.h"

static int lneb_character_add(lua_State *);
static int lneb_character_get(lua_State *);
static int lneb_attr_add(lua_State *);
static int lneb_attr_get(lua_State *);
static int lneb_note_add(lua_State *);
static int lneb_attr_value_get(lua_State *);
static int lneb_elem_value_add(lua_State *);
static int lneb_elem_ref_add(lua_State *);
static int lneb_lock(lua_State *);

struct lneb_char {
	struct nebula *neb;
	struct neb_character *ch;
};

struct lneb_attr {
	struct nebula *neb;
	struct neb_attr *attr;
};

struct lneb_ref {
	struct nebula *neb;
	struct neb_elem *elem;
};




static const struct luaL_Reg fns[] = {
	{ "character_add",	lneb_character_add },
	{ "character_get",	lneb_character_get },

	{ NULL,		NULL },
};


static const struct luaL_Reg charfns[] = {
	{ "attr_add",		lneb_attr_add },
	{ "attr_get",		lneb_attr_get },
	{ "note_add",		lneb_note_add },
	{ NULL,		NULL },
};

static const struct luaL_Reg attrfns[] = {
	{ "value_get",		lneb_attr_value_get },
	{ "value_add",	lneb_elem_value_add },
	{ "ref_add",	lneb_elem_ref_add },
	{ "lock",	lneb_lock },
	{ NULL,		NULL },
};


#define LNEB_CHARACTER	"neb_character"
#define LNEB_ATTRIBUTE	"neb_attribute"
#define LNEB_ELEMENT	"neb_element"
#define LNEB_NOTE	"neb_note"


static struct nebula *neb;

int
luaopen_nebula(lua_State *lua){
	luaL_openlib(lua, "nebula", fns, 0);

	neb = nebula_init();

	// Create types: Character, Attribute & element for now
	luaL_newmetatable(lua, LNEB_CHARACTER);
	lua_pushstring(lua, "__index");
	lua_pushvalue(lua, -2);  /* pushes the metatable */
	lua_settable(lua, -3);  /* metatable.__index = metatable */
	luaL_openlib(lua, NULL, charfns, 0);

	/* FIXME: Function this up */
	luaL_newmetatable(lua, LNEB_ATTRIBUTE);
	lua_pushstring(lua, "__index");
	lua_pushvalue(lua, -2);  /* pushes the metatable */
	lua_settable(lua, -3);  /* metatable.__index = metatable */
	luaL_openlib(lua, NULL, attrfns, 0);

	luaL_newmetatable(lua, LNEB_ELEMENT);
	luaL_newmetatable(lua, LNEB_NOTE);

	return 1;
}



static int
lneb_character_add(lua_State *lua){
	struct lneb_char *lnc;

	lnc = lua_newuserdata(lua, sizeof(struct lneb_char));
	if (!lnc) return luaL_error(lua, "Memory error");

	luaL_getmetatable(lua, LNEB_CHARACTER);
	lua_setmetatable(lua, -2);

	lnc->ch = neb_character_new(neb);
	lnc->neb = neb;

	return 1;
}

static int
lneb_character_get(lua_State *lua){

	return 0;
}


static int
lneb_attr_add(lua_State *lua){
	struct lneb_char *lnc;
	struct lneb_attr *lna;
	const char *name;

	lnc = luaL_checkudata(lua, 1, LNEB_CHARACTER);
	name = luaL_checkstring(lua,2);
	luaL_argcheck(lua, name && strlen(name) > 1,2,
			"Must pass attribute name");

	lna = lua_newuserdata(lua, sizeof(struct lneb_attr));
	lna->neb = lnc->neb;
	lna->attr = neb_character_attr_add(lnc->ch, name);

	luaL_getmetatable(lua, LNEB_ATTRIBUTE);
	lua_setmetatable(lua, -2);

	return 1;
}
static int
lneb_attr_get(lua_State *lua){
	lua_pushnil(lua);
	return 1;
}
static int
lneb_note_add(lua_State *lua){
	lua_pushnil(lua);
	return 1;
}
static int
lneb_attr_value_get(lua_State *lua){
	lua_pushnil(lua);
	return 1;
}

static int
lneb_lock(lua_State *lua){

	printf("Lock not implemented\n");
	lua_pushnil(lua);
	lua_pushstring(lua,"Not implemented");
	return 1;
}


static int
lneb_elem_value_add(lua_State *lua){
	return 1;
}

/* FIXME: Optional argument for check */
static int
lneb_elem_ref_add(lua_State *lua){
	struct lneb_ref *lnr;
	struct lneb_attr *lna;
	const char *refto;

	lna = luaL_checkudata(lua, 1, LNEB_ATTRIBUTE);
	/* FIXME: Support an object to ref too */
	refto = luaL_checkstring(lua,2);

	lnr = lua_newuserdata(lua, sizeof(struct lneb_ref));
	lnr->neb = lna->neb;
	lnr->elem = neb_attr_elem_reference_add(lna->attr, refto, true);
	if (!lnr->elem){
		/* FIXME clean up */
		return luaL_error(lua, "Failed to add reference");
	}
	return 1;
}


