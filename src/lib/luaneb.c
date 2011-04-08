#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "nebula.h"
#include "nebula_private.h"

static int luaneb_character_push(lua_State *, struct neb_character *nc, const char *);


static int lneb_character_add(lua_State *);
static int lneb_character_get(lua_State *);
static int lneb_character_save(lua_State *);
static int lneb_character_name_set(lua_State *);
static int lneb_character_name_get(lua_State *);
static int lneb_attr_add(lua_State *);
static int lneb_attr_get(lua_State *);
static int lneb_attr_prop_add(lua_State *);
static int lneb_note_add(lua_State *);
static int lneb_note_get(lua_State *);
static int lneb_attr_value_get(lua_State *);
static int lneb_elem_value_add(lua_State *);
static int lneb_elem_ref_add(lua_State *);
static int lneb_attr_filter_set(lua_State *);
static int lneb_lock(lua_State *);
static int lneb_unlock(lua_State *);
static int lneb_attr_tostring(lua_State *);
static int lneb_attr_index(lua_State *);

static int lneb_elem_index(lua_State *);

int luaneb_stackdump(lua_State* l);


struct lneb_char {
	struct nebula *neb;
	struct neb_character *ch;
};

struct lneb_attr {
	struct nebula *neb;
	struct neb_attr *attr;
};


struct lneb_elem {
	struct nebula *neb;
	struct neb_elem *elem;
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
	{ "note_get",		lneb_note_get },
	{ NULL,		NULL },
};

static const struct luaL_Reg attrfns[] = {
	{ "value_get",	lneb_attr_value_get },
	{ "value_add",	lneb_elem_value_add },
	{ "prop_add",	lneb_attr_prop_add },
	{ "property_add",lneb_attr_prop_add },
	{ "ref_add",	lneb_elem_ref_add },
	{ "filter_set", lneb_attr_filter_set },
	{ "lock",	lneb_lock },
	{ "unlock",	lneb_unlock },
	{ "__call",	lneb_attr_value_get },
	{ "__tostring",	lneb_attr_tostring },
	{ "__index",	lneb_attr_index },
	{ NULL,		NULL },
};

static const struct luaL_Reg elemfns[] = {
	{ "__index",	lneb_elem_index },
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
	if (!L){
		L = luaL_newstate();
		if (!L) return -1;
		neb->L = L;
		luaL_openlibs(L);
	}
	LEMPTY(L);

	lua_pushstring(L, "Nebula");  /* push address */
	lua_pushlightuserdata(L, neb);  /* push value */
	lua_settable(L, LUA_REGISTRYINDEX);


	LEMPTY(L);
	rv = luaL_dostring(L,"require[[nebula]]\n");
	if (rv){
		printf("Error with dostring: %s\n",lua_tostring(L,-1));
		return -1;
	}
	LEMPTY(L);


	return 0;
}



/**
 * Apply a template to a character.
 *
 * The character will be pushed as the value 'ch' into the global space.
 *
 * @param nc Nebula character.
 * @param template Name of the file.
 * @return 0 on success.
 */
int
neb_character_template_apply(struct neb_character *nc, const char *template){
	struct nebula *neb;
	lua_State *L;
	int rv;

	if (!nc || !template) return -1;

	neb = nc->neb;
	L = neb->L;
	assert(neb);
	assert(L);

	/* open the template */
	rv = luaL_loadfile(L, template);
	if (rv){
		if (rv == LUA_ERRFILE) printf("No file %s\n",template);
		if (rv == LUA_ERRSYNTAX) printf("Syntax error\n");
		if (rv == LUA_ERRMEM) printf("Memory error\n");
		printf("Erg an error from loadfile\n");
		return -1;
	}

	/* push the character */
	luaneb_character_push(L,nc,"ch");

	/* Call the template */
	rv = lua_pcall(L,0, 1, 0);
	if (rv){
		printf("Error with pcall\n");
		printf("\tError: %s\n",lua_tostring(L,-1));
		return -1;
	}
	/* FIXME: Check return */
	lua_pop(L,1);

	/* Destroy the state */

	return 0;
}


/**
 * Push the character as the specified name.
 *
 * @param L Lua state to push to.
 * @param nc Nebula character.
 * @param name Name of variable to add.
 * @param 0 on success, -1 on error. 
 */
