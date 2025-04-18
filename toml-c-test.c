#include <stdlib.h>
#include <string.h>
#include "toml.h"

int main() {
	char errbuf[200];
	int fail = 0;

	{ // Unknown value
		toml_table_t *tbl = toml_parse("", errbuf, sizeof(errbuf));
		if (!tbl) {
			fprintf(stderr, "ERROR: %s\n", errbuf);
			return 1;
		}

		toml_value_t unknown = toml_table_string(tbl, "unknown");
		if (unknown.ok) {
			fprintf(stderr, "toml-c-test: unknown.ok set\n");
			fail = 1;
		}
		toml_free(tbl);
		free(unknown.u.s);
	}

	{ // Make sure string length is good.
		toml_table_t *tbl = toml_parse("str = 'xxx'", errbuf, sizeof(errbuf));
		if (!tbl) {
			fprintf(stderr, "ERROR: %s\n", errbuf);
			return 1;
		}

		toml_value_t str = toml_table_string(tbl, "str");
		if (!str.ok) {
			fprintf(stderr, "toml-c-test: str.ok not set\n");
			fail = 1;
		}
		if (!strcmp(str.u.s, "hello world")) {
			fprintf(stderr, "toml-c-test: str.u.s wrong value: %s\n", str.u.s);
			fail = 1;
		}
		if (str.u.sl != 3) {
			fprintf(stderr, "toml-c-test: str.u.sl wrong value: %d\n", str.u.sl);
			fail = 1;
		}
		toml_free(tbl);
		free(str.u.s);
	}

	{ // e_syntax errors
		toml_parse("key", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 1:4: missing '='", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}
	{
		toml_parse("k = 'missing-q\nb = 1", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 1:14: unterminated quote (')", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}
	{
		toml_parse("k = {\n}", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 1:6: newline not allowed in inline table", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}
	{
		toml_parse("k = {{}}", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 1:6: expected a string", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}
	{
		toml_parse("k = {b = 1\n}", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 1:11: newline not allowed in inline table", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}

	{ // e_keyexists errors
		toml_parse("[a]\n[a]", errbuf, sizeof(errbuf));
		if (strncmp(errbuf, "at 2:2: key already defined", 200)) {
			fprintf(stderr, "toml-c-test: wrong error: %s\n", errbuf);
			fail = 1;
		}
	}

	if (!fail)
		printf("toml-c-test: okay\n");
	return fail;
}
