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

START_TEST(ch_note_del){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	result = neb_ch_note_del(ch, note);
	fail_if(result != 0, "Unable to delete the notes list");
}
END_TEST

START_TEST(note_find){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int i;
	char str[15];

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	for (i = 0; i < 11000; i++) {
		snprintf(str, 15, "%d", i);
		strcat(str, "Title");
		neb_character_note_add(ch, str, TEST_BODY_A);
	}

	note = neb_character_note_find(ch, "10000Title");
	fail_if(note == NULL, "Unable to find the exite note");
}
END_TEST

START_TEST(note_tag_add){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	result = neb_note_tag_add(note, "tag");
	fail_if(result != 0, "Unable to add tag");
}
END_TEST

START_TEST(note_tag_remove){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	neb_note_tag_add(note, "tag");

	result = neb_note_tag_remove(note, "tag");
	fail_if(result != 0, "Unable to remove tag");
}
END_TEST

START_TEST(note_tag_remove_not_exited){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	neb_note_tag_add(note, "tag");

	result = neb_note_tag_remove(note, "tag2");
	fail_if(result == 0, "Remove wrong tag");
}
END_TEST

START_TEST(note_tag_remove_in_large_list){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int i, result;
	char str[10];

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	for (i = 0; i < 10001; i++) {
		snprintf(str, 10, "%d", i);
		strcat(str, "tag");
		neb_note_tag_add(note, str);
	}

	result = neb_note_tag_remove(note, "10000tag");
	fail_if(result != 0, "Unable to remove tag in the large list");
}
END_TEST

START_TEST(note_tagged_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	Eina_List *list;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	neb_note_tag_add(note, "tag");

	list = neb_charcter_note_tagged_get(ch, "tag");
	fail_if(list == NULL, "Unable to get notes by tag");
}
END_TEST

START_TEST(note_text_append){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	result = neb_note_text_append(note, "text");
	fail_if(result == -1, "Unable to append text to the note");
}
END_TEST

START_TEST(note_iter_create){
	struct nebula *neb;
	struct neb_character *ch;
	Eina_Iterator *iter;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	iter = neb_character_note_iter_new(ch);
	fail_if(iter == NULL, "Unable to create the iterator");
}
END_TEST

START_TEST(note_title_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	char *title;

	neb = nebula_init();
	ch = neb_character_new(neb);

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	title = neb_note_title_get(note);
	fail_if(strcmp(title, TEST_TITLE_A), "Note title should be '%s', got %s",
			TEST_TITLE_A, title);
}
END_TEST

START_TEST(note_body_get){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	char *body;

	neb = nebula_init();
	ch = neb_character_new(neb);

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	body = neb_note_body_get(note);
	fail_if(strcmp(body, TEST_BODY_A), "Note title should be '%s', got %s",
			TEST_BODY_A, body);
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

START_TEST(note_del){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_note *note;
	int result;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");

	note = neb_character_note_add(ch, TEST_TITLE_A, TEST_BODY_A);
	fail_if(note == NULL, "Unable to add note");

	result = neb_character_note_del(ch, TEST_TITLE_A);
	fail_if(result != 0, "Unable to emove note");
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

	tcase_add_test(tc, ch_note_del);
	tcase_add_test(tc, note_find);
	tcase_add_test(tc, note_tag_add);
	tcase_add_test(tc, note_tag_remove);
	tcase_add_test(tc, note_tag_remove_in_large_list);
	tcase_add_test(tc, note_tagged_get);
	tcase_add_test(tc, note_text_append);
	tcase_add_test(tc, note_iter_create);
	tcase_add_test(tc, note_title_get);
	tcase_add_test(tc, note_del);


	suite_add_tcase(s, tc);
	return s;
}



