#include <stdbool.h>
#include <check.h>

#include "../nebula.h"


#define TEST_ATTR_A "Forte"
#define TEST_ATTR_B "Feeble"
#define TEST_PROP_A "Property"
#define TEST_PROP_B "Prop"


START_TEST(attr_add){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");
}
END_TEST

START_TEST(attr_add_twice){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at != NULL, "Add the same attribute twice");
}
END_TEST

START_TEST(attr_del){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_ch_attr_del(ch, at);
	fail_if(result == -1, "Unable to delete attribute by name");
}
END_TEST

START_TEST(attr_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	at = neb_character_attr_get(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to get attribute");
}
END_TEST

START_TEST(attr_get_not_existed){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	at = neb_character_attr_get(ch, TEST_ATTR_B);
	fail_if(at != NULL, "Get wrong attribute");
}
END_TEST

START_TEST(attr_value_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_value_get(at);
	fail_if(result == -1, "Unable to get attribute value");
}
END_TEST

START_TEST(attr_lock){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_lock(at);
	fail_if(result == -1, "Unable to lock attribute");

	result = neb_attr_lock(at);
	fail_if(result == -1, "Unable to lock attribute");
}
END_TEST

START_TEST(attr_unlock){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_unlock(at);
	fail_if(result != -1, "Unlock attribute when lock is 0");

	result = neb_attr_lock(at);
	fail_if(result == -1, "Unable to lock attribute");

	result = neb_attr_unlock(at);
	fail_if(result == -1, "Unable to unlock attribute");
}
END_TEST

START_TEST(attr_lock_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_lock(at);
	fail_if(result == -1, "Unable to lock attribute");

	result = neb_attr_lock_get(at);
	fail_if(result != 1, "Attribute lock should be 1, got %d", result);
}
END_TEST

START_TEST(attr_name_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	const char *name;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	name = neb_attr_name_get(at);
	fail_if(name == NULL, "Attribute lock should be"
			TEST_ATTR_A ", got %s", name);
}
END_TEST

START_TEST(attr_prop_add){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");
}
END_TEST

START_TEST(attr_prop_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;
	const char *prop;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");

	prop = neb_attr_prop_get(at, TEST_PROP_A);
	fail_if(strcmp(TEST_PROP_B, prop), "Property should be" TEST_PROP_B
			", but got %s", prop);
}
END_TEST

START_TEST(attr_prop_list_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;
	Eina_List *list;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");

	list = neb_attr_prop_list_get(at);
	fail_if(list == NULL, "Unable to get property names");
}
END_TEST

START_TEST(attr_prop_list_free){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;
	Eina_List *list;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");

	list = neb_attr_prop_list_get(at);
	fail_if(list == NULL, "Unable to get property names");

	result = neb_attr_prop_list_free(list);
	fail_if(result != 0, "Unable to free property names list");
}
END_TEST

START_TEST(attr_filter_set){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_filter_set(at, TEST_ATTR_B);
	fail_if(result != 0, "Unable to set filter");
}
END_TEST

START_TEST(attr_filter_set_twice){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_filter_set(at, TEST_ATTR_B);
	fail_if(result != 0, "Unable to set filter");

	result = neb_attr_filter_set(at, TEST_ATTR_A);
	fail_if(result != 0, "Unable to set filter");
}
END_TEST

START_TEST(attr_prop_iter_new){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;
	Eina_Iterator *iter;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");

	iter = neb_attr_prop_iter_new(at);
	fail_if(iter == NULL, "Unable to create iterator to property");
}
END_TEST

START_TEST(attr_elem_iter_new){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;
	Eina_Iterator *iter;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_ATTR_A);
	fail_if(at == NULL, "Unable to add attribute");

	result = neb_attr_prop_add(at, TEST_PROP_A, TEST_PROP_B);
	fail_if(result != 0, "Unable to add property");

	iter = neb_attr_elem_iter_new(at);
	fail_if(iter == NULL, "Unable to create iterator to element");
}
END_TEST


Suite *
attribute_suite(void){
	Suite *s = suite_create("Attributes");

	TCase *tc = tcase_create("attr");
	tcase_add_test(tc, attr_add);
	tcase_add_test(tc, attr_add_twice);
	tcase_add_test(tc, attr_del);
	tcase_add_test(tc, attr_get);
	tcase_add_test(tc, attr_get_not_existed);
	tcase_add_test(tc, attr_value_get);
	tcase_add_test(tc, attr_lock);
	tcase_add_test(tc, attr_unlock);
	tcase_add_test(tc, attr_lock_get);
	tcase_add_test(tc, attr_prop_add);
	tcase_add_test(tc, attr_prop_get);
	tcase_add_test(tc, attr_prop_list_get);
	tcase_add_test(tc, attr_prop_list_free);
	tcase_add_test(tc, attr_filter_set);
	tcase_add_test(tc, attr_filter_set_twice);
	tcase_add_test(tc, attr_prop_iter_new);
	tcase_add_test(tc, attr_elem_iter_new);


	suite_add_tcase(s, tc);
	return s;
}



