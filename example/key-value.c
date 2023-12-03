#include "../header/toml-c.h"

char *doc = "\n"
	"host = 'example.com'\n"
	"port = 80\n";

int main() {
	char errbuf[200];
	toml_table_t *tbl = toml_parse(doc, errbuf, sizeof(errbuf));
	if (!tbl) {
		fprintf(stderr, "ERROR: %s\n", errbuf);
		exit(1);
	}

	int n = toml_table_len(tbl);
	for (int i = 0; i < n; i++) {
		const char *key = toml_table_key(tbl, i);

		toml_value_t v;
		v = toml_table_string(tbl, key);
		if (v.u.s != NULL) {
			printf("%d → %s = %s\n", i, key, v.u.s);
			free(v.u.s);
		}
		v = toml_table_int(tbl, key);
		if (v.u.i > 0)
			printf("%d → %s = %ld\n", i, key, v.u.i);
	}

	toml_free(tbl);
	return 0;
}
