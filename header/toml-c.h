#ifndef TOML_H
#define TOML_H
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
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

typedef struct toml_table_t     toml_table_t;
typedef struct toml_array_t     toml_array_t;
typedef struct toml_value_t     toml_value_t;
typedef struct toml_timestamp_t toml_timestamp_t;
typedef struct toml_keyval_t    toml_keyval_t;
typedef struct toml_arritem_t   toml_arritem_t;
typedef struct toml_pos_t       toml_pos_t;

// TOML table.
struct toml_table_t {
	const char *key;       // Key for this table
	int keylen;            // length of key.
	bool implicit;         // Table was created implicitly
	bool readonly;         // No more modification allowed

	int nkval;             // key-values in the table
	toml_keyval_t **kval;
	int narr;              // arrays in the table
	toml_array_t **arr;
	int ntbl;              // tables in the table
	toml_table_t **tbl;
};

// TOML array.
struct toml_array_t {
	const char *key; // key to this array
	int keylen;      // length of key.
	int kind;        // element kind: 'v'alue, 'a'rray, or 't'able, 'm'ixed
	int type;        // for value kind: 'i'nt, 'd'ouble, 'b'ool, 's'tring, 't'ime, 'D'ate, 'T'imestamp, 'm'ixed
	int nitem;       // number of elements
	toml_arritem_t *item;
};
struct toml_arritem_t {
	int valtype; // for value kind: 'i'nt, 'd'ouble, 'b'ool, 's'tring, 't'ime, 'D'ate, 'T'imestamp
	char *val;
	toml_array_t *arr;
	toml_table_t *tbl;
};

// TOML key/value pair.
struct toml_keyval_t {
	const char *key;    // key to this value
	int         keylen; // length of key.
	const char *val;    // the raw value
};

// Token position.
struct toml_pos_t {
	int line;
	int col;
};

// Timestamp type; some values may be empty depending on the value of kind.
struct toml_timestamp_t {
	// datetime type:
	//
	//   'd'atetime          Full date + time + TZ
	//   'l'local-datetime   Full date + time but without TZ
	//   'D'ate-local        Date only, without TZ
	//   't'ime-local        Time only, without TZ
	char kind;

	int year, month, day;
	int hour, minute, second, millisec;
	int tz; // Timezone offset in minutes
};

// Parsed TOML value.
//
// The string value s is a regular NULL-terminated C string, but the string
// length is also given in sl since TOML values may contain NULL bytes. The
// value is guaranteed to be correct UTF-8.
struct toml_value_t {
	bool ok; // Was this value present?
	union {
		toml_timestamp_t ts; // datetime; must be freed after use.
		struct {
			char         *s;  // string value; must be freed after use
			int          sl;  // string length, excluding NULL.
		};
		bool             b;   // bool value
		int64_t          i;   // int value
		double           d;   // double value
	} u;
};


// toml_parse() parses a TOML document from a string. Returns 0 on error, with
// the error message stored in errbuf.
//
// toml_parse_file() is identical, but reads from a file descriptor.
//
// Use toml_free() to free the return value; this will invalidate all handles
// for this table.
	TOML_EXTERN toml_table_t *toml_parse      (char *toml, char *errbuf, int errbufsz);
	TOML_EXTERN toml_table_t *toml_parse_file (FILE *fp, char *errbuf, int errbufsz);
	TOML_EXTERN void          toml_free       (toml_table_t *table);

