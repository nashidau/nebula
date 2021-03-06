#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "../lib/nebula.h"

static int basic_test(struct nebula *);
static int ch_add1(struct nebula *);
static int ch_add2(struct nebula *);
static int ch_add3(struct nebula *);
static int ch_name1(struct nebula *);
static int ch_name2(struct nebula *);
static int ch_name3(struct nebula *neb);
static int ch_name4(struct nebula *neb);
static int ch_name5(struct nebula *neb);

static int ch_attr1(struct nebula *neb);
static int ch_attr2(struct nebula *neb);
static int ch_attr3(struct nebula *neb);

static int ch_elem1(struct nebula *neb);
static int ch_elem2(struct nebula *neb);
static int ch_elem3(struct nebula *neb);

static int ch_ref1(struct nebula *neb);
static int ch_ref2(struct nebula *neb);
static int ch_ref3(struct nebula *neb);
static int ch_ref4(struct nebula *neb);

static int ch_save1(struct nebula *neb);
static int ch_save2(struct nebula *neb);



#define TEST_NAME_A "Tento"
#define TEST_ATTR_A "Forte"
#define TEST_ATTR_B "Feeble"

enum {
	TEST_SUCCESS,
	TEST_FAILED,
};

static const struct tests {
	const char *name;
	int (*test)(struct nebula *);
} tests[] = {
	{ "Basic",		basic_test },
	{ "Character Add 1",	ch_add1 },
	{ "Character Add 2",	ch_add2 },
	{ "Character Add 3",	ch_add3 },
	{ "Character Name Set",	ch_name1 },
	{ "Character Name Get",	ch_name2 },
	{ "Character Name Get by Name",	ch_name3 },
	{ "Character Name Correct",	ch_name4 },
	{ "Character Get same object",	ch_name5 },

	{ "Attribute Add",	ch_attr1 },
	{ "Attribute Get",	ch_attr2 },
	{ "Attribute Dup Add",	ch_attr3 },

	{ "Element Value Add",	ch_elem1 },
	{ "Element Value set",	ch_elem2 },
	{ "Element Value total",ch_elem3 },

	{ "Element Ref Add",	ch_ref1 },
	{ "Element Ref Add invalid",	ch_ref2 },
	{ "Element Ref get",	ch_ref3 },
	{ "Element ref total",	ch_ref4 },

	{ "Character save 1",	ch_save1 },
	{ "Character save 1",	ch_save2 },
};
#define N_TESTS (sizeof(tests)/sizeof(tests[0]))

int
main(int argc, char **argv){
	struct nebula *neb;
	int i,rv;
	int passed = 0;

	for (i = 0; i < N_TESTS ; i ++){
		neb = nebula_init();

		if (!neb){
			printf("Unable to init nebula");
			exit(1);
		}

		printf("Test: %s: ",tests[i].name);
		rv = tests[i].test(neb);
		if (rv){
			printf("Failed\n");
		} else {
			printf("Passed\n");
			passed ++;
		}

		nebula_free(neb);
	}

	printf("\nPassed %d/%d\n",passed, (int)N_TESTS);

	return passed == N_TESTS;
}

static int
basic_test(struct nebula *neb){
	return !neb;
}

/* Can add a character */
static int
ch_add1(struct nebula *neb){
	struct neb_character *ch;

	ch = neb_character_new(neb);

	return !ch;
}

/* Can add multiple characters ,and they are differnt */
static int
ch_add2(struct nebula *neb){
	struct neb_character *ch1, *ch2;

	ch1 = neb_character_new(neb);
	ch2 = neb_character_new(neb);

	if (!ch1 || !ch2 || ch1 == ch2) return TEST_FAILED;
	return TEST_SUCCESS;
}

/* Add many characters: Check fine */
static int
ch_add3(struct nebula *neb){
	struct neb_character *ch[10];
	int i;

	for (i = 0 ; i < 10 ; i ++){
		ch[i] = neb_character_new(neb);
		if (ch[i] == NULL) return TEST_FAILED;
	}

	/* FIXME: Check for unique */
	return TEST_SUCCESS;
}



static int
ch_name1(struct nebula *neb){
	struct neb_character *ch;

	ch = neb_character_new(neb);
	return neb_character_name_set(ch, TEST_NAME_A);
}
static int
ch_name2(struct nebula *neb){
	struct neb_character *ch;
	const char *name;

	ch = neb_character_new(neb);
	neb_character_name_set(ch, TEST_NAME_A);
	name = neb_character_name_get(ch);
	if (strcmp(name, TEST_NAME_A) != 0) return -1;
	return TEST_SUCCESS;
}

