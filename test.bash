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
if [ -x "./toml2json" ]; then
	toml2json=./toml2json
elif [ -x "toml2json.exe" ]; then
	toml2json=./toml2json.exe
else
	echo >&2 "./toml2json or ./toml2json.exe doesn't exist; run 'make'"
	exit 1
fi

failing=(
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
	-skip invalid/encoding/bad-utf8-in-comment
	-skip invalid/encoding/bad-codepoint

	-skip invalid/encoding/utf16
	-skip invalid/encoding/utf16-comment
	-skip invalid/encoding/utf16-key

	-skip invalid/inline-table/trailing-comma  # Trailing comma should be error; not worth fixing as it'll be allowed in 1.1
)

"$tt" "$toml2json" ${failing[@]} "$@"
