#include <stdbool.h>
#include <check.h>
#include "../nebula.h"

#define TEST_CH_NAME "Name"
#define TEST_FILE "acharacter"

START_TEST(character_add){
	struct nebula *neb;
	struct neb_character *ch;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");
}
END_TEST

START_TEST(character_name_set_get){
	struct nebula *neb;
	struct neb_character *ch;
	int result;
	const char *name;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	result = neb_character_name_set(ch, TEST_CH_NAME);
	fail_if(result != 0, "Unable to set character name");

	name = neb_character_name_get(ch);
	fail_if(strcmp(name, TEST_CH_NAME), "Name should be %s, got %s",
			TEST_CH_NAME, name);
}
END_TEST

START_TEST(character_find){
	struct nebula *neb;
	struct neb_character *ch;
	int result;
	struct neb_character *ch2;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	result = neb_character_name_set(ch, TEST_CH_NAME);
	fail_if(result != 0, "Unable to set character name");

	ch2 = neb_character_get(neb, TEST_CH_NAME);
	fail_if(ch2 == NULL, "Unable to find the character by its name");
}
END_TEST

START_TEST(character_find_in_large_neb){
	struct nebula *neb;
	struct neb_character *ch;
	int result, i;
	char name[10];
	struct neb_character *ch2;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	for (i = 0; i < 10001; i++) {
		ch = neb_character_new(neb);
		fail_if(ch == NULL, "Unable to create character");

		snprintf(name, 10, "%d", i);
		strcat(name, "name");

		result = neb_character_name_set(ch, name);
		fail_if(result != 0, "Unable to set character name");
	}

	ch2 = neb_character_get(neb, "10000name");
	fail_if(ch2 == NULL, "Unable to find the character in a large neb");
}
END_TEST

START_TEST(character_neb_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct nebula *neb2;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	neb2 = neb_character_neb_get(ch);
	fail_if(neb2 == NULL, "Unable to get nebula");
}
END_TEST

START_TEST(char_add){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_CH_NAME);
	fail_if(at == NULL, "Unable to add attribute");

	result = nebula_character_add(neb, ch);
	fail_if(result != 0, "Unable to add a character to be managed");
}
END_TEST

START_TEST(character_load){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_CH_NAME);
	fail_if(at == NULL, "Unable to add attribute");

	result = nebula_character_save(ch, TEST_FILE);
	fail_if(result != 0, "Unable to save character");

	ch = neb_character_load(neb, TEST_FILE);
	fail_if(ch == NULL, "Unable to load character");
}
END_TEST

START_TEST(character_load_not_existed){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_CH_NAME);
	fail_if(at == NULL, "Unable to add attribute");

	result = nebula_character_save(ch, TEST_FILE);
	fail_if(result != 0, "Unable to save character");

	ch = neb_character_load(neb, "not existed");
	fail_if(ch != NULL, "Load wrong character");
}
END_TEST

START_TEST(character_save){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	at = neb_character_attr_add(ch, TEST_CH_NAME);
	fail_if(at == NULL, "Unable to add attribute");

	result = nebula_character_save(ch, TEST_FILE);
	fail_if(result != 0, "Unable to save character");
}
END_TEST


Suite *
character_suite(void){
	Suite *s = suite_create("Character Management Suite");

	TCase *tc = tcase_create("Core");
	tcase_add_test(tc, character_add);
	tcase_add_test(tc, character_name_set_get);
	tcase_add_test(tc, character_find);
	tcase_add_test(tc, character_find_in_large_neb);
	tcase_add_test(tc, character_neb_get);
	tcase_add_test(tc, char_add);
	tcase_add_test(tc, character_load);
	tcase_add_test(tc, character_save);


	suite_add_tcase(s, tc);
	return s;
}



