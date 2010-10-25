

#include <check.h>

Suite *character_suite(void);
Suite *attribute_suite(void);
Suite *note_suite(void);

static Suite *(*suites[])(void) = {
        character_suite,
        attribute_suite,
        note_suite,
};
#define N_SUITES ((int)(sizeof(suites)/sizeof(suites[0])))



int
main(int argc, char **argv){
        int nfailed;
        Suite *s;
        SRunner *sr;
        int i;

        nfailed = 0;
        for (i = 0 ; i < N_SUITES ; i ++){
                s = suites[i]();
                sr = srunner_create(s);
                srunner_run_all(sr, CK_NORMAL);
                nfailed += srunner_ntests_failed(sr);
                srunner_free(sr);
        }

        return !!nfailed;
}

