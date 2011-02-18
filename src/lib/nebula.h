#ifndef NEBULA_H
#define NEBULA_H 1

#include <Eina.h>

struct nebula;
struct neb_character;
struct neb_attr;
struct neb_note;
struct neb_elem;

enum neb_elem_type {
	NEB_ELEM_VALUE,
	NEB_ELEM_REFERENCE,
};

struct nebula *nebula_init(void);
int nebula_free(struct nebula *);

struct neb_character *neb_character_new(struct nebula *);

int neb_character_name_set(struct neb_character *nc, const char *name);
const char * neb_character_name_get(struct neb_character *nc);
struct neb_character *neb_character_get(struct nebula *nebula, const char *name);
struct nebula *neb_character_neb_get(struct neb_character *nc);



struct neb_character *neb_character_load(struct nebula *nebula, const char *name);
int nebula_character_save(struct neb_character *ch, const char *file);

struct neb_attr *neb_character_attr_add(struct neb_character *,
		const char *name);
struct neb_attr *neb_character_attr_get(struct neb_character *,
		const char *name);

struct neb_note *neb_character_note_add(struct neb_character *,
		const char *title, const char *body);
int neb_note_text_append(struct neb_note *, const char *);
const char *neb_note_title_get(struct neb_note *);
const char *neb_note_body_get(struct neb_note *);
struct neb_note *neb_character_note_find(struct neb_character *ch,
		const char *title);
int neb_note_tag_add(struct neb_note *note, const char *tag);
int neb_note_tag_remove(struct neb_note *note, const char *tag);
Eina_List *neb_charcter_note_tagged_get(struct neb_character *ch, const char *);

struct neb_elem *neb_attr_elem_add(struct neb_attr *,
		enum neb_elem_type);
struct neb_elem *neb_attr_elem_value_add(struct neb_attr *, int value);
struct neb_elem *neb_attr_elem_reference_add(struct neb_attr *,
		const char *refname, bool check);
int neb_attr_elem_del(struct neb_attr *, struct neb_elem *);
int neb_attr_value_get(struct neb_attr *attr);
int neb_attr_lock(struct neb_attr *attr);
int neb_attr_unlock(struct neb_attr *attr);
int neb_attr_lock_get(struct neb_attr *attr);
const char *neb_attr_name_get(struct neb_attr *attr);
int neb_attr_prop_add(struct neb_attr *, const char *prop, const char *value);
const char *neb_attr_prop_get(struct neb_attr *attr, const char *prop);
Eina_List *neb_attr_prop_list_get(struct neb_attr *attr);
int neb_attr_prop_list_free(Eina_List *);

int neb_elem_ref_transform_set(struct neb_elem *elem, const char *transform);

Eina_Iterator *neb_character_attr_iter_new(struct neb_character *);
Eina_Iterator *neb_attr_elem_iter_new(struct neb_attr *attr);
Eina_Iterator *neb_character_note_iter_new(struct neb_character *);

enum neb_elem_type neb_elem_type_get(struct neb_elem *el);
const char *neb_elem_reference_get(struct neb_elem *el);
int neb_elem_value_get(struct neb_elem *el);
const char *neb_elem_ref_transform_get(struct neb_elem *el);

#endif
