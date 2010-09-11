#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "nebula.h"
#include "nebula_private.h"

static int lneb_character_add(lua_State *);
static int lneb_character_get(lua_State *);
static int lneb_character_save(lua_State *);
static int lneb_character_name_set(lua_State *);
static int lneb_character_name_get(lua_State *);
static int lneb_attr_add(lua_State *);
static int lneb_attr_get(lua_State *);
static int lneb_note_add(lua_State *);
static int lneb_attr_value_get(lua_State *);
static int lneb_elem_value_add(lua_State *);
static int lneb_elem_ref_add(lua_State *);
static int lneb_lock(lua_State *);
static int lneb_unlock(lua_State *);
static int lneb_attr_tostring(lua_State *);

int luaneb_stackdump(lua_State* l);


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
	{ "name_set",		lneb_character_name_set },
	{ "name_get",		lneb_character_name_get },
	{ "save",		lneb_character_save },
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
	{ "unlock",	lneb_unlock },
	{ "__call",	lneb_attr_value_get },
	{ "__tostring",	lneb_attr_tostring },
	{ NULL,		NULL },
};


#define LNEB_CHARACTER	"neb_character"
#define LNEB_ATTRIBUTE	"neb_attribute"
#define LNEB_ELEMENT	"neb_element"
#define LNEB_NOTE	"neb_note"


extern int _binary_helpers_lua_size;
extern char _binary_helpers_lua_start[];
extern char _binary_helpers_lua_end[];

int
luaneb_init(struct nebula *neb){
	lua_State *L;
	int rv;

	if (!neb) return -1;

	L = neb->L;

	lua_pushstring(L, "Nebula");  /* push address */
	lua_pushlightuserdata(L, neb);  /* push value */
	lua_settable(L, LUA_REGISTRYINDEX);

	luaL_openlibs(L);

	rv = luaL_dostring(L,"require[[nebula]]\n");
	if (rv){
		printf("Error with dostring: %s\n",lua_tostring(L,-1));
		return -1;
	}


	return 0;
}

static struct nebula *
luaneb_get(lua_State *L){
	struct nebula *neb;
	lua_pushstring(L, "Nebula");  /* push address */
	lua_gettable(L, LUA_REGISTRYINDEX);

	neb = lua_touserdata(L,-1);
	lua_pop(L,1);
	return neb;
}

int
luaopen_nebula(lua_State *lua){
	struct nebula *neb;
	int len;
	int rv;

	lua_pushstring(lua, "Nebula");
	lua_gettable(lua, LUA_REGISTRYINDEX);
	if (lua_isnil(lua,-1)){
		neb = nebula_init();
		lua_pushstring(lua, "Nebula");  /* push address */
		lua_pushlightuserdata(lua, neb);  /* push value */
		lua_settable(lua, LUA_REGISTRYINDEX);
	} else
		neb = lua_touserdata(lua,-1);
	lua_pop(lua,1);

	neb->L = lua;
//	luaneb_init(neb);

	luaL_openlib(lua, "nebula", fns, 0);

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

	len = _binary_helpers_lua_end - _binary_helpers_lua_start;
	luaL_loadbuffer(lua, _binary_helpers_lua_start, len, "helpers.lua");
	rv = lua_pcall(lua,0,0,0);
	if (rv){
		printf("Error with helpers: %s\n",lua_tostring(lua,-1));
		return -1;
	}

	return 1;
}



static int
lneb_character_add(lua_State *lua){
	struct lneb_char *lnc;
	const char *name = NULL;

	lnc = lua_newuserdata(lua, sizeof(struct lneb_char));
	if (!lnc) return luaL_error(lua, "Memory error");

	if (lua_isstring(lua,1))
		name = lua_tostring(lua,1);

	luaL_getmetatable(lua, LNEB_CHARACTER);
	lua_setmetatable(lua, -2);

	/* FIXME: Inefficient getting twice */
	lnc->ch = neb_character_new(luaneb_get(lua));
	lnc->neb = luaneb_get(lua);

	if (name) neb_character_name_set(lnc->ch, name);

	return 1;
}

static int
lneb_character_get(lua_State *L){
	return luaL_error(L,"Not implemented");
	return 0;
}

static int
lneb_character_save(lua_State *L){
	struct lneb_char *lnc;
	const char *filename;
	char buf[100];

	lnc = luaL_checkudata(L,1,LNEB_CHARACTER);
	if (lua_isstring(L,2)){
		filename = lua_tostring(L,2);
	} else if (neb_character_name_get(lnc->ch)){
		snprintf(buf,100,"/tmp/%s.nch",neb_character_name_get(lnc->ch));
		filename = buf;
	} else {
		snprintf(buf,100,"/tmp/test.nch");
		filename = buf;
	}

	nebula_character_save(lnc->ch, filename);

	lua_pushboolean(L,1);
	return 1;
}


static int
lneb_character_name_set(lua_State *L){
	struct lneb_char *lnc;
	const char *name;

	lnc = luaL_checkudata(L, 1, LNEB_CHARACTER);
	name = luaL_checkstring(L,2);
	luaL_argcheck(L, lnc && lnc->ch, 1, "Character must be initied");

	neb_character_name_set(lnc->ch,name);

	lua_pushboolean(L,1);
	return 1;
}

static int
lneb_character_name_get(lua_State *L){
	struct lneb_char *lnc;

	lnc = luaL_checkudata(L, 1, LNEB_CHARACTER);
	luaL_argcheck(L, lnc && lnc->ch, 1, "Character must be initied");

	lua_pushstring(L,neb_character_name_get(lnc->ch));
	return 1;
}


