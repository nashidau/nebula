

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

struct neb_attr *neb_character_attribuate_add(struct neb_character *,
		const char *name);
struct neb_note *neb_character_note_add(struct neb_character *,
		const char *key);


struct neb_elem *neb_attribute_elem_add(struct neb_attr *,
		enum neb_elem_type);




