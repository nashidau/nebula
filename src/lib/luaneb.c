#include <stdbool.h>

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
	{ "elem_value_add",	lneb_elem_value_add },
	{ "elem_ref_add",	lneb_elem_ref_add },
	{ NULL,		NULL },
};


int
luaopen_nebula(lua_State *lua){

	return 0;
}
