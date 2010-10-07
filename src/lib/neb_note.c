#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <Eina.h>

#include "nebula.h"
#include "nebula_private.h"


/* FIXME: Use string shares for at least the tags */


struct neb_note {
	char *name;
	char *note;

	Eina_List *tags;
};

/**
 * Add a new note to a character.
 *
 * The note will be appended to the character.  The title of the note must be
 * unique.  The text is optional, and can be added to manually later.
 *
 * The new note is returned.
 *
 * Note that certain note names are reserved by Nebula, and it's viewing
 * library.
 *
 * @param nc Nebula character to add note too.
 * @param title Title of note, must be unique to character.
 * @param text Optional body text.
 * @return New note on success, NULL on error.
 */
struct neb_note *
neb_character_note_add(struct neb_character *nc, const char *title,
		const char *text){
	struct neb_note *note;
	int rv;

	if (!nc || !title) return NULL;

	note = calloc(1,sizeof(struct neb_note));
	if (!note) return NULL;

	note->name = strdup(title);
	if (!note->name){
		free(note);
		return NULL;
	}

	if (text){
		rv = neb_note_text_append(note, text);
		if (rv){
			free(note->name);
			free(note);
			return NULL;
		}
	}


	nc->notes = eina_list_append(nc->notes, note);
	return note;
}

struct neb_note *
neb_character_note_find(struct neb_character *ch, const char *title){
	Eina_List *l;
	struct neb_note *note;

	if (!ch) return NULL;
	if (!title) return NULL;

	EINA_LIST_FOREACH(ch->notes, l, note){
		if (strcmp(note->name, title) == 0)
			return note;
	}

	return NULL;
}

int
neb_note_tag_add(struct neb_note *note, const char *tag){
	if (!note) return -1;
	if (!tag) return -1;

	/* FIXME: check for duplicates */
	note->tags = eina_list_append(note->tags, strdup(tag));

	return !!note->tags;
}

int
neb_note_tag_remove(struct neb_note *note, const char *tag){
	Eina_List *l;
	char *tmp;

	if (!note) return -1;
	if (!tag) return 0;

	EINA_LIST_FOREACH(note->tags, l, tmp){
		if (strcmp(tmp,tag) == 0){
			note->tags = eina_list_remove_list(note->tags,l);
			return 0;
		}
	}

	return -1;
}

Eina_List *
neb_charcter_note_tagged_get(struct neb_character *ch, const char *tag){
	Eina_List *ntmp, *tl, *nl;
	struct neb_note *note;
	char *ntag;

	if (!ch) return NULL;
	if (!tag) return NULL;

	nl = NULL;
	EINA_LIST_FOREACH(ch->notes, ntmp, note){
		EINA_LIST_FOREACH(ch->notes, tl, ntag){
			if (strcmp(ntag, tag) == 0){
				nl = eina_list_append(nl, note);
				break;
			}
		}
	}

	return nl;
}