static int
luaneb_character_push(lua_State *L, struct neb_character *nc, const char *name){
	struct lneb_char *lnc;

	lnc = lua_newuserdata(L, sizeof(struct lneb_char));
	if (!lnc) return -1;

	luaL_getmetatable(L, LNEB_CHARACTER);
	lua_setmetatable(L, -2);
	lua_setglobal(L, name);

	/* FIXME: Inefficient getting twice */
	lnc->ch = nc;
	lnc->neb = nc->neb;



	return 0;
}

/**
 * Push an attribute onto the stack
 *
 * @param L Lua state
 * @param attr Attribute to push
 * @return 0 on success, -1 on error.
 * FIXME: Use this internally
 */
int
luaneb_attr_push(struct nebula *neb, struct neb_attr *attr){
	struct lneb_attr *lna;
	lua_State *L;

	if (!neb || !neb->L || !attr) return -1;
	L = neb->L;

	lna = lua_newuserdata(L, sizeof(struct lneb_attr));
	lna->attr = attr;
	lna->neb = attr->ch->neb;

	if (luaL_newmetatable(L,LNEB_ATTRIBUTE)){
                luaL_register(L, NULL, attrfns);
        }

        lua_setmetatable(L,-2);

	return 1;
}
/**
 * Push an element onto the stack
 *
 * @param L Lua state
 * @param elem Element to push
 * @return 0 on success, -1 on error.
 * FIXME: Use this internally
 */
int
luaneb_elem_push(struct nebula *neb, struct neb_elem *elem){
	struct lneb_elem *lne;
	lua_State *L;

	if (!neb || !neb->L || !elem) return -1;
	L = neb->L;

	lne = lua_newuserdata(L, sizeof(struct lneb_elem));
	lne->elem = elem;
	lne->neb = neb;

	if (luaL_newmetatable(L,LNEB_ELEMENT)){
                luaL_register(L, NULL, elemfns);
        }

        lua_setmetatable(L,-2);

	return 1;
}




struct neb_character *
luaneb_tocharacter(lua_State *L, int index){
	struct lneb_char *lnc;
	if (!L) return NULL;
	lnc = luaL_checkudata(L,index,LNEB_CHARACTER);
	return lnc->ch;
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
lneb_attr_index(lua_State *L){
	const char *field;
        struct lneb_attr *lna;

        field = luaL_checkstring(L,2);

        lua_getmetatable(L,1);
        lua_getfield(L,-1,field);

        if (!lua_isnil(L,-1))
                return 1;

        lua_pop(L,2);

        lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);

        if (streq(field,"name")){
                lua_pushstring(L,neb_attr_name_get(lna->attr));
        } else if (streq(field,"value")){
                lua_pushnumber(L,neb_attr_value_get(lna->attr));
        } else {
                return luaL_error(L,"Unknown field %s",field);
        }
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

	if (extra){
		/* Do properties */
		lua_getfield(L,2,"props");
		/* FIXME: do in a function */
		if (lua_istable(L,-1)){
			lua_pushnil(L);
			while (lua_next(L, -2) != 0) {
				const char *p,*v;
				p = lua_tostring(L,-2);
				v = lua_tostring(L,-1);
				neb_attr_prop_add(lna->attr,p,v);
				lua_pop(L, 1);
			}
		}
		lua_pop(L,1);
	}

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
		return luaL_error(lua, "Can't find attribute '%s'",name);

	/* FIXME: generalise wuth attr_add */
	lna = lua_newuserdata(lua, sizeof(struct lneb_attr));
	lna->neb = lnc->neb;
	lna->attr = at;

	luaL_getmetatable(lua, LNEB_ATTRIBUTE);
	lua_setmetatable(lua, -2);

	return 1;
}

static int
lneb_attr_prop_add(lua_State *L){
	struct lneb_attr *lna;
	const char *p,*v;
	int rv;

	lna = luaL_checkudata(L,1,LNEB_ATTRIBUTE);
	p = luaL_checkstring(L,2);
	v = lua_tostring(L,3); /* Value optional */

	rv = neb_attr_prop_add(lna->attr, p, v);

	lua_pushboolean(L,!rv);
	return 1;
}


