#!/usr/bin/env bash

tt=
if [ -x "./toml-test" ]; then
	tt="./toml-test"
elif command -v "toml-test" >/dev/null; then
	tt="toml-test"
elif [ -n "$(go env GOBIN)" ] && [ -x "$(go env GOBIN)/toml-test" ]; then
	tt="$(go env GOPATH)/toml-test"
elif [ -n "$(go env GOPATH)" ] && [ -x "$(go env GOPATH)/bin/toml-test" ]; then
	tt="$(go env GOPATH)/bin/toml-test"
elif [ -x "$HOME/go/bin/toml-test" ]; then
	tt="$HOME/go/bin/toml-test"
fi

if ! command -v "$tt" >/dev/null; then
	echo >&2 'toml-test not in PATH, ./, $GOBIN, $GOPATH/bin, or $HOME/go/bin; install with:'
	echo >&2 '    % go install github.com/toml-lang/toml-test/cmd/toml-test@latest'
	echo >&2
	echo >&2 'Or download a binary from:'
	echo >&2 '    https://github.com/toml-lang/toml-test/releases'
	exit 1
fi

toml2json=
if [ -e "./toml2json.exe" ]; then
	toml2json='./toml2json.exe'
elif [ -e "./toml2json" ]; then
	toml2json=./toml2json
else
	echo >&2 "./toml2json or ./toml2json.exe doesn't exist; run 'make'"
	exit 1
fi

failing=(
	-skip invalid/array/extending-table         # Appending existing tables
	-skip invalid/table/append-with-dotted-keys-1
	-skip invalid/table/append-with-dotted-keys-2
	-skip invalid/inline-table/overwrite-02
	-skip invalid/control/comment-null          # Stops parsing on first NUL
	-skip invalid/encoding/utf16-key
	-skip invalid/encoding/utf16-comment
	-skip invalid/encoding/bad-utf8-in-comment  # Doesn't parse UTF-8 in comments.
	-skip invalid/encoding/bad-codepoint
	-skip invalid/control/bare-null             # Doesn't reject some invalid CC
	-skip invalid/control/bare-cr
	-skip invalid/control/rawmulti-cr
	-skip invalid/control/multi-cr
	-skip invalid/control/only-null

	-skip invalid/inline-table/trailing-comma  # Trailing comma should be error; not worth fixing as it'll be allowed in 1.1
)

"$tt" "$toml2json" ${failing[@]} "$@"
