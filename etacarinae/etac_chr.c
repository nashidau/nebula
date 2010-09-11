
#include <stdbool.h>

#include <Elementary.h>
#include <nebula.h>

#include "etac.h"

#define DEBUG(...) printf(__VA_ARGS__)

struct etac_attr_group {
	const char *group;
	Elm_Genlist_Item *item;
};


struct etac_attr {
	int magic;
	struct etac *etac;
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



static char *attr_label_get(const void *, Evas_Object *, const char *part);
static Evas_Object *attr_icon_get(const void *, Evas_Object *, const char *part);
static Eina_Bool attr_state_get(const void *, Evas_Object *, const char *part);
static void attr_del(const void *, Evas_Object *);

static char *group_label_get(const void *, Evas_Object *, const char *part);
static Evas_Object *group_icon_get(const void *, Evas_Object *, const char *part);
static Eina_Bool group_state_get(const void *, Evas_Object *, const char *part);
static void group_del(const void *, Evas_Object *);




static const Elm_Genlist_Item_Class attr_class = {
	.item_style = "default",
	.func = {
		.label_get = attr_label_get,
		.icon_get = attr_icon_get,
		.state_get = attr_state_get,
		.del = attr_del,
	}
};

static const Elm_Genlist_Item_Class group_class = {
	.item_style = "default",
	.func = {
		.label_get = group_label_get,
		.icon_get = group_icon_get,
		.state_get = group_state_get,
		.del = group_del,
	}
};

Eina_List *groups;

/**
 * Attr callbacks for the genlist
 */
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
	struct neb_attr *subat = NULL;
	int val,val2;
	char buf[100];
	const char *sub;
	const char *fmt = NULL;
	Evas_Object *o;

	ETAC_MAGIC_CHECK(eac, NULL);

	if (strcmp("elm.swallow.end",part) != 0)
		return NULL;

	sub = neb_attr_prop_get(eac->attr,"Subvalue");
	if (sub){
		subat = neb_character_attr_get(eac->etac->chr, sub);
		if (!subat){
			printf("Unable to find sub attribute\n");
			return NULL;
		}
		fmt = neb_attr_prop_get(eac->attr, "Format");
	}


	o = elm_label_add(obj);
	val = neb_attr_value_get(eac->attr);
	if (sub) {
		val2 = neb_attr_value_get(subat);
		if (fmt)
			snprintf(buf,100,fmt,val,val2);
		else
			snprintf(buf,100,"%d (%+d)",val,val2);
	} else
		snprintf(buf,100,"%d",val);
	elm_label_label_set(o, buf);
	eac->value = o;
	return o;
}
static Eina_Bool
attr_state_get(const void *eacv, Evas_Object *obj, const char *part){
	struct etac_attr *eac = (void *)eacv;

	ETAC_MAGIC_CHECK(eac, false);
//	printf("Part is %s\n",part);


	return true;
}

static void
attr_del(const void *eacv, Evas_Object *obj){
	struct etac_attr *eac = (void *)eacv;

	ETAC_MAGIC_CHECK(eac, (void)0);
	return;
}

/*
 * Group callbacks
 */
static char *
group_label_get(const void *grpv, Evas_Object *o, const char *part){
	const struct etac_attr_group *grp = grpv;

	return strdup(grp->group);
}
static Evas_Object *
group_icon_get(const void *grpv, Evas_Object *o, const char *part){
	return NULL;
}
static Eina_Bool
group_state_get(const void *grpv, Evas_Object *o, const char *part){
	return true;
}
static void
group_del(const void *grpv, Evas_Object *o){
	printf("Not implemented\n");
}




static Eina_Bool
etac_attr_append(const void *container, void *attrv, void *etacv){
	struct neb_attr *attr = attrv;
	struct etac_attr *eac;
	struct etac *etac = etacv;
	struct etac_attr_group *grp;
	Eina_List *l;
	const char *name;
	const char *group;

	group = neb_attr_prop_get(attr, "Group");
	name = neb_attr_name_get(attr);
	if (!group) return true;

	EINA_LIST_FOREACH(groups, l, grp){
		if (strcmp(grp->group, group) == 0)
			break;
	}
	if (l == NULL){
		grp = malloc(sizeof(struct etac_attr_group));
		grp->item = elm_genlist_item_append(etac->gl, &group_class, grp,
				NULL,0,NULL,0);
		grp->group = strdup(group);
		groups = eina_list_append(groups, grp);
	}

	eac = malloc(sizeof(struct etac_attr));
	eac->etac = etac;
	eac->magic = ETAC_ATTR_MAGIC;
	eac->attr = attr;
	elm_genlist_item_append(etac->gl, &attr_class, eac, grp->item,
			0, NULL, 0);

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


