
#include <stdbool.h>

#include <Elementary.h>
#include <nebula.h>

#include "etac.h"

#define DEBUG(...) printf(__VA_ARGS__)

static char *attr_label_get(const void *, Evas_Object *, const char *part);
static Evas_Object *attr_icon_get(const void *, Evas_Object *, const char *part);
static Eina_Bool attr_state_get(const void *, Evas_Object *, const char *part);
static void attr_del(const void *, Evas_Object *);

static const Elm_Genlist_Item_Class attr_class = {
	.item_style = "default",
	.func = {
		.label_get = attr_label_get,
		.icon_get = attr_icon_get,
		.state_get = attr_state_get,
		.del = attr_del,
	}
};


struct etac_attr {
	int magic;
	struct neb_attr *attr;

	Evas_Object *value;
};

enum {
	ETAC_ATTR_MAGIC = 37738989,
};

#define ETAC_MAGIC_CHECK(ea, rv)			\
	do {						\
		if (!ea){				\
			DEBUG("Eattr is NULL\n");	\
			return rv;			\
		}					\
		if (ea->magic != ETAC_ATTR_MAGIC){	\
			DEBUG("Eattr magic check\n");	\
			return rv;			\
		}					\
	} while (0)



static char *
attr_label_get(const void *eacv, Evas_Object *obj, const char *part){
	struct etac_attr *eac = (void *)eacv;

	ETAC_MAGIC_CHECK(eac, NULL);

//	printf("label: Part is %s\n",part);
	if (strcmp(part,"elm.text") == 0){
		return strdup(neb_attr_name_get(eac->attr));
	}

	return NULL;
}
static Evas_Object *
attr_icon_get(const void *eacv, Evas_Object *obj, const char *part){
	struct etac_attr *eac = (void *)eacv;
	char buf[100];
	Evas_Object *o;

	ETAC_MAGIC_CHECK(eac, NULL);
//	printf("Part is %s\n",part);

	if (strcmp("elm.swallow.end",part) == 0){
		o = elm_label_add(obj);
		snprintf(buf,100,"%d",neb_attr_value_get(eac->attr));
		elm_label_label_set(o, buf);
		eac->value = o;
		return o;
	}
	return NULL;
}
static Eina_Bool
attr_state_get(const void *eacv, Evas_Object *obj, const char *part){
	struct etac_attr *eac = (void *)eacv;

	ETAC_MAGIC_CHECK(eac, false);
	printf("Part is %s\n",part);

	return true;
}

static void
attr_del(const void *eacv, Evas_Object *obj){
	struct etac_attr *eac = (void *)eacv;

	ETAC_MAGIC_CHECK(eac, (void)0);
	return;
}





static Eina_Bool
etac_attr_append(const void *container, void *attrv, void *etacv){
	struct neb_attr *attr = attrv;
	struct etac_attr *eac;
	struct etac *etac = etacv;
	const char *name;

	name = neb_attr_name_get(attr);
	printf("Add attr %s\n",name);

	eac = malloc(sizeof(struct etac_attr));
	eac->magic = ETAC_ATTR_MAGIC;
	eac->attr = attr;
	elm_genlist_item_append(etac->gl, &attr_class, eac, NULL, 0, NULL, 0);

	return true;
}


/**
 * Take a loaded character, and update the UI for it.
 *
 * So create windows etc as needed.
 *
 * For now UI is just one big list: just display all attrs in the list
 */
int
etac_chr_set(struct etac *etac){
	Eina_Iterator *iter;
	if (!etac) return -1;
	if (!etac->chr) return -1;

	DEBUG("Start etac character set\n");

	iter = neb_character_attr_iter_new(etac->chr);

	eina_iterator_foreach(iter, etac_attr_append, etac);

	return 0;
}


