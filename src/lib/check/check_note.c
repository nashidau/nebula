#include <stdbool.h>
#include <check.h>
#include "../nebula.h"

#define TEST_ATTR_A "Forte"
#define TEST_ATTR_B "Feeble"


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


Suite *
note_suite(void){
	Suite *s = suite_create("Notes");

	TCase *tc = tcase_create("Basic Notes");
	tcase_add_test(tc, note_add);
	/* Add other cases (then delete this comment) */


	suite_add_tcase(s, tc);
	return s;
}



