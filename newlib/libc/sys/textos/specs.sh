incdir=$1
libdir=$3
ldso=$4

cat <<EOF
*cc1:
%(cc1_cpu) -nostdinc $(for d in $incdir; do printf -- "-isystem %s " "$d"; done)

*lib:
$(for d in $libdir; do printf -- "-L %s " "$d"; done)

*link_libgcc:
%{!nostdlib:-lc} -L .%s

*libgcc:
libgcc.a%s

*startfile:
%{!shared:$libdir/crt1.o} $libdir/crti.o

*endfile:
%{!shared:$libdir/crtn.o}
*link:
-dynamic-linker $ldso -nostdlib %{shared:-shared} %{static:-static} %{rdynamic:-export-dynamic}

*esp_link:
*esp_options:

EOF

