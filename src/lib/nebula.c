
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

	luaneb_init(neb);

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



