# AGENTS.md

Combined sourceware tree containing three distinct projects sharing a top-level
GNU combined-tree `configure` (kept in sync with GCC):

- `newlib/` — embedded C library (`libc`, `libm`), plus `libgloss/` board support
- `winsup/` — Cygwin, the Windows POSIX layer (`cygwin1.dll` + utils + tests)

Licensing is mixed: newlib/libgloss use per-file licenses (`COPYING.NEWLIB`),
Cygwin is LGPL (`winsup/CYGWIN_LICENSE`).

## Build

Builds are out-of-tree. Cygwin only targets `x86_64-pc-cygwin` (configure.ac
errors on anything else) and requires a mingw-w64 cross toolchain unless you pass
`--with-cross-bootstrap`, plus `docbook2texi`/`xmlto` unless `--disable-doc`.

```
mkdir build && cd build
../configure --target=x86_64-pc-cygwin --prefix=$PWD/install
make            # silent rules by default; use make V=1 for real commands
make install
```

newlib for an embedded target is built the same way, e.g.
`../configure --target=arm-none-eabi && make` (requires a prebuilt gcc/binutils
for that target). `newlib/configure.host` is the per-target feature matrix; edit
it when adding a target.

## Regenerating configure / Makefiles

Two different mechanisms — do NOT mix them up:

- `newlib/`: run `autoreconf` inside `newlib/`. Generated `configure` and
  `Makefile.in` are **committed** to git.
- `winsup/`: run `./autogen.sh` inside `winsup/` (hardcodes `/usr/bin/aclocal`,
  `autoconf -f`, `automake -ac`). Generated `winsup/configure`,
  `winsup/**/Makefile.in`, `winsup/aclocal.m4` are **gitignored** — never commit
  them.

Newlib's README documents autoconf 2.69 / automake 1.15.1 as the pinned
versions.

## Commits

No ChangeLog entries (see `MAINTAINERS`). Commit subjects use a component
prefix, e.g. `Cygwin:`, `newlib:`, `<arch>:`, `<func>:`. Cygwin feature/bugfix
commits normally also add a `winsup/cygwin/release/<version>` "release message"
entry summarizing the user-visible change.

## Tests

- Cygwin: `make check` in `<build>/x86_64-pc-cygwin/winsup/`. This only works
  natively on an NT machine (cross-checking is unsupported) and needs
  `cygserver` running. Select tests with
  `make check TESTS="winsup.api/ltp/umask03 ..."`; build-only via
  `make -C winsup/testsuite check_programs`. New tests go under
  `winsup/testsuite/winsup.api/` and must be added to `check_PROGRAMS` in
  `winsup/testsuite/Makefile.am`. Tests exit 0 = pass, 77 = skip.
- newlib: `make check-target-newlib` in the top build dir, with `DEJAGNU`
  pointing at a `site.exp` defining `target_list` (see `newlib/README`).

## Gotchas

- Cygwin compiles with `-Werror` (`winsup/cygwin/Makefile.am`); warnings fail
  the build.
- `winsup/cygwin/` is mostly C++ (`.cc`) with C++ exceptions/RTTI disabled
  (`-fno-rtti -fno-exceptions`).
- Cygwin version lives in `winsup/cygwin/include/cygwin/version.h`
  (`CYGWIN_VERSION_DLL_MAJOR/MINOR`); exported symbols in
  `winsup/cygwin/cygwin.din`.
- The top-level `configure.ac`/`Makefile.in` are shared with GCC; keep changes
  in sync there and notify binutils/gdb lists (see `MAINTAINERS`).
- CI in `.github/workflows/cygwin.yml` is the authoritative reference for the
  full Cygwin build+test flow (Fedora cross-build and native Windows build).
  The Windows job sets `core.autocrlf input` to preserve LF line endings.

## textos port (local-only, not for upstream)

A hobby x86_64 OS (`~/textos`) is supported as target `x86_64-unknown-textos`.
The syscall layer lives in `newlib/libc/sys/textos/` and speaks textos'
Linux-x86_64-compatible syscall ABI (`syscall` instr, errno = `-ret`, numbers
from textos `bits/syscall.h`). `struct stat` and `open()` flags differ from
newlib's, so `syscalls.c` marshals both.

Building needs no cross toolchain — symlink host gcc/binutils to the target
prefix, then configure/make as usual:

```
for t in gcc cc g++ ar as ld ranlib nm objcopy objdump strip readelf; do
  ln -sf "$(which $t)" /path/to/tc/bin/x86_64-unknown-textos-$t
done
PATH=/path/to/tc/bin:$PATH ../configure --target=x86_64-unknown-textos --prefix=...
```

Pitfalls (hard-won, do not repeat):

- **automake must be 1.15.1** (newlib's pinned version). automake 1.16 changes
  per-target object naming `libm_a-*.o` → `a-*.o`, which breaks newlib's
  hard-coded `MATHOBJS_IN_LIBC` and fails the `libc.a` archive step. nix only
  ships 1.16.x; build 1.15.1 from GNU source (perl, quick) and use
  `PATH=<automake115>/bin:$PATH nix-shell -p autoconf269 --run "autoreconf -f"`.
- **`config.sub` must recognize `textos`** — already patched (added `textos*`
  to the OS list). Without it configure rejects the triple.
- **`configure` / `Makefile.in` are not committed** (autoconf/automake output
  is kept out of git). Regenerate before building with
  `PATH=<automake115>/bin:$PATH nix-shell -p autoconf269 --run "autoreconf -f"`
  inside `newlib/`. Note: autoreconf will drop the stale `libc/sys/xtensa`
  entries that upstream forgot to regenerate away — harmless, that directory
  no longer exists. `acinclude.m4` / `libc/sys/Makefile.inc` list textos at the
  *end* of their SYS_DIR/include blocks so the generated textos entries land at
  the end without renumbering.

## Writing docs

Docs live under `docs/` as Markdown, written in Chinese. Keep them plain and
readable:

- Keyboard-printable characters only: no arrows, emoji, or other special
  symbols
- English punctuation (`,`, `.`, `:`, ...), not full-width Chinese punctuation
- Plain prose, no filler, no AI-flavored phrasing
- End each file with a line stating the model that wrote it, e.g.
  `本文档由 deepseek-v4-pro 撰写.`
