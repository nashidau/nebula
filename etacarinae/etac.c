
#include <stdbool.h>

#include <Elementary.h>
#include <nebula.h>


#define DEBUG(...) printf(__VA_ARGS__)

struct etac {
	struct nebula *neb;
	struct neb_character *chr;
};

int
elm_main(int argc, char **argv){
	struct etac *etac;

	DEBUG("Welcome to eta caarinae\n");

	etac = calloc(1,sizeof(struct etac));
	if (!etac) return 1;

	etac->neb = nebula_init();

	if (!etac->neb){
		fprintf(stderr,"Unable to init nebula\n");
		return 1;
	}



	return 0;

}
ELM_MAIN()