static int
ch_name3(struct nebula *neb){
	struct neb_character *ch, *ch2;

	ch = neb_character_new(neb);
	neb_character_name_set(ch, TEST_NAME_A);
	ch2 = neb_character_get(neb, TEST_NAME_A);
	if (!ch2) return TEST_FAILED;
	return TEST_SUCCESS;
}


static int
ch_name4(struct nebula *neb){
	struct neb_character *ch, *ch2;
	const char *name;

	ch = neb_character_new(neb);
	neb_character_name_set(ch, TEST_NAME_A);
	ch2 = neb_character_get(neb, TEST_NAME_A);
	name = neb_character_name_get(ch);
	if (strcmp(name, TEST_NAME_A) != 0) return -1;
	return TEST_SUCCESS;
}

static int
ch_name5(struct nebula *neb){
	struct neb_character *ch, *ch2;

	ch = neb_character_new(neb);
	neb_character_name_set(ch, TEST_NAME_A);
	ch2 = neb_character_get(neb, TEST_NAME_A);
	if (ch != ch) return -1;
	return TEST_SUCCESS;
}




static int
ch_attr1(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	if (!at) return -1;
	return TEST_SUCCESS;
}

static int
ch_attr2(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;

	ch = neb_character_new(neb);
	neb_character_attr_add(ch, TEST_ATTR_A);
	at = neb_character_attr_get(ch, TEST_ATTR_A);
	if (!at) return -1;
	return TEST_SUCCESS;
}
static int
ch_attr3(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;

	ch = neb_character_new(neb);
	neb_character_attr_add(ch, TEST_ATTR_A);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	if (at) return -1;
	return TEST_SUCCESS;
}

static int
ch_elem1(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	el = neb_attr_elem_value_add(at, 7);
	if (!el) return -1;

	return TEST_SUCCESS;
}

static int
ch_elem2(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	int val;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	neb_attr_elem_value_add(at, 7);

	val = neb_attr_value_get(at);
	if (val != 7) return -1;

	return TEST_SUCCESS;
}

static int
ch_elem3(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	int val;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	neb_attr_elem_value_add(at, 7);
	neb_attr_elem_value_add(at, 5);

	val = neb_attr_value_get(at);
	if (val != 12){
		printf("\n\tGot %d instead of %d\n",val,12);
		return -1;
	}

	return TEST_SUCCESS;
}



static int
ch_ref1(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	el = neb_attr_elem_reference_add(at, "Elvis", false);
	if (!el) return -1;

	return TEST_SUCCESS;
}
static int
ch_ref2(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	/* Shoudl fail */
	el = neb_attr_elem_reference_add(at, TEST_ATTR_B, true);
	if (el) return -1;

	return TEST_SUCCESS;
}

/**
 * Forte:	7
 * Feeble:	Forte
 */
static int
ch_ref3(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;
	int val;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	neb_attr_elem_value_add(at, 7);

	at = neb_character_attr_add(ch, TEST_ATTR_B);
	el = neb_attr_elem_reference_add(at, TEST_ATTR_A, true);

	if (!el) return -1;

	val = neb_attr_value_get(at);
	if (val != 7){
		printf("\n\tGot %d instead of %d\n",val,7);
		return -1;
	}

	return TEST_SUCCESS;
}

static int
ch_ref4(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;
	int val;

	ch = neb_character_new(neb);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	neb_attr_elem_value_add(at, 7);
	neb_attr_elem_value_add(at, 5);

	at = neb_character_attr_add(ch, TEST_ATTR_B);
	neb_attr_elem_value_add(at, 12);
	el = neb_attr_elem_reference_add(at, TEST_ATTR_A, true);

	if (!el) return -1;

	val = neb_attr_value_get(at);
	if (val != 24){
		printf("\n\tGot %d instead of %d\n",val,24);
		return -1;
	}

	return TEST_SUCCESS;
}

static int
ch_save1(struct nebula *neb){
	struct neb_character *ch;
	struct neb_attr *at;
	struct neb_elem *el;

	ch = neb_character_new(neb);
	neb_character_name_set(ch, TEST_NAME_A);
	at = neb_character_attr_add(ch, TEST_ATTR_A);
	neb_attr_elem_value_add(at, 7);
	neb_attr_elem_value_add(at, 5);

	at = neb_character_attr_add(ch, TEST_ATTR_B);
	neb_attr_elem_value_add(at, 12);
	el = neb_attr_elem_reference_add(at, TEST_ATTR_A, true);

	nebula_character_save(ch, "/tmp/neb.nch");

	return TEST_SUCCESS;
}

static int
ch_save2(struct nebula *neb){
	return TEST_SUCCESS;
}
