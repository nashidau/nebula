
#include <stdbool.h>
#include <stdio.h>

#include <lua.h>

#include <Eina.h>

#include "nebula.h"
#include "nebula_private.h"

/**
 * Initialise the nebula library.
 *
 * The nebula instance should be passed to most other nebula functions.
 * Multiple instances may exist, but data may not be used interchangeably
 * between them unless otherwise indicated.
 * 
 * Nebula will also init the Eina library.
 *
 * @return Nebula instance, or NULL on error.
 */
struct nebula *
nebula_init(void){
	struct nebula *neb;

	eina_init();

	neb = malloc(sizeof(struct nebula));
	neb->characters = NULL;

	return neb;
}

/**
 * Free a nebula instance, freeing all it's data.
 *
 * Note that you should make sure any changed data is saved before this
 * function is called.
 * 
 * @param neb Nebula instance.
 * @return 0 on success.
 */
int
nebula_free(struct nebula *neb){

	eina_shutdown();

	/* FIXME: Really clean up */

	free(neb);
	return 0;
}

/**
 * Add a character to be managed by nebula.
 *
 * If the character is already managed by another nebula instance, it should
 * be removed first.
 *
 * Note the neb_character_new() automatically adds the character to a nebula
 * instance.
 *
 * @param neb Nebula instance.
 * @param ch Character to add.
 * @return -1 on error, 0 otherwise.
 */
int
nebula_charater_add(struct nebula *neb, struct neb_character *ch){
	if (!neb) return -1;
	if (!ch) return -1;

	neb->characters = eina_list_append(neb->characters, ch);
	return 0;
}

/**
 * Load a character.
 */
struct neb_character *
neb_character_load(struct nebula *nebula, const char *name){
	printf("not implemented\n");
	return NULL;
}


