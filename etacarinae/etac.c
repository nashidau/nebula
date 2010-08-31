
#include <stdbool.h>

#include <Elementary.h>
#include <nebula.h>


#define DEBUG(...) printf(__VA_ARGS__)

struct etac {
	struct nebula *neb;
	struct neb_character *chr;
};

static void usage(const char *progname);
int window_create(struct etac *etac);

int
elm_main(int argc, char **argv){
	struct etac *etac;
	const char *file;

	DEBUG("Welcome to eta caarinae\n");

	if (!argv[1]) usage(argv[0]);

	file = argv[1];

	etac = calloc(1,sizeof(struct etac));
	if (!etac) return 1;

	etac->neb = nebula_init();

	if (!etac->neb){
		fprintf(stderr,"Unable to init nebula\n");
		return 1;
	}

	window_create(etac);

	elm_run();
	elm_shutdown();

	return 0;

}
ELM_MAIN()

static void
win_main_del(void *data, Evas_Object *obj, void *event_info) {
        elm_exit();
}

int
window_create(struct etac *etac){
	Evas_Object *win, *bg, *bx, *gl;

	win = elm_win_add(NULL,"Eta Carinae", ELM_WIN_BASIC);
	elm_win_title_set(win, "Eta Carinae");
	evas_object_smart_callback_add(win, "delete,request", win_main_del, NULL);

	bg = elm_bg_add(win);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
        elm_win_resize_object_add(win, bg);
        evas_object_show(bg);

        bx = elm_box_add(win);
        evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
        elm_win_resize_object_add(win, bx);
        evas_object_show(bx);

	gl = elm_genlist_add(win);
        elm_genlist_always_select_mode_set(gl, false);
        evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
	evas_object_show(gl);
	elm_box_pack_start(bx, gl);


	evas_object_size_hint_min_set(bg, 480, 640);
	evas_object_show(win);

	return 0;
}

static void
usage(const char *progname){
	printf("Eta Carinae Character Vieweer for Nebula\n");
	printf("Usage: %s <characterfile>\n", progname);
	exit(1);
}