static int
lneb_note_add(lua_State *L){
	struct lneb_char *lnc;
	const char *key;
	const char *value;
	struct neb_note *note;

	lnc = luaL_checkudata(L, 1, LNEB_CHARACTER);
	key = luaL_checkstring(L,2);
	luaL_argcheck(L, lnc && lnc->ch, 1, "Character must be initied");

	if (lua_isstring(L,3))
		value = lua_tostring(L,3);
	else
		value = NULL;

	note = neb_character_note_add(lnc->ch, key, value);

	lua_pushboolean(L,note != NULL);
	return 1;
}

static int
lneb_note_get(lua_State *L){
	struct lneb_char *lnc;
	const char *key;
	const char *value;
	struct neb_note *note;

	lnc = luaL_checkudata(L, 1, LNEB_CHARACTER);
	key = luaL_checkstring(L,2);
	luaL_argcheck(L, lnc && lnc->ch, 1, "Character must be initied");

	note = neb_character_note_find(lnc->ch, key);
	if (!note){
		lua_pushnil(L);
		lua_pushfstring(L, "Unable to find note '%s'",key);
		return 2;
	}
	value = neb_note_body_get(note);
	if (value)
		lua_pushstring(L,value);
	else
		lua_pushboolean(L,0);
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
	const char *note;

	lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);
	val = luaL_checknumber(L, 2);
	note = luaL_optstring(L,3,NULL);

	/* FIXME: Should be lnev_elem? */
	lnr = lua_newuserdata(L, sizeof(struct lneb_ref));
	lnr->neb = lna->neb;
	lnr->elem = neb_attr_elem_value_add(lna->attr, note, val);

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
	const char *transform = NULL;
	const char *note = NULL;
	bool docheck = true;

	lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);
	if (lua_istable(L,2)){
		lua_getfield(L,2,"ref");
		refto = lua_tostring(L,-1);
		lua_getfield(L,2,"transform");
		transform = lua_tostring(L,-1);
		lua_getfield(L,2, "note");
		note = lua_tostring(L,-1);
		lua_pop(L,3);
	} else {
		refto = luaL_checkstring(L,2);
	}
	luaL_argcheck(L, refto, 1, "Need something to reference to");

	if (lua_isboolean(L,3))
		docheck = lua_toboolean(L,3);
	else if (lua_istable(L,2)){
		lua_getfield(L,2,"check");
		if (lua_isboolean(L,-1))
			docheck = lua_toboolean(L,-1);
	}

	lnr = lua_newuserdata(L, sizeof(struct lneb_ref));
	lnr->neb = lna->neb;
	lnr->elem = neb_attr_elem_reference_add(lna->attr, note, refto,docheck);
	if (!lnr->elem){
		/* FIXME clean up */
		printf("%p %s %d\n",lna->attr,refto, docheck);
		return luaL_error(L, "Failed to add reference");
	}

	if (transform)
		neb_elem_ref_transform_set(lnr->elem, transform);

	return 1;
}


static int
lneb_attr_filter_set(lua_State *L){
	struct lneb_attr *lna;
	const char *filter;

	lna = luaL_checkudata(L, 1, LNEB_ATTRIBUTE);
	filter = luaL_checkstring(L, 2);

	/* FIXME: Check error return */
	neb_attr_filter_set(lna->attr, filter);

	lua_pushboolean(L,1);
	return 1;
}




/** Element functions.
 * FIXME: Put in own file.
 */
static int
lneb_elem_index(lua_State *L){
	const char *field;
        struct lneb_elem *lne;

        field = luaL_checkstring(L,2);

        lua_getmetatable(L,1);
        lua_getfield(L,-1,field);

        if (!lua_isnil(L,-1))
                return 1;

        lua_pop(L,2);

        lne = luaL_checkudata(L, 1, LNEB_ELEMENT);

        if (streq(field,"value")){
                lua_pushnumber(L,neb_elem_value_get(lne->elem));
        } else {
                return luaL_error(L,"Unknown field %s",field);
        }
        return 1;
}

/* Misc functions */


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


/**
 * Returns a quoted version of a string safe for printing as Lua code.
 *
 * For simplicity it always uses [[ ]] style strings (even without newlines).
 * If there is a ]] in the string it will try [=[ until it finds one which is
 * safe.
 *
 * You want to use it like:
 * 	str = luaneb_quote_str(foo->val);
 * 	fprintf(luafile, "{ val = %s }", str);
 * 	free(str);
 *
 * @param str String to save
 * @return String that must be freed.
 */
