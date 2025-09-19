#!/usr/bin/env bash
# Also compatible with zsh, but not POSIX sh.
#
# Run the toml-test compliance tests: https://github.com/toml-lang/toml-test

decoder="./toml2json"
toml=1.0.0

# Skip known failures.
skip=(
    # Extending existing tables
    -skip='invalid/array/extending-table'
    -skip='invalid/table/append-with-dotted-keys-01'
    -skip='invalid/table/append-with-dotted-keys-02'
    -skip='invalid/inline-table/overwrite-02'

    # Encoding
    -skip='invalid/encoding/bad-codepoint'
    -skip='invalid/encoding/bad-utf8-in-comment'
    -skip='invalid/encoding/utf16-comment'
    -skip='invalid/encoding/utf16-key'

    # Allows "invalid" control characters
    -skip='invalid/control/bare-cr'
    -skip='invalid/control/bare-null'
    -skip='invalid/control/comment-null'
    -skip='invalid/control/multi-cr'
    -skip='invalid/control/only-null'
    -skip='invalid/control/rawmulti-cr'

    # TOML 1.1
    -skip='invalid/inline-table/trailing-comma'

    # Newly failing tests
    -skip='invalid/table/append-with-dotted-keys-1'
    -skip='invalid/table/append-with-dotted-keys-2'
)

# Find toml-test
tt=
if [[ -x "./toml-test" ]]; then
	tt="./toml-test"
elif command -v "toml-test" >/dev/null; then
	tt="toml-test"
elif [[ -n "$(go env GOBIN)" ]] && [[ -x "$(go env GOBIN)/toml-test" ]]; then
	tt="$(go env GOPATH)/toml-test"
elif [[ -n "$(go env GOPATH)" ]] && [[ -x "$(go env GOPATH)/bin/toml-test" ]]; then
	tt="$(go env GOPATH)/bin/toml-test"
elif [[ -x "$HOME/go/bin/toml-test" ]]; then
	tt="$HOME/go/bin/toml-test"
fi
if ! command -v "$tt" >/dev/null; then
	echo >&2 'toml-test not in current dir, $PATH, $GOBIN, $GOPATH/bin, or $HOME/go/bin; install with:'
	echo >&2 '    % go install github.com/toml-lang/toml-test/cmd/toml-test@latest'
	echo >&2
	echo >&2 'Or download a binary from:'
	echo >&2 '    https://github.com/toml-lang/toml-test/releases'
	exit 1
fi

"$tt" -toml="$toml" -skip-must-err ${skip[@]} "$@" -- "$decoder"
