{ pkgs }:

pkgs.runCommand "x86_64-unknown-textos-toolchain" { } ''
  mkdir -p $out/bin
  ln -s ${pkgs.gcc-unwrapped}/bin/gcc $out/bin/x86_64-unknown-textos-gcc
  ln -s ${pkgs.gcc-unwrapped}/bin/gcc $out/bin/x86_64-unknown-textos-cc
  ln -s ${pkgs.gcc-unwrapped}/bin/g++ $out/bin/x86_64-unknown-textos-g++
  for t in ar as ld ranlib nm objcopy objdump strip readelf; do
    ln -s ${pkgs.binutils-unwrapped}/bin/$t $out/bin/x86_64-unknown-textos-$t
  done
''
