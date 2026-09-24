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

stdarg=$("$cmdcc" -print-file-name="include/stdarg.h")
if [[ -f "$stdarg" ]] ; then
  if testit "$stdarg"; then
    exec cat "$stdarg"
  fi
fi

timeout 10 \
  curl https://raw.githubusercontent.com/gcc-mirror/gcc/59d235ffa5a69231eb42e5290d52dc8c90d28b7a/gcc/ginclude/stdarg.h || exit 1
