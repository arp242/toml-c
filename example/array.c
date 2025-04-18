#include "../header/toml-c.h"

char *doc = "\n"
	"ints  = [1, 2, 3]\n"
	"mixed = [1, 'one', 1.2]\n"
	"\n"
	"[[aot]]\n"
	"k = 'one'\n"
	"[[aot]]\n"
	"k = 'two'\n";

int main(void) {
	char errbuf[200];
	toml_table_t *tbl = toml_parse(doc, errbuf, sizeof(errbuf));
	if (!tbl) {
		fprintf(stderr, "ERROR: %s\n", errbuf);
		exit(1);
	}

	// Array of ints.
	toml_array_t *arr = toml_table_array(tbl, "ints");
	int l = toml_array_len(arr);
	printf("ints:\n");
	for (int i = 0; i < l; i++)
		printf("  index %d = %ld\n", i, toml_array_int(arr, i).u.i);
	printf("\n");

	// Mixed array.
	arr = toml_table_array(tbl, "mixed");
	l = toml_array_len(arr);
	printf("mixed:\n");
	for (int i = 0; i < l; i++) {
		// TODO: like with table.c, this also would be tons easier if record and
		// return the type.
		toml_value_t v = toml_array_int(arr, i);
		if (v.ok) {
			printf("  index %d = (int)%ld\n", i, v.u.i);
			continue;
		}
		v = toml_array_double(arr, i);
		if (v.ok) {
			printf("  index %d = (float)%0.17g\n", i, v.u.d);
			continue;
		}
		v = toml_array_string(arr, i);
		if (v.ok) {
			printf("  index %d = (string)\"%s\"\n", i, v.u.s);
			continue;
		}
	}
	printf("\n");

	// Array-of-tables works just like inline tables.
	arr = toml_table_array(tbl, "aot");
	l = toml_array_len(arr);
	for (int i = 0; i < l; i++) {
		toml_table_t *t = toml_array_table(arr, i);
		toml_value_t v = toml_table_string(t, "k");
		printf("aot[%d].k = \"%s\"\n", i, v.u.s);
	}
}
