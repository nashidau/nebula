#include <stdbool.h>
#include <check.h>
#include "../nebula.h"


START_TEST(template_apply){
	struct nebula *neb;
	struct neb_character *ch;
	struct neb_attr *at;
	int rv;

	neb = nebula_init();

	ch = neb_character_new(neb);

	rv = neb_character_template_apply(ch, "check/data/nwod.ntemplate");
	fail_if(rv, "Neb character template apply barfed");

	at = neb_character_attr_get(ch, "Strength");
	fail_if(at == NULL, "COuld not get attribute");

}
END_TEST


Suite *
template_suite(void){
	Suite *s = suite_create("Template Suite");

        TCase *tc = tcase_create("Basic");
        tcase_add_test(tc, template_apply);

        suite_add_tcase(s, tc);
        return s;
}
