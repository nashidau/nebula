#define _GNU_SOURCE
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


#include "nebula.h"
#include "nebula_private.h"


int
luaneb_filter_init(struct neb_attr *attr){
	lua_State *L;
	struct neb_character *nv;
	int top;
	int fref;
	int rv;
	const char *name, *end = NULL;
	printf("lua neb filter init\n");

	L = attr->ch->neb->L;

	top = lua_gettop(L);

	/* Find filter */
	lua_getglobal(L, "Filters");
	name = attr->filter;

	do {
		if (end) name = end + 1;
		end = strchrnul(name, '.');
		lua_pushlstring(L, name, end-name);
		lua_rawget(L, -2);
	} while (*end);

	/* Push attr */
	luaneb_attr_push(attr->ch->neb, attr);

	/* Call it */
	rv = lua_pcall(L, 1, 1, 0);
	if (rv){
		printf("Error: %s\n",lua_tostring(L,-1));
	}

	/* Save in registry */
	fref = luaL_ref(L, LUA_REGISTRYINDEX);

	/* clean up */
	lua_settop(L,top);

	return fref;
}
int
luaneb_filter_next(struct neb_attr *attr, int fref, struct neb_elem *el){
	lua_State *L;
	int rv;

	printf("Filter next!\n");

	L = attr->ch->neb->L;

	lua_rawgeti(L, LUA_REGISTRYINDEX, fref);

	/* Table on top */
	lua_pushstring(L, "next");
	lua_rawget(L,-2);

	luaneb_elem_push(attr->ch->neb, el);

	rv = lua_pcall(L, 1, 1, 0);
	if (rv){
		printf("Error: %s\n",lua_tostring(L,-1));
	}

	return 0;
}
int
luaneb_filter_done(struct neb_attr *attr, int fref){
	lua_State *L;
	int val;

	printf("Filter done\n");

	L = attr->ch->neb->L;

	lua_rawgeti(L, LUA_REGISTRYINDEX, fref);

	/* Table on top */
	lua_pushstring(L, "done");
	lua_rawget(L,-2);

	//luaneb_attr_push(L, attr);

	lua_pcall(L, 0, 1, 0);
	val = lua_tointeger(L,-1);
	lua_pop(L,2); /* Table & rv */

	/* Clean up registry */

	return val;
}
