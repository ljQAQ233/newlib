#!/bin/sh

cmdcc="$1"

function testit
{
    hdr=$1
    tmp=$(mktemp --suffix .c)
    trap 'rm -f "'$tmp'"' EXIT
    echo "#include \"$hdr\"" > $tmp
    "$cmdcc" -E "$tmp" >/dev/null 2>&1
}

stddef=$("$cmdcc" -print-file-name="include/stddef.h")
if [[ -f "$stddef" ]] ; then
  if testit "$stddef"; then
    exec cat "$stddef"
  fi
fi

timeout 10 \
  curl https://raw.githubusercontent.com/gcc-mirror/gcc/59d235ffa5a69231eb42e5290d52dc8c90d28b7a/gcc/ginclude/stddef.h || exit 1
