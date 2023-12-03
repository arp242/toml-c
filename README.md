C library for parsing TOML 1.0. Passes all of [TOML test suite].

This is a fork of https://github.com/cktan/tomlc99, as that didn't seem hugely
maintained, and has a number of errors. This library isn't compatible.

**Note: API is likely to change in the future!**

[TOML test suite]: https://github.com/toml-lang/toml-test

Installation
------------
This can be used in two ways: as a library or in "header mode":

- For "header mode" just copy `header/toml-c.h` to you project and `#include
  "toml-c.h"`.

- For "library mode" build `libtoml.so.1.0` and `libtoml.a` by just typing `make`.

Testing
-------
Run `make check` to run the tests; this requires [toml-test] to be in $PATH.

[toml-test]: https://github.com/toml-lang/toml-test
