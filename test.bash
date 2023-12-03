#!/usr/bin/env bash

if ! command -v toml-test >/dev/null; then
	echo >&2 'toml-test not in PATH; install with:'
	echo >&2 '    % go install github.com/toml-lang/toml-test/cmd/toml-test@latest'
	echo >&2
	echo >&2 'Or download a binary from:'
	echo >&2 '    https://github.com/toml-lang/toml-test/releases'
	exit 1
fi
if [ ! -f ./toml2json ]; then
	echo >&2 "./toml2json doesn't exist; run 'make'"
	exit 1
fi

failing=(
	-skip valid/key/quoted-unicode             # Doesn't deal with NUL byte in keys.

	-skip invalid/string/literal-multiline-quotes-1
	-skip invalid/string/literal-multiline-quotes-2
	-skip invalid/string/multiline-quotes-1
	-skip invalid/inline-table/trailing-comma  # Trailing comma should be error; not worth fixing as it'll be allowed in 1.1
	-skip invalid/inline-table/add             # Appending existing tables
	-skip invalid/array/extending-table
	-skip invalid/table/append-with-dotted-keys-1
	-skip invalid/table/append-with-dotted-keys-2
	-skip invalid/inline-table/overwrite-2
	-skip invalid/control/bare-cr              # Doesn't reject some forbidden control characters.
	-skip invalid/control/bare-null
	-skip invalid/control/comment-cr
	-skip invalid/control/comment-del
	-skip invalid/control/comment-lf
	-skip invalid/control/comment-null
	-skip invalid/control/comment-us
	-skip invalid/control/rawmulti-cd
	-skip invalid/control/multi-cr
	-skip invalid/control/comment-ff
	-skip invalid/encoding/bad-codepoint       # Doesn't reject invalid UTF-8; nothing is multi-byte aware, so...
	-skip invalid/encoding/bad-utf8-in-comment
	-skip invalid/encoding/bad-utf8-in-multiline-literal
	-skip invalid/encoding/bad-utf8-in-multiline
	-skip invalid/encoding/bad-utf8-in-string-literal
	-skip invalid/encoding/bad-utf8-in-string
	-skip invalid/encoding/utf16
	-skip invalid/datetime/feb-29              # Doesn't reject invalid Feb dates
	-skip invalid/datetime/feb-30
	-skip invalid/local-date/feb-29
	-skip invalid/local-date/feb-30
	-skip invalid/local-datetime/feb-29
	-skip invalid/local-datetime/feb-30
)

toml-test ./toml2json ${failing[@]} "$@"
