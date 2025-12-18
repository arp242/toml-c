#include <stdlib.h>
#include <string.h>

#include "toml.h"

int failed = 0;

#define errorf(...)                                                             \
	{                                                                           \
		failed = 1;                                                             \
		fprintf(stderr, "FAIL: %s:%d: %s()\n\t", __FILE__, __LINE__, __func__); \
		fprintf(stderr, __VA_ARGS__);                                           \
		fprintf(stderr, "\n");                                                  \
	}

#define fatalf(...)          \
	{                        \
		errorf(__VA_ARGS__); \
		return;              \
	}

#define streq(a, b) (strcmp(a, b) == 0)

void test_toml_table_value(void) {
	char          errbuf[200];
	toml_table_t* tbl = toml_parse("str   = 'xxx'\n"
	                               "int   = 42\n"
	                               "bool  = true\n"
	                               "float = 6.666\n"
	                               "ts    = 2012-01-02T15:16:17Z\n",
	                               errbuf, sizeof(errbuf));
	if (!tbl)
		fatalf("%s", errbuf);
	int l = toml_table_len(tbl);
	if (l != 5)
		errorf("wrong table length: %d", l);

	toml_value_t str = toml_table_string(tbl, "str");
	if (!str.ok)
		errorf("str.ok not set");
	if (!streq(str.u.s, "xxx"))
		errorf("str.u.s wrong value: '%s'; want: 'xxx'", str.u.s);
	if (str.u.sl != 3)
		errorf("str.u.sl wrong value: %d", str.u.sl);
	free(str.u.s);

	toml_value_t i = toml_table_int(tbl, "int");
	if (!i.ok)
		errorf("int.ok not set");
	if (i.u.i != 42)
		errorf("int.u.u wrong value: %ld", i.u.i);

	toml_value_t b = toml_table_bool(tbl, "bool");
	if (!b.ok)
		errorf("b.ok not set");
	if (!b.u.b)
		errorf("int.u.b wrong value: %d", b.u.b);

	toml_value_t f = toml_table_double(tbl, "float");
	if (!f.ok)
		errorf("f.ok not set");
	if (f.u.d != 6.666)
		errorf("int.u.u wrong value: %f", f.u.d);

	toml_value_t ts = toml_table_timestamp(tbl, "ts");
	if (!ts.ok)
		errorf("ts.ok not set");
	char have[200];

	// clang-format off
	snprintf(have, 200, "'%c' %d-%02d-%02d %02d:%02d:%02d.%d TZ=%d",
		ts.u.ts.kind, ts.u.ts.year, ts.u.ts.month, ts.u.ts.day, ts.u.ts.hour,
		ts.u.ts.minute, ts.u.ts.second, ts.u.ts.millisec, ts.u.ts.tz);
	// clang-format on
	char want[200] = "'d' 2012-01-02 15:16:17.0 TZ=0";
	if (!streq(have, want))
		errorf("have: %s\n\twant: %s", have, want);

	toml_free(tbl);
}

void test_toml_array_value(void) {
	char          errbuf[200];
	toml_table_t* tbl = toml_parse("str   = ['xxx', \"yyy\"]\n"
	                               "int   = [42, 43]\n"
	                               "bool  = [true, false]\n"
	                               "float = [6.666, 6.667]\n"
	                               "ts    = [2012-01-02T15:16:17Z, 2013-02-03T16:17:18Z]\n",
	                               errbuf, sizeof(errbuf));
	if (!tbl)
		fatalf("%s", errbuf);

	{
		toml_array_t* arr  = toml_table_array(tbl, "str");
		toml_value_t  str1 = toml_array_string(arr, 0);
		if (!str1.ok)
			errorf("str1.ok not set");
		if (!streq(str1.u.s, "xxx"))
			errorf("str1.u.s wrong value: '%s'; want: 'xxx'", str1.u.s);
		if (str1.u.sl != 3)
			errorf("str1.u.sl wrong value: %d", str1.u.sl);

		toml_value_t str2 = toml_array_string(arr, 1);
		if (!str2.ok)
			errorf("str2.ok not set");
		if (!streq(str2.u.s, "yyy"))
			errorf("str2.u.s wrong value: %s; want: 'yyy'", str2.u.s);
		if (str2.u.sl != 3)
			errorf("str2.u.sl wrong value: %d", str2.u.sl);
		free(str1.u.s);
		free(str2.u.s);
	}

	{
		toml_array_t* arr = toml_table_array(tbl, "ts");
		toml_value_t  ts1 = toml_array_timestamp(arr, 0);
		char          have[200];

		// clang-format off
		snprintf(have, 200, "'%c' %d-%02d-%02d %02d:%02d:%02d.%d TZ=%d",
			ts1.u.ts.kind, ts1.u.ts.year, ts1.u.ts.month, ts1.u.ts.day, ts1.u.ts.hour,
			ts1.u.ts.minute, ts1.u.ts.second, ts1.u.ts.millisec, ts1.u.ts.tz);
		// clang-format on
		char want[200] = "'d' 2012-01-02 15:16:17.0 TZ=0";
		if (!streq(have, want))
			errorf("have: %s\n\twant: %s", have, want);
	}

	toml_free(tbl);
}

void test_toml_table_string_unknown_value(void) {
	char          errbuf[200];
	toml_table_t* tbl = toml_parse("a = 'a'", errbuf, sizeof(errbuf));
	if (!tbl)
		fatalf("%s", errbuf);

	toml_value_t unknown = toml_table_string(tbl, "aa");
	if (unknown.ok)
		errorf("unknown.ok set");

	toml_free(tbl);
	free(unknown.u.s);
}

// TODO: can probably use toml-test's -errors feature for this.
void test_error(void) {
	char errbuf[200];

	// e_syntax errors
	toml_parse("key", errbuf, sizeof(errbuf));
	if (!streq(errbuf, "at 1:4: missing '='"))
		errorf("wrong error: %s", errbuf);

	toml_parse("k = 'missing-q\nb = 1", errbuf, sizeof(errbuf));
	if (!streq(errbuf, "at 1:14: unterminated quote (')"))
		errorf("wrong error: %s", errbuf);

	toml_parse("k = {{}}", errbuf, sizeof(errbuf));
	if (!streq(errbuf, "at 1:6: expected a string"))
		errorf("wrong error: %s", errbuf);

	// e_keyexists errors
	toml_parse("[a]\n[a]", errbuf, sizeof(errbuf));
	if (!streq(errbuf, "at 2:2: key already defined"))
		errorf("wrong error: %s", errbuf);
}

int main(void) {
	test_toml_table_value();
	test_toml_array_value();
	test_toml_table_string_unknown_value();
	test_error();

	printf("%s: %s\n", __FILE__, failed ? "FAIL" : "PASS");
	return failed;
}
