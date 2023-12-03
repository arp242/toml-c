#ifndef TOML_H
#define TOML_H
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif
#ifdef __cplusplus
#define TOML_EXTERN extern "C"
#else
#define TOML_EXTERN extern
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Timestamp type. The year, month, day, hour, minute, second, z fields may be
// NULL if they are not relevant. e.g. In a local-date type, the hour, minute,
// second and z fields will be NULL.
struct toml_timestamp_t {
	struct { // internal; do not use.
		int year, month, day;
		int hour, minute, second, millisec;
		char z[10];
	} __buffer;

	int *year, *month, *day;
	int *hour, *minute, *second, *millisec;
	char *z;
};
typedef struct toml_timestamp_t toml_timestamp_t;

// TODO: move types here.
typedef struct toml_table_t toml_table_t;
typedef struct toml_array_t toml_array_t;

// TOML value.
struct toml_value_t {
	int ok;
	union {
		toml_timestamp_t *ts; // ts must be freed after use
		char             *s;  // string value; s must be freed after use
		int              sl;  // string length, excluding NULL.
		int              b;   // bool value
		int64_t          i;   // int value
		double           d;   // double value
	} u;
};
typedef struct toml_value_t toml_value_t;

// Parse a file; returns 0 on error, with the error message stored in errbuf.
TOML_EXTERN toml_table_t *toml_parse_file(FILE *fp, char *errbuf, int errbufsz);

// Parse a TOML document from a string; returns 0 on error, with the error
// message stored in errbuf.
TOML_EXTERN toml_table_t *toml_parse(char *toml, char *errbuf, int errbufsz);

// Free the table returned by toml_parse() or toml_parse_file(). This will
// invalidate all handles accessed through this tab.
TOML_EXTERN void toml_free(toml_table_t *tab);

// Array functions.
	TOML_EXTERN int toml_array_nelem(const toml_array_t *arr);

	TOML_EXTERN toml_value_t toml_string_at    (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_value_t toml_bool_at      (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_value_t toml_int_at       (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_value_t toml_double_at    (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_value_t toml_timestamp_at (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_array_t *toml_array_at    (const toml_array_t *arr, int idx);
	TOML_EXTERN toml_table_t *toml_table_at    (const toml_array_t *arr, int idx);

// Table functions
	TOML_EXTERN const char *toml_key_in(const toml_table_t *tab, int keyidx); // 0 if out of range.
	// Report if the key exists in the table.
	// TODO: why not toml_key_in(..) != 0?
	TOML_EXTERN bool toml_key_exists(const toml_table_t *tab, const char *key);

	TOML_EXTERN toml_value_t toml_string_in    (const toml_table_t *arr, const char *key);
	TOML_EXTERN toml_value_t toml_bool_in      (const toml_table_t *arr, const char *key);
	TOML_EXTERN toml_value_t toml_int_in       (const toml_table_t *arr, const char *key);
	TOML_EXTERN toml_value_t toml_double_in    (const toml_table_t *arr, const char *key);
	TOML_EXTERN toml_value_t toml_timestamp_in (const toml_table_t *arr, const char *key);
	TOML_EXTERN toml_array_t *toml_array_in    (const toml_table_t *tab, const char *key);
	TOML_EXTERN toml_table_t *toml_table_in    (const toml_table_t *tab, const char *key);


// Lesser used
// -----------------------------------------------------------------
TOML_EXTERN const char *toml_array_key(const toml_array_t *arr); // Array key.
TOML_EXTERN const char *toml_table_key(const toml_table_t *tab); // Table key.

// element kind: 'v'alue, 'a'rray, or 't'able, 'm'ixed
TOML_EXTERN char toml_array_kind(const toml_array_t *arr);

// Return the type of the values for array kind 'v'alue:
//   'i'nt, 'd'ouble, 'b'ool, 's'tring, 't'ime, 'D'ate, 'T'imestamp, 'm'ixed
TOML_EXTERN char toml_array_type(const toml_array_t *arr);





//--------------------------------------------------------------
// Deprecated

TOML_EXTERN int toml_table_nkval (const toml_table_t *tab); // Number of key-values in a table
TOML_EXTERN int toml_table_narr  (const toml_table_t *tab); // Number of arrays in a table
TOML_EXTERN int toml_table_ntab  (const toml_table_t *tab); // Number of sub-tables in a table

// A raw value, must be processed by toml_rto* before using.
typedef const char *toml_raw_t;
toml_raw_t toml_raw_in(const toml_table_t *tab, const char *key);
toml_raw_t toml_raw_at(const toml_array_t *arr, int idx);
int toml_rtos(toml_raw_t s, char **ret, int *len);
int toml_rtob(toml_raw_t s, int *ret);
int toml_rtoi(toml_raw_t s, int64_t *ret);
int toml_rtod(toml_raw_t s, double *ret);
int toml_rtod_ex(toml_raw_t s, double *ret, char *buf, int buflen);
int toml_rtots(toml_raw_t s, toml_timestamp_t *ret);

#endif
