C library for parsing TOML 1.0. Passes all of [TOML test suite].

This is a fork of https://github.com/cktan/tomlc99, as that didn't seem hugely
maintained, and has a number of errors.

[TOML test suite]: https://github.com/toml-lang/toml-test

Installation
------------
This can be used in two ways: as a library or in "header mode":

- For "header mode" just copy toml-c.h to you project and `#include "toml-c.h"`.

- For "library mode" build `libtoml.so.1.0` and `libtoml.a` by just typing `make`.


Usage
-----
Please see the `toml.h` file for details. The following is a simple example that
parses this config file:

```toml
[server]
	host = "www.example.com"
	port = [ 8080, 8181, 8282 ]
```

These are the usual steps for getting values from a file:

1. Parse the TOML file.
2. Traverse and locate a table in TOML.
3. Extract values from the table.
4. Free up allocated memory.

Below is an example of parsing the values from the example table.

    #include <stdio.h>
    #include <string.h>
    #include <errno.h>
    #include <stdlib.h>
    #include "toml.h"

    static void error(const char* msg, const char* msg1)
    {
        fprintf(stderr, "ERROR: %s%s\n", msg, msg1?msg1:"");
        exit(1);
    }


    int main()
    {
        FILE* fp;
        char errbuf[200];

        // 1. Read and parse toml file
        fp = fopen("sample.toml", "r");
        if (!fp) {
            error("cannot open sample.toml - ", strerror(errno));
        }

        toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
        fclose(fp);

        if (!conf) {
            error("cannot parse - ", errbuf);
        }

        // 2. Traverse to a table.
        toml_table_t* server = toml_table_in(conf, "server");
        if (!server) {
            error("missing [server]", "");
        }

        // 3. Extract values
        toml_datum_t host = toml_string_in(server, "host");
        if (!host.ok) {
            error("cannot read server.host", "");
        }

        toml_array_t* portarray = toml_array_in(server, "port");
        if (!portarray) {
            error("cannot read server.port", "");
        }

        printf("host: %s\n", host.u.s);
        printf("port: ");
        for (int i = 0; ; i++) {
            toml_datum_t port = toml_int_at(portarray, i);
            if (!port.ok) break;
            printf("%d ", (int)port.u.i);
        }
        printf("\n");

        // 4. Free memory
        free(host.u.s);
        toml_free(conf);
        return 0;
    }

#### Accessing Table Content

TOML tables are dictionaries where lookups are done using string keys. In
general, all access functions on tables are named `toml_*_in(...)`.

In the normal case, you know the key and its content type, and retrievals can be done
using one of these functions:

    toml_string_in(tab, key);
    toml_bool_in(tab, key);
    toml_int_in(tab, key);
    toml_double_in(tab, key);
    toml_timestamp_in(tab, key);
    toml_table_in(tab, key);
    toml_array_in(tab, key);

You can also interrogate the keys in a table using an integer index:

    toml_table_t* tab = toml_parse_file(...);
    for (int i = 0; ; i++) {
        const char* key = toml_key_in(tab, i);
        if (!key) break;
        printf("key %d: %s\n", i, key);
    }

#### Accessing Array Content

TOML arrays can be deref-ed using integer indices. In general, all access methods on arrays are named `toml_*_at()`.

To obtain the size of an array:

    int size = toml_array_nelem(arr);

To obtain the content of an array, use a valid index and call one of these functions:


    toml_string_at(arr, idx);
    toml_bool_at(arr, idx);
    toml_int_at(arr, idx);
    toml_double_at(arr, idx);
    toml_timestamp_at(arr, idx);
    toml_table_at(arr, idx);
    toml_array_at(arr, idx);

#### toml_datum_t

Some `toml_*_at` and `toml_*_in` functions return a toml_datum_t
structure. The `ok` flag in the structure indicates if the function
call was successful. If so, you may proceed to read the value
corresponding to the type of the content.

For example:

    toml_datum_t host = toml_string_in(tab, "host");
    if (host.ok) {
        printf("host: %s\n", host.u.s);
        free(host.u.s);   /* FREE applies to string and timestamp types only */
    }

** IMPORTANT: if the accessed value is a string or a timestamp, you must call
`free(datum.u.s)` or `free(datum.u.ts)` respectively after usage. **

Testing
-------
Run `make check` to run the tests; this requires [toml-test] to be in $PATH.
