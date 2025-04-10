#include <stdlib.h>
#include <string.h>
#include "toml.h"

int main() {
       char errbuf[200];
       toml_table_t* tbl = toml_parse("str = 'xxx'", errbuf, sizeof(errbuf));
       if (!tbl) {
               fprintf(stderr, "ERROR: %s\n", errbuf);
               return 1;
       }

       int fail = 0;

       // Unknown value
       toml_value_t unknown = toml_table_string(tbl, "unknown");
       if (unknown.ok) {
               fprintf(stderr, "toml-c-test: unknown.ok set\n");
               fail = 1;
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

       if (!fail)
               printf("toml-c-test: okay\n");
       return fail;
}
