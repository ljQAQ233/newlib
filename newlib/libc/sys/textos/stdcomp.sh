#!/bin/sh

file="$1"

: ${cmdcc:=gcc}
: ${commit:="59d235ffa5a69231eb42e5290d52dc8c90d28b7a"}

function testit
{
    hdr=$1
    tmp=$(mktemp --suffix .c)
    trap 'rm -f "'$tmp'"' EXIT
    echo "#include \"$hdr\"" > $tmp
    "$cmdcc" -E "$tmp" >/dev/null 2>&1
}

locpath=$("$cmdcc" -print-file-name="include/${file}")
if [[ -f "$locpath" ]] ; then
  if testit "$locpath"; then
    exec cat "$locpath"
  fi
fi

timeout 10 \
  curl https://raw.githubusercontent.com/gcc-mirror/gcc/${commit}/gcc/ginclude/${file} || exit 1

