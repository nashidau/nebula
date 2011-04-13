#include <stdbool.h>
#include <check.h>
#include "../nebula.h"

#define TEST_TITLE_A	"Note"
#define TEST_BODY_A	"Hello"
#define TEST_BODY_B	"World"


START_TEST(note_add){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, "Title", "Body");
	fail_if(note == NULL, "Unable to add note");
}
END_TEST

START_TEST(note_add2){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, "Title", "Body");
	fail_if(note == NULL, "Unable to add note");

	fail_if(strcmp("Title", neb_note_title_get(note)), "Titles differed");
	fail_if(strcmp("Body", neb_note_body_get(note)), "Body differed");
}
END_TEST

START_TEST(note_len){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int len;

	neb = nebula_init();
	ch = neb_character_new(neb);

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	len = neb_note_length_get(note);
	fail_if(len != 5, "Note length should have been '5', got %d",len);
}
END_TEST

START_TEST(note_len_change){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int len;
	const char *str;

	neb = nebula_init();
	ch = neb_character_new(neb);

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	len = neb_note_length_get(note);
	fail_if(len != 5, "Note length should have been '5', got %d",len);

	neb_note_text_append(note, TEST_BODY_B);

	len = neb_note_length_get(note);
	fail_if(len != 10, "Note length should have been '10', got %d",len);

	str = neb_note_body_get(note);
	fail_if(strcmp(str, TEST_BODY_A TEST_BODY_B), "Strings differed: "
			"'%s' vs '" TEST_BODY_A TEST_BODY_B "' (expected)",str);
}
END_TEST

Suite *
note_suite(void){
	Suite *s = suite_create("Notes");

	TCase *tc = tcase_create("Basic Notes");
	tcase_add_test(tc, note_add);
	tcase_add_test(tc, note_add2);
	tcase_add_test(tc, note_len);
	tcase_add_test(tc, note_len_change);


	suite_add_tcase(s, tc);
	return s;
}



