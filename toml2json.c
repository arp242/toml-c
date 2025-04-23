#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "toml.h"

typedef const char* toml_unparsed_t;
toml_unparsed_t     toml_table_unparsed(const toml_table_t* table, const char* key);
toml_unparsed_t     toml_array_unparsed(const toml_array_t* array, int idx);
int                 toml_value_string(toml_unparsed_t s, char** ret, int* len);
int                 toml_value_bool(toml_unparsed_t s, bool* ret);
int                 toml_value_int(toml_unparsed_t s, int64_t* ret);
int                 toml_value_double(toml_unparsed_t s, double* ret);
int                 toml_value_timestamp(toml_unparsed_t s, toml_timestamp_t* ret);

static void print_escape_string(const char* s, int sl) {
	for (int i = 0; i < sl; i++) {
		char ch = s[i];
		switch (ch) {
		case '\b': printf("\\b"); break;
		case '\t': printf("\\t"); break;
		case '\n': printf("\\n"); break;
		case '\f': printf("\\f"); break;
		case '\r': printf("\\r"); break;
		case '"':  printf("\\\""); break;
		case '\\': printf("\\\\"); break;
		default:
			if (ch >= 0x00 && ch <= 0x1f)
				printf("\\u00%02X", ch);
			else
				printf("%c", ch);
			break;
		}
	}
}

static void print_raw(const char* s) {
	char*            sval;
	int              slen;
	int64_t          ival;
	bool             bval;
	double           dval;
	toml_timestamp_t ts;

	if (toml_value_string(s, &sval, &slen) == 0) {
		printf("{\"type\": \"string\",\"value\": \"");
		print_escape_string(sval, slen);
		printf("\"}");
		free(sval);
	} else if (toml_value_int(s, &ival) == 0) {
		printf("{\"type\": \"integer\",\"value\": \"%" PRId64 "\"}", ival);
	} else if (toml_value_bool(s, &bval) == 0) {
		printf("{\"type\": \"bool\",\"value\": \"%s\"}", bval ? "true" : "false");
	} else if (toml_value_double(s, &dval) == 0) {
		if (isnan(dval))
			printf("{\"type\": \"float\",\"value\": \"nan\"}");
		else
			printf("{\"type\": \"float\",\"value\": \"%0.17g\"}", dval);
	} else if (toml_value_timestamp(s, &ts) == 0) {
		char millisec[10];
		if (ts.millisec)
			snprintf(millisec, 10, ".%03d", ts.millisec);
		else
			millisec[0] = 0;
		if (ts.kind == 'd' || ts.kind == 'l') {
			char off[15];
			off[0] = 'Z';
			off[1] = 0;
			if (ts.tz != 0)
				snprintf(off, 15, "%c%02d:%02d", (ts.tz > 0 ? '+' : '-'), abs(ts.tz) / 60, abs(ts.tz) % 60);
			// clang-format off
			printf("{\"type\": \"%s\",\"value\": \"%04d-%02d-%02dT%02d:%02d:%02d%s%s\"}",
				(ts.kind == 'd' ? "datetime" : "datetime-local"),
				ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, millisec,
				(ts.kind == 'd' ? off : ""));
		} else if (ts.kind == 'D') {
			printf("{\"type\": \"date-local\",\"value\": \"%04d-%02d-%02d\"}",
				ts.year, ts.month, ts.day);
		} else if (ts.kind == 't') {
			printf("{\"type\": \"time-local\",\"value\": \"%02d:%02d:%02d%s\"}",
				ts.hour, ts.minute, ts.second, millisec);
			// clang-format on
		}
	} else {
		fprintf(stderr, "unknown type\n");
		exit(1);
	}
}

static void print_array(toml_array_t* arr);
static void print_table(toml_table_t* curtbl) {
	const char*   key;
	int           keylen;
	const char*   raw;
	toml_array_t* arr;
	toml_table_t* tbl;

	printf("{");
	for (int i = 0; (key = toml_table_key(curtbl, i, &keylen)) != 0; i++) {
		printf("%s\"", i > 0 ? ",\n" : "");
		print_escape_string(key, keylen);
		printf("\":");

		if ((raw = toml_table_unparsed(curtbl, key)) != 0)
			print_raw(raw);
		else if ((arr = toml_table_array(curtbl, key)) != 0)
			print_array(arr);
		else if ((tbl = toml_table_table(curtbl, key)) != 0)
			print_table(tbl);
		else
			abort();
	}
	printf("}");
}

static void print_table_array(toml_array_t* curarr) {
	toml_table_t* tbl;

	printf("[");
	for (int i = 0; (tbl = toml_array_table(curarr, i)) != 0; i++) {
		printf("%s", i > 0 ? "," : "");
		print_table(tbl);
	}
	printf("]");
}

static void print_array(toml_array_t* curarr) {
	if (curarr->kind == 't') {
		print_table_array(curarr);
		return;
	}

	printf("[");

	const char*   raw;
	toml_array_t* arr;
	toml_table_t* tbl;

	const int n = toml_array_len(curarr);
	for (int i = 0; i < n; i++) {
		printf("%s", i > 0 ? "," : "");

		if ((arr = toml_array_array(curarr, i)) != 0) {
			print_array(arr);
			continue;
		}

		if ((tbl = toml_array_table(curarr, i)) != 0) {
			print_table(tbl);
			continue;
		}

		raw = toml_array_unparsed(curarr, i);
		if (raw) {
			print_raw(raw);
			continue;
		}

		fflush(stdout);
		fprintf(stderr, "ERROR: unable to decode value in array\n");
		exit(1);
	}

	printf("]");
}

static void cat(FILE* fp) {
	char errbuf[200];

	toml_table_t* tbl = toml_parse_file(fp, errbuf, sizeof(errbuf));
	if (!tbl) {
		fprintf(stderr, "ERROR: %s\n", errbuf);
		exit(1);
	}

	print_table(tbl);
	printf("\n");

	toml_free(tbl);
}

int main(int argc, const char* argv[argc + 1]) {
	if (argc == 1) {
		cat(stdin);
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		FILE* fp = fopen(argv[i], "r");
		if (!fp) {
			fprintf(stderr, "ERROR: cannot open %s: %s\n", argv[i], strerror(errno));
			exit(1);
		}
		cat(fp);
		fclose(fp);
	}
	return 0;
}