// Table functions.
//
// toml_table_len() gets the number of direct keys for this table;
// toml_table_key() gets the nth direct key in this table.
	TOML_EXTERN int           toml_table_len       (const toml_table_t *table);
	TOML_EXTERN const char   *toml_table_key       (const toml_table_t *table, int keyidx, int *keylen);
	TOML_EXTERN toml_value_t  toml_table_string    (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_value_t  toml_table_bool      (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_value_t  toml_table_int       (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_value_t  toml_table_double    (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_value_t  toml_table_timestamp (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_array_t *toml_table_array     (const toml_table_t *table, const char *key);
	TOML_EXTERN toml_table_t *toml_table_table     (const toml_table_t *table, const char *key);

// Array functions.
	TOML_EXTERN int           toml_array_len       (const toml_array_t *array);
	TOML_EXTERN toml_value_t  toml_array_string    (const toml_array_t *array, int idx);
	TOML_EXTERN toml_value_t  toml_array_bool      (const toml_array_t *array, int idx);
	TOML_EXTERN toml_value_t  toml_array_int       (const toml_array_t *array, int idx);
	TOML_EXTERN toml_value_t  toml_array_double    (const toml_array_t *array, int idx);
	TOML_EXTERN toml_value_t  toml_array_timestamp (const toml_array_t *array, int idx);
	TOML_EXTERN toml_array_t *toml_array_array     (const toml_array_t *array, int idx);
	TOML_EXTERN toml_table_t *toml_array_table     (const toml_array_t *array, int idx);

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ALIGN8(sz) (((sz) + 7) & ~7)
#define calloc(x, y) error - forbidden - use CALLOC instead
static void *CALLOC(size_t nmemb, size_t sz) {
	int nb = ALIGN8(sz) * nmemb;
	void *p = malloc(nb);
	if (p) {
		memset(p, 0, nb);
	}
	return p;
}

// some old platforms define strdup macro -- drop it.
#undef strdup
#define strdup(x) error - forbidden - use STRDUP instead
static char *STRDUP(const char *s) {
	int len = strlen(s);
	char *p = malloc(len + 1);
	if (p) {
		memcpy(p, s, len);
		p[len] = 0;
	}
	return p;
}

// some old platforms define strndup macro -- drop it.
#undef strndup
#define strndup(x) error - forbidden - use STRNDUP instead
static char *STRNDUP(const char *s, size_t n) {
	size_t len = strnlen(s, n);
	char *p = malloc(len + 1);
	if (p) {
		memcpy(p, s, len);
		p[len] = 0;
	}
	return p;
}

// Unparsed values.
typedef const char *toml_unparsed_t;
toml_unparsed_t toml_table_unparsed  (const toml_table_t *table, const char *key);
toml_unparsed_t toml_array_unparsed  (const toml_array_t *array, int idx);
int             toml_value_string    (toml_unparsed_t s, char **ret, int *len);
int             toml_value_bool      (toml_unparsed_t s, bool *ret);
int             toml_value_int       (toml_unparsed_t s, int64_t *ret);
int             toml_value_double    (toml_unparsed_t s, double *ret);
int             toml_value_timestamp (toml_unparsed_t s, toml_timestamp_t *ret);

// Convert escape to UTF-8; return #bytes used in buf to encode the char, or -1
// on error.
// http://stackoverflow.com/questions/6240055/manually-converting-unicode-codepoints-into-utf-8-and-utf-16
int read_unicode_escape(int64_t code, char buf[6]) {
	if (0xd800 <= code && code <= 0xdfff) /// UTF-16 surrogates
		return -1;
	if (0x10FFFF < code)
		return -1;
	if (code < 0)
		return -1;
	if (code <= 0x7F) { /// 0x00000000 - 0x0000007F: 0xxxxxxx
		buf[0] = (unsigned char)code;
		return 1;
	}
	if (code <= 0x000007FF) { /// 0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
		buf[0] = (unsigned char)(0xc0 | (code >> 6));
		buf[1] = (unsigned char)(0x80 | (code & 0x3f));
		return 2;
	}
	if (code <= 0x0000FFFF) { /// 0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
		buf[0] = (unsigned char)(0xe0 | (code >> 12));
		buf[1] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
		buf[2] = (unsigned char)(0x80 | (code & 0x3f));
		return 3;
	}
	if (code <= 0x001FFFFF) { /// 0x00010000 - 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		buf[0] = (unsigned char)(0xf0 | (code >> 18));
		buf[1] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
		buf[2] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
		buf[3] = (unsigned char)(0x80 | (code & 0x3f));
		return 4;
	}
	if (code <= 0x03FFFFFF) { /// 0x00200000 - 0x03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		buf[0] = (unsigned char)(0xf8 | (code >> 24));
		buf[1] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
		buf[2] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
		buf[3] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
		buf[4] = (unsigned char)(0x80 | (code & 0x3f));
		return 5;
	}
	if (code <= 0x7FFFFFFF) { /// 0x04000000 - 0x7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		buf[0] = (unsigned char)(0xfc | (code >> 30));
		buf[1] = (unsigned char)(0x80 | ((code >> 24) & 0x3f));
		buf[2] = (unsigned char)(0x80 | ((code >> 18) & 0x3f));
		buf[3] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
		buf[4] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
		buf[5] = (unsigned char)(0x80 | (code & 0x3f));
		return 6;
	}
	return -1;
}

static inline void xfree(const void *x) {
	if (x)
		free((void *)(intptr_t)x);
}

enum tokentype_t {
	INVALID,
	DOT,
	COMMA,
	EQUAL,
	LBRACE,
	RBRACE,
	NEWLINE,
	LBRACKET,
	RBRACKET,
	STRING,
	MSTRING
};
typedef enum tokentype_t tokentype_t;

typedef struct token_t token_t;
struct token_t {
	tokentype_t  tok;
	toml_pos_t   pos;
	char        *ptr; // points into context->start
	int          len;
	int          eof;
};

typedef struct context_t context_t;
struct context_t {
	char *start;
	char *stop;
	char *errbuf;
	int   errbufsz;

	token_t       tok;
	toml_table_t *root;
	toml_table_t *curtbl;

	struct {
		int      top;
		char    *key[10];
		int      keylen[10];
		token_t  tok[10];
	} tpath;
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FLINE __FILE__ ":" TOSTRING(__LINE__)

static int next_token(context_t *ctx, bool dotisspecial);

// Error reporting. Call when an error is detected. Always return -1.
static int e_outofmemory(context_t *ctx, const char *fline) {
	snprintf(ctx->errbuf, ctx->errbufsz, "ERROR: out of memory (%s)", fline);
	return -1;
}

static int e_internal(context_t *ctx, const char *fline) {
	snprintf(ctx->errbuf, ctx->errbufsz, "internal error (%s)", fline);
	return -1;
}

static int e_syntax(context_t *ctx, toml_pos_t pos, const char *msg) {
	snprintf(ctx->errbuf, ctx->errbufsz, "at %d:%d: %s", pos.line, pos.col, msg);
	return -1;
}

static int e_keyexists(context_t *ctx, toml_pos_t pos) {
	snprintf(ctx->errbuf, ctx->errbufsz, "at %d:%d: key already defined", pos.line, pos.col);
	return -1;
}

static void *expand(void *p, int sz, int newsz) {
	void *s = malloc(newsz);
	if (!s)
		return 0;

	if (p) {
		memcpy(s, p, sz);
		free(p);
	}
	return s;
}

static void **expand_ptrarr(void **p, int n) {
	void **s = malloc((n + 1) * sizeof(void *));
	if (!s)
		return 0;

	s[n] = 0;
	if (p) {
		memcpy(s, p, n * sizeof(void *));
		free(p);
	}
	return s;
}

static toml_arritem_t *expand_arritem(toml_arritem_t *p, int n) {
	toml_arritem_t *pp = expand(p, n * sizeof(*p), (n + 1) * sizeof(*p));
	if (!pp)
		return 0;

	memset(&pp[n], 0, sizeof(pp[n]));
	return pp;
}

static uint8_t const u8_length[] = {1,1,1,1,1,1,1,1,0,0,0,0,2,2,3,4};
#define u8length(s) u8_length[(((uint8_t *)(s))[0] & 0xFF) >> 4];

static char *norm_lit_str(const char *src, int srclen, int *len, bool multiline, bool is_key, char *errbuf, int errbufsz) {
	const char *sp  = src;
	const char *sq  = src + srclen;
	char       *dst = 0; /// will write to dst[] and return it
	int        max  = 0; /// max size of dst[]
	int        off  = 0; /// cur offset in dst[]

	for (;;) { /// scan forward on src
		if (off >= max - 10) { /// have some slack for misc stuff
			int newmax = max + 50;
			char *x = expand(dst, max, newmax);
			if (!x) {
				xfree(dst);
				snprintf(errbuf, errbufsz, "out of memory");
				return 0;
			}
			dst = x;
			max = newmax;
		}

		if (sp >= sq) /// finished?
			break;

		uint8_t l = u8length(sp);
		if (l == 0) {
			xfree(dst);
			snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
			return 0;
		}
		if (l > 1) {
			for (int i = 0; i < l; i++) {
				char ch = *sp++;
				if ((ch & 0x80) != 0x80) {
					xfree(dst);
					snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
					return 0;
				}
				dst[off++] = ch;
			}
			continue;
		}

		char ch = *sp++;
		if (is_key && ch == '\n') {
			xfree(dst);
			snprintf(errbuf, errbufsz, "literal newlines not allowed in key");
			return 0;
		}
		/// control characters other than Tab are not allowed
		if ((0 <= ch && ch <= 0x08) || (0x0a <= ch && ch <= 0x1f) || ch == 0x7f) {
			if (!(multiline && (ch == '\r' || ch == '\n'))) {
				xfree(dst);
				snprintf(errbuf, errbufsz, "invalid char U+%04x", ch);
				return 0;
			}
		}

		dst[off++] = ch; /// a plain copy suffice
	}

	*len = off;
	dst[off++] = 0;
	return dst;
}

// Convert src to raw unescaped utf-8 string. Returns NULL if error with errmsg
// in errbuf.
static char *norm_basic_str(const char *src, int srclen, int *len, bool multiline, bool is_key, char *errbuf, int errbufsz) {
	const char *sp  = src;
	const char *sq  = src + srclen;
	char       *dst = 0; /// will write to dst[] and return it
	int        max  = 0; /// max size of dst[]
	int        off  = 0; /// cur offset in dst[]
						 ///
	/// scan forward on src
	for (;;) {
		if (off >= max - 10) { /// have some slack for misc stuff
			int newmax = max + 50;
			char *x = expand(dst, max, newmax);
			if (!x) {
				xfree(dst);
				snprintf(errbuf, errbufsz, "out of memory");
				return 0;
			}
			dst = x;
			max = newmax;
		}

		if (sp >= sq) /// finished?
			break;

		uint8_t l = u8length(sp);
		if (l == 0) {
			xfree(dst);
			snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
			return 0;
		}
		if (l > 1) {
			for (int i = 0; i < l; i++) {
				char ch = *sp++;
				if ((ch & 0x80) != 0x80) {
					xfree(dst);
					snprintf(errbuf, errbufsz, "invalid UTF-8 at byte pos %d", off);
					return 0;
				}
				dst[off++] = ch;
			}
			continue;
		}

		char ch = *sp++;
		if (is_key && ch == '\n') {
			xfree(dst);
			snprintf(errbuf, errbufsz, "literal newlines not allowed in key");
			return 0;
		}
		if (ch != '\\') {
			/// must be escaped: U+0000 to U+0008, U+000A to U+001F, U+007F
			if ((ch >= 0 && ch <= 0x08) || (ch >= 0x0a && ch <= 0x1f) || ch == 0x7f) {
				if (!(multiline && (ch == '\r' || ch == '\n'))) {
					xfree(dst);
					snprintf(errbuf, errbufsz, "invalid char U+%04x", ch);
					return 0;
				}
			}

			dst[off++] = ch; /// a plain copy suffice
			continue;
		}

		if (sp >= sq) { /// ch was backslash. we expect the escape char.
			snprintf(errbuf, errbufsz, "last backslash is invalid");
			xfree(dst);
			return 0;
		}

		if (multiline) { /// for multi-line, we want to kill line-ending-backslash.
			if (sp[strspn(sp, " \t\r")] == '\n') { /// if there is only whitespace after the backslash ...
				sp += strspn(sp, " \t\r\n"); /// skip all the following whitespaces
				continue;
			}
		}

		ch = *sp++; /// get the escaped char
		switch (ch) {
			case 'u':
			case 'U': {
				int64_t ucs = 0;
				int nhex = (ch == 'u' ? 4 : 8);
				for (int i = 0; i < nhex; i++) {
					if (sp >= sq) {
						snprintf(errbuf, errbufsz, "\\%c expected %d hex chars", ch, nhex);
						xfree(dst);
						return 0;
					}
					ch = *sp++;
					int v = -1;
					if ('0' <= ch && ch <= '9')
						v = ch - '0';
					else if ('A' <= ch && ch <= 'F')
						v = ch - 'A' + 10;
					else if ('a' <= ch && ch <= 'f')
						v = (ch ^ 0x20) - 'A' + 10;
					if (v == -1) {
						snprintf(errbuf, errbufsz, "invalid hex chars for \\u or \\U");
						xfree(dst);
						return 0;
					}
					ucs = ucs * 16 + v;
				}
				int n = read_unicode_escape(ucs, &dst[off]);
				if (n == -1) {
					snprintf(errbuf, errbufsz, "illegal ucs code in \\u or \\U");
					xfree(dst);
					return 0;
				}
				off += n;
			}; continue;
			case 'b':  ch = '\b';  break;
			case 't':  ch = '\t';  break;
			case 'n':  ch = '\n';  break;
			case 'f':  ch = '\f';  break;
			case 'r':  ch = '\r';  break;
			case '"':  ch = '"';   break;
			case '\\': ch = '\\'; break;
			default:
				snprintf(errbuf, errbufsz, "illegal escape char \\%c", ch);
				xfree(dst);
				return 0;
		}

		dst[off++] = ch;
	}

	*len = off;
	dst[off++] = 0; /// Cap with NUL and return it.
	return dst;
}

// Normalize a key. Convert all special chars to raw unescaped utf-8 chars.
static char *normalize_key(context_t *ctx, token_t strtok, int *keylen) {
	const char *sp   = strtok.ptr;
	const char *sq   = strtok.ptr + strtok.len;
	int        ch    = *sp;
	char       *ret;

	// Quoted string
	if (ch == '\'' || ch == '\"') {
		/// if ''' or """, take 3 chars off front and back. Else, take 1 char off.
		bool multiline = (sp[1] == ch && sp[2] == ch);
		if (multiline)
			sp += 3, sq -= 3;
		else
			sp++, sq--;

		char ebuf[80];
		if (ch == '\'')
			ret = norm_lit_str(sp, sq - sp, keylen, multiline, true, ebuf, sizeof(ebuf));
		else
			ret = norm_basic_str(sp, sq - sp, keylen, multiline, true, ebuf, sizeof(ebuf));
		if (!ret) {
			e_syntax(ctx, strtok.pos, ebuf);
			return 0;
		}
		return ret;
	}

	*keylen = 0;
	for (const char *c = sp; c != sq; c++) { /// Bare key: allow: [A-Za-z0-9_-]+
		*keylen = *keylen + 1;
		if (isalnum(*c) || *c == '_' || *c == '-')
			continue;
		// TODO: never triggered? When reading the file it already validates
		// this, so seems redundant? Need to double-check.
		e_syntax(ctx, ctx->tok.pos, "invalid key");
		return 0;
	}

	if (!(ret = STRNDUP(sp, sq - sp))) { /// dup and return
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	return ret;
}

// Look up key in tbl. Return 0 if not found, or 'v'alue, 'a'rray or 't'able
// depending on the element.
static int check_key(toml_table_t *tbl, const char *key, toml_keyval_t **ret_val, toml_array_t **ret_arr, toml_table_t **ret_tbl) {
	int i;
	void *dummy;

	if (!ret_tbl)
		ret_tbl = (toml_table_t **)&dummy;
	if (!ret_arr)
		ret_arr = (toml_array_t **)&dummy;
	if (!ret_val)
		ret_val = (toml_keyval_t **)&dummy;

	*ret_tbl = 0;
	*ret_arr = 0;
	*ret_val = 0;

	for (i = 0; i < tbl->nkval; i++) {
		if (strcmp(key, tbl->kval[i]->key) == 0) {
			*ret_val = tbl->kval[i];
			return 'v';
		}
	}
	for (i = 0; i < tbl->narr; i++) {
		if (strcmp(key, tbl->arr[i]->key) == 0) {
			*ret_arr = tbl->arr[i];
			return 'a';
		}
	}
	for (i = 0; i < tbl->ntbl; i++) {
		if (strcmp(key, tbl->tbl[i]->key) == 0) {
			*ret_tbl = tbl->tbl[i];
			return 't';
		}
	}
	return 0;
}

static int key_kind(toml_table_t *tbl, const char *key) {
	return check_key(tbl, key, 0, 0, 0);
}

// Create a keyval in the table.
static toml_keyval_t *create_keyval_in_table(context_t *ctx, toml_table_t *tbl, token_t keytok) {
	int keylen;
	char *newkey = normalize_key(ctx, keytok, &keylen);
	if (!newkey)
		return 0;

	toml_keyval_t *dest = 0;
	if (key_kind(tbl, newkey)) {
		xfree(newkey);
		e_keyexists(ctx, keytok.pos);
		return 0;
	}

	int n = tbl->nkval;
	toml_keyval_t **base;
	if ((base = (toml_keyval_t **)expand_ptrarr((void **)tbl->kval, n)) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	tbl->kval = base;

	if ((base[n] = (toml_keyval_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}

	dest = tbl->kval[tbl->nkval++];
	dest->key = newkey;
	dest->keylen = keylen;
	return dest;
}

// Create a table in the table.
static toml_table_t *create_keytable_in_table(context_t *ctx, toml_table_t *tbl, token_t keytok) {
	int keylen;
	char *newkey = normalize_key(ctx, keytok, &keylen);
	if (!newkey)
		return 0;

	toml_table_t *dest = 0;
	// TODO: need to check all parts for:
	//
	//   [a]
	//   [a.c]   # checks of "a.c" is defined, which is false.
	if (check_key(tbl, newkey, 0, 0, &dest)) {
		xfree(newkey);

		/// Special case: make explicit if table exists and was created
		/// implicitly.
		if (dest && dest->implicit) {
			dest->implicit = false;
			return dest;
		}
		e_keyexists(ctx, keytok.pos);
		return 0;
	}

	int n = tbl->ntbl;
	toml_table_t **base;
	if ((base = (toml_table_t **)expand_ptrarr((void **)tbl->tbl, n)) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	tbl->tbl = base;

	if ((base[n] = (toml_table_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}

	dest = tbl->tbl[tbl->ntbl++];
	dest->key = newkey;
	dest->keylen = keylen;
	return dest;
}

// Create an array in the table.
static toml_array_t *create_keyarray_in_table(context_t *ctx, toml_table_t *tbl, token_t keytok, char kind) {
	int keylen;
	char *newkey = normalize_key(ctx, keytok, &keylen);
	if (!newkey)
		return 0;

	if (key_kind(tbl, newkey)) {
		xfree(newkey);
		e_keyexists(ctx, keytok.pos);
		return 0;
	}

	int n = tbl->narr;
	toml_array_t **base;
	if ((base = (toml_array_t **)expand_ptrarr((void **)tbl->arr, n)) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	tbl->arr = base;

	if ((base[n] = (toml_array_t *)CALLOC(1, sizeof(*base[n]))) == 0) {
		xfree(newkey);
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	toml_array_t *dest = tbl->arr[tbl->narr++];

	dest->keylen = keylen;
	dest->key = newkey;
	dest->kind = kind;
	return dest;
}

static toml_arritem_t *create_value_in_array(context_t *ctx, toml_array_t *parent) {
	const int n = parent->nitem;
	toml_arritem_t *base = expand_arritem(parent->item, n);
	if (!base) {
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	parent->item = base;
	parent->nitem++;
	return &parent->item[n];
}

// Create an array in an array.
static toml_array_t *create_array_in_array(context_t *ctx, toml_array_t *parent) {
	const int n = parent->nitem;
	toml_arritem_t *base = expand_arritem(parent->item, n);
	if (!base) {
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	toml_array_t *ret = (toml_array_t *)CALLOC(1, sizeof(toml_array_t));
	if (!ret) {
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	base[n].arr = ret;
	parent->item = base;
	parent->nitem++;
	return ret;
}

// Create a table in an array
static toml_table_t *create_table_in_array(context_t *ctx, toml_array_t *parent) {
	int n = parent->nitem;
	toml_arritem_t *base = expand_arritem(parent->item, n);
	if (!base) {
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	toml_table_t *ret = (toml_table_t *)CALLOC(1, sizeof(toml_table_t));
	if (!ret) {
		e_outofmemory(ctx, FLINE);
		return 0;
	}
	base[n].tbl = ret;
	parent->item = base;
	parent->nitem++;
	return ret;
}

static bool skip_newlines(context_t *ctx, bool isdotspecial) {
	while (ctx->tok.tok == NEWLINE) {
		if (next_token(ctx, isdotspecial))
			return false;
		if (ctx->tok.eof)
			break;
	}
	return true;
}

static int parse_keyval(context_t *ctx, toml_table_t *tbl);

static inline int eat_token(context_t *ctx, tokentype_t typ, bool isdotspecial, const char *fline) {
	if (ctx->tok.tok != typ)
		return e_internal(ctx, fline);
	if (next_token(ctx, isdotspecial))
		return -1;
	return 0;
}

// We are at '{ ... }'; parse the table.
static int parse_inline_table(context_t *ctx, toml_table_t *tbl) {
	if (eat_token(ctx, LBRACE, 1, FLINE))
		return -1;

	for (;;) {
		if (ctx->tok.tok == NEWLINE)
			return e_syntax(ctx, ctx->tok.pos, "newline not allowed in inline table");

		if (ctx->tok.tok == RBRACE) // until closing brace
			break;

		if (ctx->tok.tok != STRING)
			return e_syntax(ctx, ctx->tok.pos, "expected a string");

		if (parse_keyval(ctx, tbl))
			return -1;

		if (ctx->tok.tok == NEWLINE)
			return e_syntax(ctx, ctx->tok.pos, "newline not allowed in inline table");

		// On comma, continue to scan for next keyval.
		if (ctx->tok.tok == COMMA) {
			if (eat_token(ctx, COMMA, 1, FLINE))
				return -1;
			continue;
		}
		break;
	}

	if (eat_token(ctx, RBRACE, 1, FLINE))
		return -1;
	tbl->readonly = 1;
	return 0;
}

static int valtype(const char *val) {
	toml_timestamp_t ts;
	if (*val == '\'' || *val == '"')
		return 's';
	if (toml_value_bool(val, false) == 0)
		return 'b';
	if (toml_value_int(val, 0) == 0)
		return 'i';
	if (toml_value_double(val, 0) == 0)
		return 'd';
	if (toml_value_timestamp(val, &ts) == 0) {
		if (ts.year && ts.hour)
			return 'T'; /// timestamp
		if (ts.year)
			return 'D'; /// date
		return 't';   /// time
	}
	return 'u'; /// unknown
}

// We are at '[...]'
static int parse_array(context_t *ctx, toml_array_t *arr) {
	if (eat_token(ctx, LBRACKET, 0, FLINE))
		return -1;

	for (;;) {
		if (!skip_newlines(ctx, 0))
			return -1;

		if (ctx->tok.tok == RBRACKET) /// until ]
			break;

		switch (ctx->tok.tok) {
			case MSTRING:
			case STRING: {
				/// set array kind if this will be the first entry
				if (arr->kind == 0)
					arr->kind = 'v';
				else if (arr->kind != 'v')
					arr->kind = 'm';

				char *val = ctx->tok.ptr;
				int vlen = ctx->tok.len;

				/// make a new value in array
				toml_arritem_t *newval = create_value_in_array(ctx, arr);
				if (!newval)
					return e_outofmemory(ctx, FLINE);

				if (!(newval->val = STRNDUP(val, vlen)))
					return e_outofmemory(ctx, FLINE);

				newval->valtype = valtype(newval->val);

				/// set array type if this is the first entry
				if (arr->nitem == 1)
					arr->type = newval->valtype;
				else if (arr->type != newval->valtype)
					arr->type = 'm'; /// mixed

				if (eat_token(ctx, ctx->tok.tok, 0, FLINE))
					return -1;
				break;
			}
			case LBRACKET: { // [ [array], [array] ... ]
				// set the array kind if this will be the first entry.
				if (arr->kind == 0)
					arr->kind = 'a';
				else if (arr->kind != 'a')
					arr->kind = 'm';

				toml_array_t *subarr = create_array_in_array(ctx, arr);
				if (!subarr)
					return -1;
				if (parse_array(ctx, subarr))
					return -1;
				break;
			}
			case LBRACE: { // [ {table}, {table} ... ]
				// set the array kind if this will be the first entry.
				if (arr->kind == 0)
					arr->kind = 't';
				else if (arr->kind != 't')
					arr->kind = 'm';

				toml_table_t *subtbl = create_table_in_array(ctx, arr);
				if (!subtbl)
					return -1;
				if (parse_inline_table(ctx, subtbl))
					return -1;
				break;
			}
			default:
				return e_syntax(ctx, ctx->tok.pos, "syntax error");
		}

		if (!skip_newlines(ctx, 0))
			return -1;

		// on comma, continue to scan for next element
		if (ctx->tok.tok == COMMA) {
			if (eat_token(ctx, COMMA, 0, FLINE))
				return -1;
			continue;
		}
		break;
	}

	if (eat_token(ctx, RBRACKET, 1, FLINE))
		return -1;
	return 0;
}

// Handle lines like:
//   key = "value"
//   key = [ array ]
//   key = { table }
static int parse_keyval(context_t *ctx, toml_table_t *tbl) {
	if (tbl->readonly)
		return e_keyexists(ctx, ctx->tok.pos);

	token_t key = ctx->tok;
	if (eat_token(ctx, STRING, 1, FLINE))
		return -1;

	if (ctx->tok.tok == DOT) {
		// Handle inline dotted key:
		//   physical.color = "orange"
		//   physical.shape = "round"
		toml_table_t *subtbl = 0;
		{
			int keylen;
			char *subtblstr = normalize_key(ctx, key, &keylen);
			if (!subtblstr)
				return -1;

			subtbl = toml_table_table(tbl, subtblstr);
			if (subtbl)
				subtbl->keylen = keylen;
			xfree(subtblstr);
		}
		if (!subtbl) {
			subtbl = create_keytable_in_table(ctx, tbl, key);
			if (!subtbl)
				return -1;
		}
		if (next_token(ctx, true))
			return -1;
		if (parse_keyval(ctx, subtbl))
			return -1;
		return 0;
	}

	if (ctx->tok.tok != EQUAL)
		return e_syntax(ctx, ctx->tok.pos, "missing '='");

	if (next_token(ctx, false))
		return -1;

	switch (ctx->tok.tok) {
		case MSTRING:
		case STRING: { // key = "value"
			toml_keyval_t *keyval = create_keyval_in_table(ctx, tbl, key);
			if (!keyval)
				return -1;
			token_t val = ctx->tok;

			assert(keyval->val == 0);
			if (!(keyval->val = STRNDUP(val.ptr, val.len)))
				return e_outofmemory(ctx, FLINE);

			if (next_token(ctx, true))
				return -1;

			return 0;
		}
		case LBRACKET: { // key = [ array ]
			toml_array_t *arr = create_keyarray_in_table(ctx, tbl, key, 0);
			if (!arr)
				return -1;
			if (parse_array(ctx, arr))
				return -1;
			return 0;
		}
		case LBRACE: { // key = { table }
			toml_table_t *nexttbl = create_keytable_in_table(ctx, tbl, key);
			if (!nexttbl)
				return -1;
			if (parse_inline_table(ctx, nexttbl))
				return -1;
			return 0;
		}
		default:
			return e_syntax(ctx, ctx->tok.pos, "syntax error");
	}
	return 0;
}

typedef struct tabpath_t tabpath_t;
struct tabpath_t {
	int     cnt;
	token_t key[10];
};

// At [x.y.z] or [[x.y.z]]
// Scan forward and fill tblpath until it enters ] or ]]
// There will be at least one entry on return.
static int fill_tblpath(context_t *ctx) {
	// clear tpath
	for (int i = 0; i < ctx->tpath.top; i++) {
		char **p = &ctx->tpath.key[i];
		xfree(*p);
		*p = 0;
	}
	ctx->tpath.top = 0;

	for (;;) {
		if (ctx->tpath.top >= 10)
			return e_syntax(ctx, ctx->tok.pos, "table path is too deep; max allowed is 10.");
		if (ctx->tok.tok != STRING)
			return e_syntax(ctx, ctx->tok.pos, "invalid or missing key");

		int keylen;
		char *key = normalize_key(ctx, ctx->tok, &keylen);
		if (!key)
			return -1;
		ctx->tpath.tok[ctx->tpath.top] = ctx->tok;
		ctx->tpath.key[ctx->tpath.top] = key;
		ctx->tpath.keylen[ctx->tpath.top] = keylen;
		ctx->tpath.top++;

		if (next_token(ctx, true))
			return -1;

		if (ctx->tok.tok == RBRACKET)
			break;
		if (ctx->tok.tok != DOT)
			return e_syntax(ctx, ctx->tok.pos, "invalid key");
		if (next_token(ctx, true))
			return -1;
	}

	if (ctx->tpath.top <= 0)
		return e_syntax(ctx, ctx->tok.pos, "empty table selector");
	return 0;
}

// Walk tblpath from the root, and create new tables on the way. Sets
// ctx->curtbl to the final table.
static int walk_tabpath(context_t *ctx) {
	toml_table_t *curtbl = ctx->root; /// start from root

	for (int i = 0; i < ctx->tpath.top; i++) {
		const char *key = ctx->tpath.key[i];
		int keylen = ctx->tpath.keylen[i];

		toml_keyval_t *nextval = 0;
		toml_array_t *nextarr = 0;
		toml_table_t *nexttbl = 0;
		switch (check_key(curtbl, key, &nextval, &nextarr, &nexttbl)) {
			case 't': /// found a table. nexttbl is where we will go next.
				break;
			case 'a': /// found an array. nexttbl is the last table in the array.
				if (nextarr->kind != 't')
					return e_internal(ctx, FLINE);

				if (nextarr->nitem == 0)
					return e_internal(ctx, FLINE);

				nexttbl = nextarr->item[nextarr->nitem - 1].tbl;
				break;
			case 'v':
				return e_keyexists(ctx, ctx->tpath.tok[i].pos);
			default: { /// Not found. Let's create an implicit table.
				int n = curtbl->ntbl;
				toml_table_t **base = (toml_table_t **)expand_ptrarr((void **)curtbl->tbl, n);
				if (base == 0)
					return e_outofmemory(ctx, FLINE);

				curtbl->tbl = base;

				if ((base[n] = (toml_table_t *)CALLOC(1, sizeof(*base[n]))) == 0)
					return e_outofmemory(ctx, FLINE);

				if ((base[n]->key = STRDUP(key)) == 0)
					return e_outofmemory(ctx, FLINE);
				base[n]->keylen = keylen;

				nexttbl = curtbl->tbl[curtbl->ntbl++];

				/// tabs created by walk_tabpath are considered implicit
				nexttbl->implicit = true;
			}; break;
		}
		curtbl = nexttbl; /// switch to next tbl
	}

	ctx->curtbl = curtbl; /// save it
	return 0;
}

// handle lines like [x.y.z] or [[x.y.z]]
static int parse_select(context_t *ctx) {
	assert(ctx->tok.tok == LBRACKET);

	// true if [[
	bool aot = (ctx->tok.ptr + 1 < ctx->stop && ctx->tok.ptr[1] == '[');

	// Need to detect '[[' on our own because next_token() will skip whitespace,
	// and '[ [' would be taken as '[[', which is wrong.

	// eat [ or [[
	if (eat_token(ctx, LBRACKET, 1, FLINE))
		return -1;
	if (aot) {
		assert(ctx->tok.tok == LBRACKET);
		if (eat_token(ctx, LBRACKET, 1, FLINE))
			return -1;
	}

	if (fill_tblpath(ctx))
		return -1;

	// For [x.y.z] or [[x.y.z]], remove z from tpath.
	token_t z = ctx->tpath.tok[ctx->tpath.top - 1];
	xfree(ctx->tpath.key[ctx->tpath.top - 1]);
	ctx->tpath.top--;

	// Set up ctx->curtbl.
	if (walk_tabpath(ctx))
		return -1;

	if (!aot) {
		// [x.y.z] -> create z = {} in x.y
		toml_table_t *curtbl = create_keytable_in_table(ctx, ctx->curtbl, z);
		if (!curtbl)
			return -1;
		ctx->curtbl = curtbl;
	} else {
		// [[x.y.z]] -> create z = [] in x.y
		toml_array_t *arr = 0;
		{
			int keylen;
			char *zstr = normalize_key(ctx, z, &keylen);
			if (!zstr)
				return -1;
			arr = toml_table_array(ctx->curtbl, zstr);
			if (arr)
				arr->keylen = keylen;
			xfree(zstr);
		}
		if (!arr) {
			arr = create_keyarray_in_table(ctx, ctx->curtbl, z, 't');
			if (!arr)
				return -1;
		}
		if (arr->kind != 't')
			return e_syntax(ctx, z.pos, "array mismatch");

		// add to z[]
		toml_table_t *dest;
		{
			toml_table_t *t = create_table_in_array(ctx, arr);
			if (!t)
				return -1;

			if ((t->key = STRDUP("__anon__")) == 0)
				return e_outofmemory(ctx, FLINE);
			dest = t;
		}

		ctx->curtbl = dest;
	}

	if (ctx->tok.tok != RBRACKET)
		return e_syntax(ctx, ctx->tok.pos, "expected ']'");
	if (aot) {
		if (!(ctx->tok.ptr + 1 < ctx->stop && ctx->tok.ptr[1] == ']'))
			return e_syntax(ctx, ctx->tok.pos, "expected ']]'");
		if (eat_token(ctx, RBRACKET, 1, FLINE))
			return -1;
	}

	if (eat_token(ctx, RBRACKET, 1, FLINE))
		return -1;
	if (ctx->tok.tok != NEWLINE)
		return e_syntax(ctx, ctx->tok.pos, "extra chars after ] or ]]");
	return 0;
}

toml_table_t *toml_parse(char *toml, char *errbuf, int errbufsz) {
	context_t ctx;

	/// clear errbuf
	if (errbufsz <= 0)
		errbufsz = 0;
	if (errbufsz > 0)
		errbuf[0] = 0;

	// init context
	memset(&ctx, 0, sizeof(ctx));
	ctx.start = toml;
	ctx.stop = ctx.start + strlen(toml);
	ctx.errbuf = errbuf;
	ctx.errbufsz = errbufsz;

	// start with an artificial newline of length 0
	ctx.tok.tok = NEWLINE;
	ctx.tok.pos.line = 1;
	ctx.tok.pos.col = 1;
	ctx.tok.ptr = toml;
	ctx.tok.len = 0;

	// make a root table
	if ((ctx.root = CALLOC(1, sizeof(*ctx.root))) == 0) {
		e_outofmemory(&ctx, FLINE);
		return 0; // Do not goto fail, root table not set up yet
	}

	// set root as default table
	ctx.curtbl = ctx.root;

	// Scan forward until EOF
	for (token_t tok = ctx.tok; !tok.eof; tok = ctx.tok) {
		switch (tok.tok) {
			case NEWLINE:
				if (next_token(&ctx, true))
					goto fail;
				break;

			case STRING:
				if (parse_keyval(&ctx, ctx.curtbl))
					goto fail;

				if (ctx.tok.tok != NEWLINE) {
					e_syntax(&ctx, ctx.tok.pos, "extra chars after value");
					goto fail;
				}

				if (eat_token(&ctx, NEWLINE, 1, FLINE))
					goto fail;
				break;

			case LBRACKET: // [ x.y.z ] or [[ x.y.z ]]
				if (parse_select(&ctx))
					goto fail;
				break;

			default:
				e_syntax(&ctx, tok.pos, "syntax error");
				goto fail;
		}
	}

	/// success
	for (int i = 0; i < ctx.tpath.top; i++)
		xfree(ctx.tpath.key[i]);
	return ctx.root;

fail:
	// Something bad has happened. Free resources and return error.
	for (int i = 0; i < ctx.tpath.top; i++)
		xfree(ctx.tpath.key[i]);
	toml_free(ctx.root);
	return 0;
}

toml_table_t *toml_parse_file(FILE *fp, char *errbuf, int errbufsz) {
	int bufsz = 0;
	char *buf = 0;
	int off = 0;
	int inc = 1024;

	while (!feof(fp)) {
		if (bufsz == 1024 * 20) /// Increment buffer by 20k after 20k.
			inc = 1024 * 20;
		if (off == bufsz) {
			int xsz = bufsz + inc;
			char *x = expand(buf, bufsz, xsz);
			if (!x) {
				snprintf(errbuf, errbufsz, "out of memory");
				xfree(buf);
				return 0;
			}
			buf = x;
			bufsz = xsz;
		}

		errno = 0;
		int n = fread(buf + off, 1, bufsz - off, fp);
		if (ferror(fp)) {
			snprintf(errbuf, errbufsz, "%s", (errno ? strerror(errno) : "Error reading file"));
			xfree(buf);
			return 0;
		}
		off += n;
	}

	/// tag on a NUL to cap the string
	if (off == bufsz) {
		int xsz = bufsz + 1;
		char *x = expand(buf, bufsz, xsz);
		if (!x) {
			snprintf(errbuf, errbufsz, "out of memory");
			xfree(buf);
			return 0;
		}
		buf = x;
		bufsz = xsz;
	}
	buf[off] = 0;

	/// parse it, cleanup and finish.
	toml_table_t *ret = toml_parse(buf, errbuf, errbufsz);
	xfree(buf);
	return ret;
}

static void xfree_kval(toml_keyval_t *p) {
	if (!p)
		return;
	xfree(p->key);
	xfree(p->val);
	xfree(p);
}

static void xfree_tbl(toml_table_t *p);

static void xfree_arr(toml_array_t *p) {
	if (!p)
		return;

	xfree(p->key);
	const int n = p->nitem;
	for (int i = 0; i < n; i++) {
		toml_arritem_t *a = &p->item[i];
		if (a->val)
			xfree(a->val);
		else if (a->arr)
			xfree_arr(a->arr);
		else if (a->tbl)
			xfree_tbl(a->tbl);
	}
	xfree(p->item);
	xfree(p);
}

static void xfree_tbl(toml_table_t *p) {
	if (!p)
		return;

	xfree(p->key);

	for (int i = 0; i < p->nkval; i++)
		xfree_kval(p->kval[i]);
	xfree(p->kval);

	for (int i = 0; i < p->narr; i++)
		xfree_arr(p->arr[i]);
	xfree(p->arr);

	for (int i = 0; i < p->ntbl; i++)
		xfree_tbl(p->tbl[i]);
	xfree(p->tbl);

	xfree(p);
}

void toml_free(toml_table_t *tbl) { xfree_tbl(tbl); }

static void set_token(context_t *ctx, tokentype_t tok, toml_pos_t pos, char *ptr, int len) {
	token_t t;
	t.tok    = tok;
	t.pos    = pos;
	t.ptr    = ptr;
	t.len    = len;
	t.eof    = 0;
	ctx->tok = t;
}

static void set_eof(context_t *ctx, toml_pos_t pos) {
	set_token(ctx, NEWLINE, pos, ctx->stop, 0);
	ctx->tok.eof = 1;
}

// Scan p for n digits compositing entirely of [0-9]
static int scan_digits(const char *p, int n) {
	int ret = 0;
	for (; n > 0 && isdigit(*p); n--, p++)
		ret = 10 * ret + (*p - '0');
	return n ? -1 : ret;
}

static bool scan_date(const char *p, int *YY, int *MM, int *DD) {
	int year  = scan_digits(p, 4);
	int month = (year >= 0  && p[4] == '-') ? scan_digits(p + 5, 2) : -1;
	int day   = (month >= 0 && p[7] == '-') ? scan_digits(p + 8, 2) : -1;
	if (YY)
		*YY = year;
	if (MM)
		*MM = month;
	if (DD)
		*DD = day;
	return (year >= 0 && month >= 0 && day >= 0);
}

static bool scan_time(const char *p, int *hh, int *mm, int *ss) {
	int hour   = scan_digits(p, 2);
	int minute = (hour >= 0 && p[2] == ':') ? scan_digits(p + 3, 2) : -1;
	int second = (minute >= 0 && p[5] == ':') ? scan_digits(p + 6, 2) : -1;
	if (hh)
		*hh = hour;
	if (mm)
		*mm = minute;
	if (ss)
		*ss = second;
	return (hour >= 0 && minute >= 0 && second >= 0);
}

static int parse_millisec(const char *p, const char **endp) {
	int ret  = 0;
	int unit = 100; /// unit in millisec
	for (; '0' <= *p && *p <= '9'; p++, unit /= 10)
		ret += (*p - '0') * unit;
	*endp = p;
	return ret;
}

static bool scan_offset(const char *p, int *tz) {
	int hour   = scan_digits(p, 2);
	int minute = (hour >= 0 && p[2] == ':') ? scan_digits(p + 3, 2) : -1;
	if (hour < -12 || hour > 14 || minute < 0 || minute > 59)
		return false;
	if (tz)
		*tz = hour*60 + minute;
	return true;
}

static int scan_string(context_t *ctx, char *p, toml_pos_t *pos, bool dotisspecial) {
	char *orig = p;

	// Literal multiline.
	if (strncmp(p, "'''", 3) == 0) {
		char *q = p + 3;
		pos->col += 3;
		while (true) {
			q = strstr(q, "'''");
			if (q == 0)
				return e_syntax(ctx, *pos, "unterminated triple quote (''')");
			int i = 0;
			while (q[3] == '\'') {
				i++;
				if (i >= 3)
					return e_syntax(ctx, *pos, "too many ''' in triple-s-quote");
				q++;
			}
			break;
		}
		set_token(ctx, MSTRING, *pos, orig, q + 3 - orig);
		return 0;
	}

	// Multiline.
	if (strncmp(p, "\"\"\"", 3) == 0) {
		char *q = p + 3;
		pos->col += 3;
		while (true) {
			q = strstr(q, "\"\"\"");
			if (q == 0)
				return e_syntax(ctx, *pos, "unterminated triple quote (\"\"\")");
			if (q[-1] == '\\') {
				q++;
				continue;
			}
			int i = 0;
			while (q[3] == '\"') {
				i++;
				if (i >= 3)
					return e_syntax(ctx, *pos, "too many \"\"\" in triple-d-quote");
				q++;
			}
			break;
		}

		/// the string is [p+3, q-1]
		int hexreq = 0; /// #hex required
		bool escape = false;
		for (p += 3; p < q; p++) {
			if (escape) {
				escape = false;
				if (strchr("btnfr\"\\", *p))
					continue;
				if (*p == 'u') {
					hexreq = 4;
					continue;
				}
				if (*p == 'U') {
					hexreq = 8;
					continue;
				}
				if (p[strspn(p, " \t\r")] == '\n')
					continue; // allow for line ending backslash
				return e_syntax(ctx, *pos, "bad escape char");
			}
			if (hexreq) {
				hexreq--;
				if (strchr("0123456789ABCDEFabcdef", *p))
					continue;
				return e_syntax(ctx, *pos, "expected hex char");
			}
			if (*p == '\\') {
				escape = true;
				continue;
			}
		}
		if (escape)
			return e_syntax(ctx, *pos, "expected an escape char");
		if (hexreq)
			return e_syntax(ctx, *pos, "expected more hex char");

		set_token(ctx, MSTRING, *pos, orig, q + 3 - orig);
		return 0;
	}

	// Literal string.
	if (*p == '\'') {
		for (p++; *p && *p != '\n' && *p != '\''; p++)
			pos->col++;
		if (*p != '\'')
			return e_syntax(ctx, *pos, "unterminated quote (')");
		set_token(ctx, STRING, *pos, orig, p + 1 - orig);
		return 0;
	}

	// Basic String.
	if (*p == '\"') {
		int hexreq = 0; /// #hex required
		bool escape = false;
		for (p++; *p; p++) {
			pos->col++;
			if (escape) {
				escape = false;
				if (strchr("btnfr\"\\", *p))
					continue;
				if (*p == 'u') {
					hexreq = 4;
					continue;
				}
				if (*p == 'U') {
					hexreq = 8;
					continue;
				}
				return e_syntax(ctx, *pos, "bad escape char");
			}
			if (hexreq) {
				hexreq--;
				if (strchr("0123456789ABCDEFabcdef", *p))
					continue;
				return e_syntax(ctx, *pos, "expected hex char");
			}
			if (*p == '\\') {
				escape = true;
				continue;
			}
			if (*p == '\n')
				break;
			if (*p == '"')
				break;
		}
		if (*p != '"')
			return e_syntax(ctx, *pos, "unterminated quote (\")");

		set_token(ctx, STRING, *pos, orig, p + 1 - orig);
		return 0;
	}

	// Time
	if (!dotisspecial && scan_time(p, 0, 0, 0)) {
		p += strspn(p, "0123456789:"); /// forward thru the time.
		if (p[0] == '.') { /// Subseconds
			int n = strspn(++p, "0123456789");
			if (n == 0)
				return e_syntax(ctx, *pos, "invalid text after '.'");
			p += n;
		}
		for (; p[-1] == ' '; p--) /// squeeze out any spaces at end of string
			;
		set_token(ctx, STRING, *pos, orig, p - orig); /// tokenize
		return 0;
	}

	// Datetime
	if (!dotisspecial && scan_date(p, 0, 0, 0)) {
		p += strspn(p, "0123456789-"); /// forward thru the date
		if (p[0] == ' ' || p[0] == 't' || p[0] == 'T') { /// forward thru the time
			p++;
			p += strspn(p, "0123456789:");
			if (p[0] == '.') { /// Subseconds
				int n = strspn(++p, "0123456789");
				if (n == 0)
					return e_syntax(ctx, *pos, "invalid text after '.'");
				p += n;
			}
		}

		// Offset
		if (p[0] == 'Z' || p[0] == 'z') {
			p++;
		} else if (p[0] == '+' || p[0] == '-') {
			if (!scan_offset(++p, 0))
				return e_syntax(ctx, *pos, "invalid offset");
			p += 5;
		}

		for (; p[-1] == ' '; p--) /// squeeze out any spaces at end of string
			;
		set_token(ctx, STRING, *pos, orig, p - orig); /// tokenize
		return 0;
	}

	// Literals
	for (; *p && *p != '\n'; p++) {
		int ch = *p;
		if (ch == '.' && dotisspecial)
			break;
		if ('A' <= ch && ch <= 'Z')
			continue;
		if ('a' <= ch && ch <= 'z')
			continue;
		if (strchr("0123456789+-_.", ch))
			continue;
		break;
	}

	set_token(ctx, STRING, *pos, orig, p - orig);
	return 0;
}

static int next_token(context_t *ctx, bool dotisspecial) {
	// Eat this tok.
	char *p = ctx->tok.ptr;
	toml_pos_t pos = ctx->tok.pos;
	for (int i = 0; i < ctx->tok.len; i++) {
		pos.col++;
		if (*p++ == '\n') {
			pos.line++;
			pos.col = 1;
		}
	}

	/// Make next tok
	while (p < ctx->stop) {
		if (*p == '#') { /// Skip comment. stop just before the \n.
			for (p++; p < ctx->stop && *p != '\n'; p++) {
				pos.col++;
				if ((*p != '\t' && *p != '\r' && *p != '\n') && ((*p >= 0x00 && *p <= 0x1f) || *p == 0x7f))
					return e_syntax(ctx, pos, "invalid control character");
				if (*p == '\r' && p < ctx->stop + 1 && *(p + 1) != '\n')
					return e_syntax(ctx, pos, "invalid control character");
			}
			continue;
		}

		if (dotisspecial && *p == '.') {
			set_token(ctx, DOT, pos, p, 1);
			return 0;
		}

		switch (*p) {
			case ',':
				set_token(ctx, COMMA, pos, p, 1);
				return 0;
			case '=':
				set_token(ctx, EQUAL, pos, p, 1);
				return 0;
			case '{':
				set_token(ctx, LBRACE, pos, p, 1);
				return 0;
			case '}':
				set_token(ctx, RBRACE, pos, p, 1);
				return 0;
			case '[':
				set_token(ctx, LBRACKET, pos, p, 1);
				return 0;
			case ']':
				set_token(ctx, RBRACKET, pos, p, 1);
				return 0;
			case '\n':
				set_token(ctx, NEWLINE, pos, p, 1);
				return 0;
			case '\r': case ' ': case '\t': /// ignore white spaces
				p++;
				pos.col++;
				continue;
		}

		return scan_string(ctx, p, &pos, dotisspecial);
	}

	set_eof(ctx, pos);
	return 0;
}

const char *toml_table_key(const toml_table_t *tbl, int keyidx, int *keylen) {
	if (keyidx < tbl->nkval) {
		*keylen = tbl->kval[keyidx]->keylen;
		return    tbl->kval[keyidx]->key;
	}
	if ((keyidx -= tbl->nkval) < tbl->narr) {
		*keylen = tbl->arr[keyidx]->keylen;
		return    tbl->arr[keyidx]->key;
	}
	if ((keyidx -= tbl->narr) < tbl->ntbl) {
		*keylen = tbl->tbl[keyidx]->keylen;
		return    tbl->tbl[keyidx]->key;
	}
	*keylen = 0;
	return 0;
}

toml_unparsed_t toml_table_unparsed(const toml_table_t *tbl, const char *key) {
	for (int i = 0; i < tbl->nkval; i++)
		if (strcmp(key, tbl->kval[i]->key) == 0)
			return tbl->kval[i]->val;
	return 0;
}

toml_array_t *toml_table_array(const toml_table_t *tbl, const char *key) {
	for (int i = 0; i < tbl->narr; i++)
		if (strcmp(key, tbl->arr[i]->key) == 0)
			return tbl->arr[i];
	return 0;
}

toml_table_t *toml_table_table(const toml_table_t *tbl, const char *key) {
	for (int i  = 0; i < tbl->ntbl; i++)
		if (strcmp(key, tbl->tbl[i]->key) == 0)
			return tbl->tbl[i];
	return 0;
}

toml_unparsed_t toml_array_unparsed(const toml_array_t *arr, int idx) {
	return (0 <= idx && idx < arr->nitem) ? arr->item[idx].val : 0;
}

int toml_table_len(const toml_table_t *tbl) {
	return tbl->nkval + tbl->narr + tbl->ntbl;
}

int toml_array_len(const toml_array_t *arr) {
	return arr->nitem;
}

toml_array_t *toml_array_array(const toml_array_t *arr, int idx) {
	return (0 <= idx && idx < arr->nitem) ? arr->item[idx].arr : 0;
}

toml_table_t *toml_array_table(const toml_array_t *arr, int idx) {
	return (0 <= idx && idx < arr->nitem) ? arr->item[idx].tbl : 0;
}

bool is_leap(int y) { return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0); }

int toml_value_timestamp(toml_unparsed_t src_, toml_timestamp_t *ret) {
	if (!src_)
		return -1;

	const char *p = src_;
	bool must_parse_time = false;

	memset(ret, 0, sizeof(*ret));

	/// YYYY-MM-DD
	if (scan_date(p, &ret->year, &ret->month, &ret->day)) {
		if (ret->month < 1 || ret->day < 1 || ret->month > 12 || ret->day > 31)
			return -1;
		if (ret->month == 2 && ret->day > (is_leap(ret->year) ? 29 : 28))
			return -1;
		ret->kind = 'D';

		p += 10;
		if (*p) {
			if (*p != 'T' && *p != 't' && *p != ' ') /// T or space
				return -1;
			must_parse_time = true;
			p++;
		}
	}

	/// HH:MM:SS
	if (scan_time(p, &ret->hour, &ret->minute, &ret->second)) {
		if (ret->second < 0 || ret->minute < 0 || ret->hour < 0 || ret->hour > 23 || ret->minute > 59 || ret->second > 60)
			return -1;
		ret->kind = (ret->kind == 'D' ? 'l' : 't');

		p += 8;
		if (*p == '.') { /// optionally, parse millisec
			p++; /// skip '.'
			const char *qq;
			ret->millisec = parse_millisec(p, &qq);
			p = qq;
		}

		if (*p) { /// parse and copy Z
			ret->kind = 'd';
			if (*p == 'Z' || *p == 'z')
				p++;
			else if (*p == '+' || *p == '-') {
				if (!scan_offset(p + 1, &ret->tz))
					return -1;
				p += 6;
			}
		}
	}
	if (*p != 0)
		return -1;
	if (must_parse_time && ret->kind == 'D')
		return -1;
	return 0;
}

// Raw to boolean
int toml_value_bool(toml_unparsed_t src, bool *ret_) {
	if (!src)
		return -1;
	bool dummy = false;
	bool *ret = ret_ ? ret_ : &dummy;

	if (strcmp(src, "true") == 0) {
		*ret = true;
		return 0;
	}
	if (strcmp(src, "false") == 0) {
		*ret = false;
		return 0;
	}
	return -1;
}

// Raw to integer
int toml_value_int(toml_unparsed_t src, int64_t *ret_) {
	if (!src)
		return -1;

	char buf[100];
	char *p = buf;
	char *q = p + sizeof(buf);
	const char *s = src;
	int64_t dummy = 0;
	int64_t *ret = ret_ ? ret_ : &dummy;
	bool have_sign = false;

	if (s[0] == '+' || s[0] == '-') { /// allow +/-
		have_sign = true;
		*p++ = *s++;
	}

	if (s[0] == '_') /// disallow +_100
		return -1;

	int base = 0;
	if (s[0] == '0') { /// if 0* ...
		switch (s[1]) {
			case 'x': base = 16; s += 2; break;
			case 'o': base = 8;  s += 2; break;
			case 'b': base = 2;  s += 2; break;
			case '\0':
				return *ret = 0, 0;
			default:
				if (s[1]) /// ensure no other digits after it
					return -1;
		}
		if (!*s)
			return -1;
		if (have_sign)   /// disallow +0xff, -0xff
			return -1;
		if (s[0] == '_') /// disallow 0x_, 0o_, 0b_
			return -1;
		if (s[0] == '+' || s[0] == '-') /// disallow 0x+10, 0x-10
			return -1;
	}

	while (*s && p < q) { /// just strip underscores and pass to strtoll
		int ch = *s++;
		if (ch == '_') {
			if (s[0] == '_')  /// disallow '__'
				return -1;
			if (s[0] == '\0') /// numbers cannot end with '_'
				return -1;
			continue; /// skip _
		}
		*p++ = ch;
	}

	if (*s || p == q) /// if not at end-of-string or we ran out of buffer ...
		return -1;

	*p = 0; /// cap with NUL

	/// Run strtoll on buf to get the integer
	char *endp;
	errno = 0;
	*ret = strtoll(buf, &endp, base);
	return (errno || *endp) ? -1 : 0;
}

int toml_value_double(toml_unparsed_t src, double *ret_) {
	if (!src)
		return -1;

	char buf[100];
	char *p = buf;
	char *q = p + sizeof(buf);
	const char *s = src;
	double dummy = 0.0;
	double *ret = ret_ ? ret_ : &dummy;

	if (s[0] == '+' || s[0] == '-') /// allow +/-
		*p++ = *s++;

	if (s[0] == '_') /// disallow +_1.00
		return -1;

	{ /// decimal point, if used, must be surrounded by at least one digit on each side
		char *dot = strchr(s, '.');
		if (dot) {
			if (dot == s || !isdigit(dot[-1]) || !isdigit(dot[1]))
				return -1;
		}
	}

	/// zero must be followed by . or 'e', or NUL
	if (s[0] == '0' && s[1] && !strchr("eE.", s[1]))
		return -1;

	/// Just strip underscores and pass to strtod
	bool have_us = false;
	while (*s && p < q) {
		int ch = *s++;
		if (ch == '_') {
			have_us = true;
			if (s[0] == '_') /// disallow '__'
				return -1;
			if (s[0] == 'e') /// disallow _e
				return -1;
			if (s[0] == 0)   /// disallow last char '_'
				return -1;
			continue; /// skip _
		}
		if (ch == 'I' || ch == 'N' || ch == 'F' || ch == 'A') /// inf and nan are case-sensitive.
			return -1;
		if (ch == 'e' && s[0] == '_') /// disallow e_
			return -1;
		*p++ = ch;
	}
	if (*s || p == q)
		return -1; /// reached end of string or buffer is full?

	*p = 0; /// cap with NUL

	/// Run strtod on buf to get the value
	char *endp;
	errno = 0;
	*ret = strtod(buf, &endp);
	if (errno || *endp)
		return -1;
	if (have_us && (isnan(*ret) || isinf(*ret)))
		return -1;
	return 0;
}

int toml_value_string(toml_unparsed_t src, char **ret, int *len) {
	bool multiline = false;
	const char *sp;
	const char *sq;

	*ret = 0;
	if (!src)
		return -1;

	/// First char must be a s-quote or d-quote
	int qchar = src[0];
	int srclen = strlen(src);
	if (!(qchar == '\'' || qchar == '"')) {
		return -1;
	}

	/// triple quotes?
	if (qchar == src[1] && qchar == src[2]) {
		multiline = true;      /// triple-quote implies multiline
		sp = src + 3;          /// first char after quote
		sq = src + srclen - 3; /// first char of ending quote

		if (!(sp <= sq && sq[0] == qchar && sq[1] == qchar && sq[2] == qchar))
			return -1; /// last 3 chars in src must be qchar

		if (sp[0] == '\n') /// skip new line immediate after qchar
			sp++;
		else if (sp[0] == '\r' && sp[1] == '\n')
			sp += 2;
	} else {
		sp = src + 1;          /// first char after quote
		sq = src + srclen - 1; /// ending quote
		if (!(sp <= sq && *sq == qchar)) /// last char in src must be qchar
			return -1;
	}

	/// at this point:
	///     sp points to first valid char after quote.
	///     sq points to one char beyond last valid char.
	///     string len is (sq - sp).
	if (qchar == '\'')
		*ret = norm_lit_str(sp, sq - sp, len, multiline, false, 0, 0);
	else
		*ret = norm_basic_str(sp, sq - sp, len, multiline, false, 0, 0);
	return *ret ? 0 : -1;
}

toml_value_t toml_array_string(const toml_array_t *arr, int idx) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_string(toml_array_unparsed(arr, idx), &ret.u.s, &ret.u.sl) == 0);
	return ret;
}

toml_value_t toml_array_bool(const toml_array_t *arr, int idx) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_bool(toml_array_unparsed(arr, idx), &ret.u.b) == 0);
	return ret;
}

toml_value_t toml_array_int(const toml_array_t *arr, int idx) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_int(toml_array_unparsed(arr, idx), &ret.u.i) == 0);
	return ret;
}

toml_value_t toml_array_double(const toml_array_t *arr, int idx) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_double(toml_array_unparsed(arr, idx), &ret.u.d) == 0);
	return ret;
}

toml_value_t toml_array_timestamp(const toml_array_t *arr, int idx) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_timestamp(toml_array_unparsed(arr, idx), &ret.u.ts) == 0);
	return ret;
}

toml_value_t toml_table_string(const toml_table_t *tbl, const char *key) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	toml_unparsed_t raw = toml_table_unparsed(tbl, key);
	if (raw)
		ret.ok = (toml_value_string(raw, &ret.u.s, &ret.u.sl) == 0);
	return ret;
}

toml_value_t toml_table_bool(const toml_table_t *tbl, const char *key) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_bool(toml_table_unparsed(tbl, key), &ret.u.b) == 0);
	return ret;
}

toml_value_t toml_table_int(const toml_table_t *tbl, const char *key) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_int(toml_table_unparsed(tbl, key), &ret.u.i) == 0);
	return ret;
}

toml_value_t toml_table_double(const toml_table_t *tbl, const char *key) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_double(toml_table_unparsed(tbl, key), &ret.u.d) == 0);
	return ret;
}

toml_value_t toml_table_timestamp(const toml_table_t *tbl, const char *key) {
	toml_value_t ret;
	memset(&ret, 0, sizeof(ret));
	ret.ok = (toml_value_timestamp(toml_table_unparsed(tbl, key), &ret.u.ts) == 0);
	return ret;
}
#endif // TOML_H
