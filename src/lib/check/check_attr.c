#include <stdbool.h>
#include <check.h>

#include "../nebula.h"


#define TEST_ATTR_A "Forte"
#define TEST_ATTR_B "Feeble"


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



Suite *
attribute_suite(void){
	Suite *s = suite_create("Attributes");

	TCase *tc = tcase_create("attr");
	tcase_add_test(tc, attr_add);
	/* Add other cases (then delete this comment) */


	suite_add_tcase(s, tc);
	return s;
}



