#include <stdbool.h>
#include <check.h>
#include "../nebula.h"

START_TEST(character_add){
	struct nebula *neb;
	struct neb_character *ch;

	neb = nebula_init();
	fail_if(neb == NULL, "Nebula was NULL");

	ch = neb_character_new(neb);
	fail_if(ch == NULL, "Unable to create character");
}
END_TEST



Suite *
character_suite(void){
	Suite *s = suite_create("Character Management Suite");

	TCase *tc = tcase_create("Core");
	tcase_add_test(tc, character_add);
	/* Add other cases (then delete this comment) */


	suite_add_tcase(s, tc);
	return s;
}



