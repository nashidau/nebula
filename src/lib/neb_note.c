/**
 * Note interface for characters.
 *
 * Notes provide a number of features.  The first is for character notes such
 * as background, journal entries, player notes, or even messages from the GM.  
 *
 * Secondly they provide a mechanism for storing other character data not
 * directly represented as part of a characters attributes.  These are things
 * like a dnd's feats or a talent from Harp or rolemaster.
 *
 * Notes consist of a title, and some body text.  Optionally they may consist
 * of a list of tags.
 *
 * Tags allow notes to be easily grouped or retreived in a particular
 * category.  For instance all Journal entries should be tagged with journal,
 * while feats would be tagged with feat.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Eina.h>

#include "nebula.h"
#include "nebula_private.h"


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

/**
 * Delete the notes list from a character.
 *
 * @param nc Character to get the notes list.
 * @param nt the notes list be deleted from a character.
 * @return -1 on error, 0 otherwise.
 */
int
neb_ch_note_del(struct neb_character *nc, struct neb_note *nt){
	if (!nc) return -1;
	if (!nt) return 0;

	nc->notes = eina_list_remove(nc->notes, nt);

	if (nt->name) free((char *) nt->name);
	if (nt->note) free((char *) nt->note);
	if (nt) free(nt);

	return 0;
}

/**
 * Get a note by it's title.
 *
 * This retrieves the note with the given title.  Titles are (meant) to be
 * unique.  If a character has multiple notes with the same title, any may be
 * retrieved by this function.
 *
 * @param ch Character to get note from.
 * @param title Title of note to find.
 * @return Note or NULL on error.
 */
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

/**
 * Add a tag to a note.
 *
 * @todo Handle duplicates gracefully.
 * @param note Note to add tag too.
 * @param tag Tag to add.
 * @return 0 on success, non-zero on error.
 */
int
neb_note_tag_add(struct neb_note *note, const char *tag){
	if (!note) return -1;
	if (!tag) return -1;

	/* FIXME: check for duplicates */
	note->tags = eina_list_append(note->tags, strdup(tag));

	return !!note->tags;
}

/**
 * Remove a tag from a note.
 *
 * If the note does not have the tag, an error is returned.
 *
 * @param note Note to remove tag from.
 * @param tag Tag to remove.
 * @return 0 on success, non-zero on error.
 */
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

/**
 * Returns a list of notes with a particular tag.
 *
 * The returned list must be freed, but the notes themselves should not.
 * The returned list is invalidated if any note is deleted.
 *
 * @param ch Nebular character
 * @param tag Tag to search for
 * @return list of notes to be freed, or NULL on error.
 */
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


/**
 * Append text to an existing note.
 *
 * If an error occurs with memory allocation, the existing text is preserved.
 *
 * @param note Note to add text too
 * @param text Text to add.
 * @return 0 on success, -1 on error.
 */
int
neb_note_text_append(struct neb_note *note, const char *text){
	char *nbuf;
	int len,len2;
	if (!note) return -1;
	if (!text) return 0;

	if (!note->note){
		note->note = strdup(text);
		return !note->note;
	}

	len = strlen(note->note);
	len2 = strlen(text);

	nbuf = realloc(note->note, len + len2 + 1);
	if (!nbuf)
		return -1;

	note->note = nbuf;
	memcpy(note->note + len, text, len2);
	note->note[len + len2] = '\0';

	return 0;
}

/**
 * Create a new iterator to iterate over all notes.
 *
 * The iterator should be freed when done.
 *
 * @param nch Nebula character to generate iterator for
 * @retuen New note iterator.
 */
Eina_Iterator *
neb_character_note_iter_new(struct neb_character *nch){
	if (!nch) return NULL;
	return eina_list_iterator_new(nch->notes);
}

/**
 * Gets the title of a note.
 *
 * The return value should not be modified by the caller.  It is only
 * guaranteed to be valid until the next note API call.
 *
 * @param note Note to get title of.
 * @return Note name or NULL on error.
 */
const char *
neb_note_title_get(struct neb_note *note){
	if (!note) return NULL;
	return note->name;
}

/**
 * Gets the body of a note.
 *
 * The return value should not be modified by the caller.  It is only
 * guaranteed to be valid until the next note API call.
 *
 * @param note Note to get body of.
 * @return Note name or NULL on error.
 */
const char *
neb_note_body_get(struct neb_note *note){
	if (!note) return NULL;
	return note->note;
}
