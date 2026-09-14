{
  description = "TextOS newlib forked from RedHat";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    {
      self,
      nixpkgs,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };

      textosToolchain = pkgs.runCommand "x86_64-unknown-textos-toolchain" { } ''
        mkdir -p $out/bin
        ln -s ${pkgs.gcc-unwrapped}/bin/gcc $out/bin/x86_64-unknown-textos-gcc
        ln -s ${pkgs.gcc-unwrapped}/bin/gcc $out/bin/x86_64-unknown-textos-cc
        ln -s ${pkgs.gcc-unwrapped}/bin/g++ $out/bin/x86_64-unknown-textos-g++
        for t in ar as ld ranlib nm objcopy objdump strip readelf; do
          ln -s ${pkgs.binutils-unwrapped}/bin/$t $out/bin/x86_64-unknown-textos-$t
        done
      '';
    in
    {
      packages.${system} = {
        default = textosToolchain;
        x86_64-unknown-textos-toolchain = textosToolchain;
      };

      devShells.${system}.default = pkgs.mkShellNoCC {
        nativeBuildInputs = with pkgs; [
          stdenv
          gnumake
          patchelf
          gcc
          binutils
          gdb
          perl
          # for check-macros.py
          python3Packages.python
          self.packages.${system}.x86_64-unknown-textos-toolchain
        ];
      };
    };
}
