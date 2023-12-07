C library for parsing TOML 1.0. Passes all of [TOML test suite].

This is a fork of https://github.com/cktan/tomlc99, as that didn't seem hugely
maintained, and has a number of errors. This library isn't compatible.

**Note: API is likely to change in the future!**

[TOML test suite]: https://github.com/toml-lang/toml-test

Installation
------------
This can be used in two ways: as a library or in "header only mode":

- "Header only mode" is to make it a bit easier to include this in a project:
  just copy `header/toml-c.h` to your project and `#include <toml-c.h>` and
  you're done – nothing else needed.

  It's essentially just "cat toml.h toml.c > toml-c.h" wuth a bit of frobbing.

- For "library mode" build `libtoml.so.1.0` and `libtoml.a` by just typing `make`.

Usage
-----
See `toml.h` and `examples`.

Testing
-------
Run `make check` to run the tests; this requires [toml-test] to be in $PATH.

[toml-test]: https://github.com/toml-lang/toml-test
