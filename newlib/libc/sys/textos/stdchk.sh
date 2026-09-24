#!/bin/sh

cmdcc="$1"
hdr=$2

tmp=$(mktemp --suffix .c)
trap 'rm -f "'$tmp'"' EXIT
echo "#include \"$hdr\"" > $tmp
"$cmdcc" -E "$tmp" >/dev/null 2>&1
