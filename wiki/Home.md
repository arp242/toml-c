Welcome to the documentation for the toml-c library. It is highly suggested you read the Official TOML documentation before reading through this wiki.

Examples
--------

Examples are presented in the `example` directory of the repo. Bellow you will find an explanation of the examples, where we go line by line, explaining exactly what is happening.

### array.c
**line 1:** Here we include the -c.h version of the library header. We could've also included the plain .h version and included the separate .c file when compiling.
**lines 3-10:** Here we write our TOML so we can parse it using `toml_parse()`.
**lines 14-18:** Here we parse the file and set the root table pointer as `*tbl`.
**lines 21-26:** Here we parse the integer array `ints = [1, 2, 3]` and printing it to the console. Using `toml_table_array(tbl, "ints")`, to get the pointer to the array with the key `ints` and `toml_array_len(arr)`, to get the length of the array (here 3).
**lines 29-51:** Here we parse the mixed array `mixed = [1, 'one', 1.2]`, printing the value of each element and its type. This relies mostly on the `toml_value_t` struct's `ok` field to check if the datatype we have tried to parse the nth index of the array as, matches its datatype. If we have parsed it as the wrong datatype `ok` will return false and thus the correct conditional will execute.
**lines 54-61:** Here we parse the array of tables `[[aot]]`, printing the value belonging to `k` in each table. Note here that the string value of key `k` is taken by accessing the `s` field of the union `u`.