char *
luaneb_quote_str(const char *str){
	char *equals = strdup("");
	char *buf;
	int i;
	int len;

	if (!str) return NULL;

	if (strstr(str,"]]")){
		equals = strdup("=");
		for (i = 0 ; strstr(str,equals) ; i ++){
			equals = realloc(equals, i + 1);
			memset(equals,'=',i);
			equals[i] = 0;
		}
	}

	len = strlen(str);
	len += 2 * strlen(equals) + 4 /* [[ ]] */+ 1 /* \0 */;
	buf = malloc(len);
	if (!buf){
		free(equals);
		return buf;
	}
	snprintf(buf,len, "[%s[%s]%s]",equals, str, equals);

	free(equals);

	return buf;
}


int
luaneb_table_count(lua_State *lua, int table){
        int count = 0;
        if (!lua_istable(lua, table))
                return -1;
        lua_pushnil(lua);  /* first key */
        while (lua_next(lua, table) != 0) {
                count ++;
                lua_pop(lua, 1);
        }
        return count;
}


void
luaneb_stack_dump(lua_State *lua,const char *msg,...){
        int i,n,st;
        const char *str;
        va_list ap;

        if (!msg)
                printf("Current Lua Stack:");
        else {
                va_start(ap, msg);
                vprintf(msg, ap);
                va_end(ap);
                printf("\n");
        }
        n = lua_gettop(lua);
        if (n == 0){
                printf("\tEmpty stack\n");
                return;
        } else if (n < 0){
                printf("\tMassive stack corruption: Top is %d\n",n);
                assert(n >= 0);
                return;
        }
        for (i = 1; i <= n; i++){
                printf("%7d: %s ",i, lua_typename(lua,lua_type(lua,i)));
                switch (lua_type(lua,i)){
                case LUA_TNIL:
                        printf(" nil\n");
                        break;
                case LUA_TNUMBER:
                        printf(" %lf\n",lua_tonumber(lua,i));
                        break;
                case LUA_TBOOLEAN:
                        printf(" %s\n", lua_toboolean(lua,i) ? "true":"false");
                        break;
                case LUA_TSTRING:
                        printf(" '%s'\n", lua_tostring(lua,i));
                        break;
                case LUA_TLIGHTUSERDATA:
                        printf(" %p\n", lua_touserdata(lua,i));
                        break;
               case LUA_TTABLE:
                        lua_pushstring(lua, "__mode");
                        lua_rawget(lua,i);
                        if (lua_isnil(lua, -1))
                                lua_pop(lua,1);
                        else {
                                const char *m = lua_tostring(lua, -1);
                                printf("Weak");
                                if (strchr(m,'k')) printf(" keys");
                                if (strchr(m,'v')) printf(" values");
                                lua_pop(lua,1);
                        }
                        printf(" %d items (%d array items)",
                                luaneb_table_count(lua,i),
                                (int)lua_objlen(lua,i));
                        if (luaL_callmeta(lua, i, "__tostring")){
                                str = lua_tostring(lua,-1);
                                printf("\t%s\n",str);
                                lua_pop(lua,1);
                        }
                        printf("[%p]\n",lua_topointer(lua,i));
                        lua_getfield(lua, i, "name");
                        if (lua_isstring(lua,-1)){
                                printf("\t\tQuest Name is %s\n",lua_tostring(lua,-1));
                        }
                        lua_pop(lua,1);
                        break;
                case LUA_TTHREAD:
                        st = lua_objlen(lua,i);
                        if (st == 0) printf(" %d Okay",st);
                        else if (st == LUA_YIELD) printf(" %d yeild\n",st);
                        else if (st == LUA_ERRRUN) printf(" %d Error Run\n",st);
                        else if (st == LUA_ERRSYNTAX)
                                printf(" %d Error Syntax\n",st);
                        else if (st == LUA_ERRMEM)
                                printf(" %d Error Memory\n",st);
                        else if (st == LUA_ERRERR)
                                printf(" %d Error Error\n",st);
                        break;
                case LUA_TFUNCTION:
                        printf(" (%p)\n", lua_topointer(lua,i));
                        break;
                default:
                        if (luaL_callmeta(lua, i, "__tostring")){
                                str = lua_tostring(lua,-1);
                                lua_pop(lua,1);
                                printf(" %s\n",str);
                        } else {
                                printf(" (no __tostring function)\n");
                        }
                }
        }
}