static int
lneb_attr_add(lua_State *L){
	struct lneb_char *lnc;
	struct lneb_attr *lna;
	const char *name = NULL;
	int extra = 0;

	lnc = luaL_checkudata(L, 1, LNEB_CHARACTER);
	if (lua_istable(L,2)){
		lua_getfield(L,2,"name");
		name = lua_tostring(L,-1);
		extra = 1;
	} else if (lua_isstring(L,2)){
		name = luaL_checkstring(L,2);
	}
	luaL_argcheck(L, name && strlen(name) > 1,2,
			"Must pass attribute name");


	lna = lua_newuserdata(L, sizeof(struct lneb_attr));
	lna->neb = lnc->neb;
	lna->attr = neb_character_attr_add(lnc->ch, name);

	if (!lna->attr) return luaL_error(L,"Unable to create attribute");

	luaL_getmetatable(L, LNEB_ATTRIBUTE);
	lua_setmetatable(L, -2);

	return 1;
}
static int
lneb_attr_get(lua_State *lua){
	struct lneb_char *lnc;
	struct lneb_attr *lna;
	struct neb_attr *at;
	const char *name;

	lnc = luaL_checkudata(lua, 1, LNEB_CHARACTER);
	name = luaL_checkstring(lua,2);
	luaL_argcheck(lua, name && strlen(name) > 1, 2,
			"Must pass attribute name get");

	at = neb_character_attr_get(lnc->ch, name);
	if (!at)
		return luaL_error(lua, "Can't find attribute");

	/* FIXME: generalise wuth attr_add */
	lna = lua_newuserdata(lua, sizeof(struct lneb_attr));
	lna->neb = lnc->neb;
	lna->attr = at;

	luaL_getmetatable(lua, LNEB_ATTRIBUTE);
	lua_setmetatable(lua, -2);

	return 1;
}
static int
lneb_note_add(lua_State *lua){
	return luaL_error(lua, "note add: Not implemented");
	lua_pushnil(lua);
	return 1;
}
static int
lneb_attr_value_get(lua_State *lua){
	struct lneb_attr *lna;
	int val;

	lna = luaL_checkudata(lua, 1, LNEB_ATTRIBUTE);

	val = neb_attr_value_get(lna->attr);
	lua_pushnumber(lua, val);
	return 1;
}

/**
 * Locks an attribute, returns lock level
 */
static int
lneb_lock(lua_State *lua){
	struct lneb_attr *lna;
	int lvl;

	lna = luaL_checkudata(lua, 1, LNEB_ATTRIBUTE);

	lvl = neb_attr_lock(lna->attr);
	if (lvl < 1)
		return luaL_error(lua, "Unable to lock attribute");

	lua_pushnumber(lua, lvl);
	return 1;
}

static int
lneb_unlock(lua_State *lua){
	struct lneb_attr *lna;
	int lvl;

	lna = luaL_checkudata(lua, 1, LNEB_ATTRIBUTE);
	lvl = neb_attr_unlock(lna->attr);

	if (lvl < 0)
		return luaL_error(lua, "Unable to unlock attribute");
	lua_pushnumber(lua, lvl);
	return 1;
}


static int
lneb_elem_value_add(lua_State *L){
	struct lneb_ref *lnr;
	struct lneb_attr *lna;
	int val;

	lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);
	val = luaL_checknumber(L, 2);

	/* FIXME: Should be lnev_elem? */
	lnr = lua_newuserdata(L, sizeof(struct lneb_ref));
	lnr->neb = lna->neb;
	lnr->elem = neb_attr_elem_value_add(lna->attr, val);

	return 1;
}

static int
lneb_attr_tostring(lua_State *lua){
	struct lneb_attr *lna;

	lna = luaL_checkudata(lua, 1, LNEB_ATTRIBUTE);

	lua_pushfstring(lua, "Attribute '%s' (= %d)%s",
			neb_attr_name_get(lna->attr),
			neb_attr_value_get(lna->attr),
			neb_attr_lock_get(lna->attr)>0?" Locked":"");
	return 1;
}

/* FIXME: Optional argument for check */
static int
lneb_elem_ref_add(lua_State *L){
	struct lneb_ref *lnr;
	struct lneb_attr *lna;
	const char *refto = NULL;
	bool docheck;

	lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);
	if (lua_istable(L,2)){
		lua_getfield(L,2,"ref");
		refto = lua_tostring(L,-1);
	} else {
		refto = luaL_checkstring(L,2);
	}
	luaL_argcheck(L, refto, 1, "Need something to reference to");

	if (lua_isboolean(L,3))
		docheck = lua_toboolean(L,3);
	else
		docheck = true;

	lnr = lua_newuserdata(L, sizeof(struct lneb_ref));
	lnr->neb = lna->neb;
	lnr->elem = neb_attr_elem_reference_add(lna->attr, refto, docheck);
	if (!lnr->elem){
		/* FIXME clean up */
		printf("%p %s %d\n",lna->attr,refto, docheck);
		return luaL_error(L, "Failed to add reference");
	}
	return 1;
}


int
luaneb_stackdump(lua_State* l){
    int i;
    int top = lua_gettop(l);

    printf("total in stack %d\n",top);
    for (i = top; i ; i--) {
        int t = lua_type(l, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
                printf("%2d  string: '%s'\n", i,lua_tostring(l, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                printf("%2d  boolean %s\n",i,
				lua_toboolean(l, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                printf("%2d  number: %g\n", i,lua_tonumber(l, i));
                break;
            default:  /* other values */
                printf("%2d  %s\n", i,lua_typename(l, t));
                break;
        }
    }
    return top;
}