### table.c
**lines 1-18:** See the explanation of `array.c` since they're practically identical up to this point.
**lines 21-27:** Here we obtain the values of specific Value/Key pairs, using `toml_table_string(tbl, "host")` and `toml_table_int(tbl, "port")`. We then give some default values (for if they aren't parsed {LINES 23-26}), following that up with a basic print of the values. *Note: This is how you can assign defaults for app configs.*
**line 30:** Here we are getting a sub table of `tbl`, of key `tbl` which we name `sub_tbl` with `toml_table_table(tbl, "tbl")`. We are then looping over all the keys in the table printing every one of them to the terminal.

Custom Types
------------

1. **toml_table_t** is the struct used to define tables.
Its fields are:
    - `char* key`, a string that stores the key for the table.
    - `int keylen`, the length of the key.
    - `bool implicit`, whether or not the table was created implicitly.
    - `bool readonly`, whether or nor further modification is allowed.
    - `int nkval`, the number of key/value pairs in the table.
    - `toml_keyval_t** kval`, pointer to key/value pairs in the table.
    - `int narr`, number of arrays in the table.
    - `toml_array_t** arr`, pointer to arrays in the table.
    - `int ntbl`, number of nested tables.
    - `toml_table_t** tbl`, pointer to nested tables.

2. **toml_array_t** is the struct used to define arrays.
Its fields are:
    - `char* key`, a string that stores the key for this array.
    - `int keylen`, the length of the key.
    - `int kind`, the kind of elements the array has. **'v'** is used for values, **'a'** for arrays, **'t'** for tables, **'m'** for mixed.
    - `int type`, the kind of values we have.**'i'** is used for integer values, **'d'** is used for double values, **'s'** is used for string values, **'t'** is used for time, **'D'** for date, **'T'** is used for timestamps and **'m'** is used for mixed value types.
    - `int nitem`, the number of elements in the array.
    - `toml_arritem_t* item`, a pointer to an item in the array, the first by default.

3. **toml_arritem_t** is the struct used to define an item in array.
Its fields are:
    - `int valtype`, similar to the `type` field of `toml_array_t`, it holds the value kind for this item.
    - `char* val`, the value of the Key/Value pair.
    - `toml_array_t* arr`, pointer that points to a nested array (if it exists).
    - `toml_table_t* tbl`, pointer that points to the nested table (if it exists).

4. **toml_keyval_t** is the struct used to define a Key/Value pair.
Its fields are:
    - `char* key`, the Key of this pair.
    - `int keylen`, the length of the key.
    - `char* val`, the raw value.

5. **toml_timestamp_t** is the struct used to define a timestamp.
Its fields are:
    - `char kind`, the type of timestamp we have. Options are: **'d'** for datetime (Full date + time + timezone), **'l'** for local datetime (Full date+ time but without timezone), **'D'** for local date (Date only, no timezone), **'t'** for local time (Time onlu, no timezone).
    - `int year`, the year.
    - `int month`, the month.
    - `int day`, the day.
    - `int hour`, 24-hour clock hour.
    - `int minute`, minutes.
    - `int second`, seconds.
    - `int millisec`, milliseconds.
    - `int tz`, the timezone offset in minutes.
*NOTE:One or more values may be empty, depending on what value `kind` takes.*

6. **toml_value_t** is the struct used to represent all parsed values.
Its fields are:
    - `bool ok`, is true if the value was present when we tried to parse it.
    - `union  u`, the union that holds all the actual values.
        - `char* s`, a string value. It **MUST** be freed after use.
        - `int sl`, the length of the string (null-byte terminator excluded).
        - `toml_timestamp_t ts`, the parsed timestamp.
        - `bool b`, the parsed bool.
        - `int64_t i`, the parsed integer.
        - `double d`, the parsed double.


Defined Functions
-----------------

### General Functions

- `toml_parse(char* toml, char* errbuf, int errbufsz);` is one of two parsing functions. It parses a TOML document from a string containing the entirety of the TOML. Should everything work out, it will return the root TOML table, should an error occur, the function will Return 0, and the error message will be stored in the errbuf, with errbufsz holding the buffer's size.

- `toml_parse_file(FILE* fp, char* errbuf, int errbufsz);` is the other parsing function. It behaves exactly like its counterpart `toml_parse`, however instead of parsing from a string, it parses from a file pointer. When using this you should check if the file pointer is null externaly (a.k.a. check if file was accessed with fopen), since no checks happen internaly.

- `toml_free(toml_table_t* table);` Should be used to free the return value. Doing so will invalidate all handles for `table`.

### Array Functions

- `toml_array_len(toml_array_t* array);` returns the length of `array`.
- `toml_array_string(toml_array_t* array, int idx);` returns the string present at the index `idx` as part of a union (So to actually get a `char*` you would need to do `toml_array_string(toml_array_t* array, int idx).u.s;`). The length of the string can be taken with `.u.sl`.
- Similar functions exist to return other data types such as: bool, int, double (with the respective `toml_array_*datatype*(toml_array_t* array, int idx);`).
- `toml_array_timestamp(toml_array_t* array, int idx);` returns the timestamp present at the index `idx`. See `toml_value_t` for an explanation of the way toml-c handles timestamps.
- `toml_array_array(toml_array_t* array, int idx);` returns a pointer to the nested array present at index `idx`.
- `toml_array_table(toml_array_t* array, int idx);` returns a pointer to the table present at index `idx`.

### Table Functions

- `toml_table_len(toml_table_t* table);` returns the number of keys this table possesses.
- `toml_table_key(toml_table_t* table, int keyidx, int* keylen);` returns the `keyidx`'th direct key in `table`. Keylen is a pass-by-refernece int variable that stores the length of the key. If you do not need the key's length pass `NULL` as the parameter.
- `toml_table_string(toml_table_t* table, char* key);` returns the string value from the `keyidx`/*string_to_be_returned* Key/Value pair as part of the toml_value_t union (So to actually get a `char*` you would need to do `toml_table_string(toml_table_t* table, char* key).u.s;`).
- Similar functions exist to return other data types such as: bool, int, double (with the respective `toml_table_*datatype*(toml_array_t* array, int idx);`).
- `toml_table_timestamp(toml_table_t* table, char* key);` returns the timestamp from the `key`/`timestamp_to_be_returned` Key/Value pair. See the toml_timestamp_t and toml_value_t sections (Under Custom Types) for an explanation of the way toml-c handles timestamps.
- `toml_table_array(toml_table_t* table, char* key);` returns a pointer to the array nested within the `table` whose Key matches `key`.
- `toml_table_table(toml_table_t* table, char* key);` returns a pointer to the nested table whose Key matches `key`.
