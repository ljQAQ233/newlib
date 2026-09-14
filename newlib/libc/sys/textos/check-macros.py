#!/usr/bin/env python3
##
# @file
# @brief Compare macros defined by both the textos and newlib C libraries.
# @author deepseek-v4-flash
##

"""Compare macros defined by both the textos and newlib C libraries.

For every object-like macro that both header sets define, the macro is
evaluated with the compiler on each side and the resulting integer values
are compared.  A mismatch is reported as an error unless the macro is in
the allow list (--allow), which is empty by default.

The allow list is for differences that are deliberately handled by the
textos compatibility layer rather than by a header override; add names
there when such a case appears.

Values are obtained by generating a small translation unit per side that
casts every candidate macro to ``long long`` and compiling it to assembly
with the matching header set.  Macros that are not integer constant
expressions (strings, function pointers, ...) cannot be evaluated and are
reported separately rather than compared.

Usage:

    check-macros.py \
        --textos /path/to/textos/src/include \
        --textos /path/to/textos/src/include/arch/x86_64 \
        --newlib /path/to/newlib-install/x86_64-unknown-textos/include

Exit status is 0 when no unexpected mismatch is found, 1 otherwise.
"""

import argparse
import concurrent.futures
import os
import re
import subprocess
import sys

DEFAULT_HEADERS = [
    "errno.h",
    "fcntl.h",
    "signal.h",
    "unistd.h",
    "sys/stat.h",
    "sys/types.h",
    "sys/mman.h",
    "sys/wait.h",
    "sys/time.h",
    "sys/times.h",
    "sys/ioctl.h",
    "sys/select.h",
    "sys/socket.h",
    "sys/un.h",
    "sys/utsname.h",
    "sys/resource.h",
    "poll.h",
    "dirent.h",
    "termios.h",
    "netinet/in.h",
    "arpa/inet.h",
    "net/if.h",
]

# Macros whose difference is expected and handled by the textos
# compatibility layer.  Empty now that the headers carry the textos/Linux
# values; add names here (or pass --allow) if a difference is deliberate.
DEFAULT_ALLOW = set()

DEF_RE = re.compile(r"#define\s+([A-Za-z_]\w*)(\([^)]*\))?\s*(.*)")
PROBE_LABEL = "__check_macros_values"


def run(cmd, **kwargs):
    return subprocess.run(cmd, capture_output=True, text=True, **kwargs)


def cc_base(cc, dirs, compiler_include):
    cmd = [cc, "-nostdinc"]
    for d in dirs:
        cmd += ["-isystem", d]
    if compiler_include:
        cmd += ["-isystem", compiler_include]
    return cmd


def existing_headers(dirs, headers):
    found = []
    for h in headers:
        for d in dirs:
            if os.path.isfile(os.path.join(d, h)):
                found.append(h)
                break
    return found


def usable_headers(cc, dirs, compiler_include, headers):
    """Drop headers that cannot be preprocessed on this side."""
    usable = []
    dropped = []
    for h in headers:
        cmd = cc_base(cc, dirs, compiler_include) + ["-E", "-include", h, "-"]
        if run(cmd, input="").returncode == 0:
            usable.append(h)
        else:
            dropped.append(h)
    return usable, dropped


def dump_macros(cc, dirs, compiler_include, headers):
    cmd = cc_base(cc, dirs, compiler_include) + ["-dM", "-E"]
    for h in headers:
        cmd += ["-include", h]
    cmd += ["-"]
    proc = run(cmd, input="")
    if proc.returncode != 0:
        sys.stderr.write(proc.stderr)
        raise SystemExit("macro dump failed")
    macros = {}
    for line in proc.stdout.splitlines():
        m = DEF_RE.match(line)
        if not m:
            continue
        name, args, body = m.group(1), m.group(2), m.group(3).strip()
        if name.startswith("__"):
            continue
        macros[name] = (bool(args), body)
    return macros


def _eval_batch(cc, dirs, compiler_include, headers, names, result):
    if not names:
        return
    src = "".join("#include <%s>\n" % h for h in headers)
    src += "long long %s[] = {\n" % PROBE_LABEL
    src += ",\n".join("((long long)(%s))" % n for n in names)
    src += "\n};\n"
    cmd = cc_base(cc, dirs, compiler_include) + [
        "-S",
        "-o",
        "-",
        "-O0",
        "-fno-asynchronous-unwind-tables",
        "-fno-unwind-tables",
        "-fno-stack-protector",
        "-x",
        "c",
        "-",
    ]
    proc = run(cmd, input=src)
    values = None
    if proc.returncode == 0:
        m = re.search(
            r"%s:\s*\n((?:\s*\.quad\s+[^\n]+\n)+)" % re.escape(PROBE_LABEL),
            proc.stdout,
        )
        if m:
            raw = re.findall(r"\.quad\s+([^\s]+)", m.group(1))
            if len(raw) == len(names):
                values = [int(x, 0) for x in raw]
    if values is None:
        if len(names) == 1:
            result[names[0]] = None
            return
        mid = len(names) // 2
        _eval_batch(cc, dirs, compiler_include, headers, names[:mid], result)
        _eval_batch(cc, dirs, compiler_include, headers, names[mid:], result)
        return
    for name, value in zip(names, values):
        result[name] = value


def eval_side(cc, dirs, compiler_include, headers, names):
    result = {}
    _eval_batch(cc, dirs, compiler_include, headers, names, result)
    return result


def parse_args():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--textos", action="append", required=True, metavar="DIR",
                   help="textos include directory (repeatable)")
    p.add_argument("--newlib", action="append", required=True, metavar="DIR",
                   help="newlib include directory (repeatable)")
    p.add_argument("--cc", default="gcc", help="compiler to use (default: gcc)")
    p.add_argument("--compiler-include", metavar="DIR",
                   help="compiler builtin include dir (default: ask --cc)")
    p.add_argument("--headers", action="append", metavar="H",
                   help="headers to include (default: a broad set)")
    p.add_argument("--allow", action="append", default=[], metavar="NAME",
                   help="macro whose difference is expected (repeatable)")
    p.add_argument("--no-default-allow", action="store_true",
                   help="do not seed --allow with the built-in open flags")
    p.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 4,
                   help="parallel compiler jobs")
    return p.parse_args()


def main():
    args = parse_args()

    compiler_include = args.compiler_include
    if compiler_include is None:
        compiler_include = run([args.cc, "-print-file-name=include"]).stdout.strip()
        if not os.path.isdir(compiler_include):
            compiler_include = None

    headers = args.headers or DEFAULT_HEADERS
    textos_headers = existing_headers(args.textos, headers)
    newlib_headers = existing_headers(args.newlib, headers)
    candidate = [h for h in headers if h in textos_headers and h in newlib_headers]
    tx_ok, tx_drop = usable_headers(args.cc, args.textos, compiler_include, candidate)
    nl_ok, nl_drop = usable_headers(args.cc, args.newlib, compiler_include, candidate)
    common_headers = [h for h in candidate if h in tx_ok and h in nl_ok]
    dropped = sorted(set(tx_drop) | set(nl_drop))
    if not common_headers:
        raise SystemExit("no usable header present in both include sets")

    allow = set() if args.no_default_allow else set(DEFAULT_ALLOW)
    allow.update(args.allow)

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        fut_t = pool.submit(dump_macros, args.cc, args.textos, compiler_include,
                            common_headers)
        fut_n = pool.submit(dump_macros, args.cc, args.newlib, compiler_include,
                            common_headers)
        tx_macros = fut_t.result()
        nl_macros = fut_n.result()

    common = sorted(set(tx_macros) & set(nl_macros))
    obj = [n for n in common if not tx_macros[n][0] and not nl_macros[n][0]]
    func = [n for n in common if n not in obj]

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        fut_t = pool.submit(eval_side, args.cc, args.textos, compiler_include,
                            common_headers, obj)
        fut_n = pool.submit(eval_side, args.cc, args.newlib, compiler_include,
                            common_headers, obj)
        tx_vals = fut_t.result()
        nl_vals = fut_n.result()

    mismatches = []
    skipped = []
    allowed = []
    for name in obj:
        tv, nv = tx_vals.get(name), nl_vals.get(name)
        if tv is None or nv is None:
            skipped.append(name)
            continue
        if tv == nv:
            continue
        entry = (name, nv, tv)
        if name in allow:
            allowed.append(entry)
        else:
            mismatches.append(entry)

    print("headers compared: %s" % ", ".join(common_headers))
    if dropped:
        print("headers skipped (unusable): %s" % ", ".join(dropped))
    print("common macros: %d object-like, %d function-like"
          % (len(obj), len(func)))
    print()

    if allowed:
        print("allowed differences (%d):" % len(allowed))
        for name, nv, tv in allowed:
            print("  %-20s newlib=%-14s textos=%s" % (name, nv, tv))
        print()

    if skipped:
        print("not integer constant expressions, not compared (%d):" % len(skipped))
        print("  " + " ".join(skipped))
        print()

    if func:
        print("function-like macros (not compared) (%d):" % len(func))
        print("  " + " ".join(func))
        print()

    if mismatches:
        print("VALUE MISMATCHES (%d):" % len(mismatches))
        for name, nv, tv in mismatches:
            print("  %-20s newlib=%-14s textos=%s" % (name, nv, tv))
        return 1

    print("no unexpected value mismatches")
    return 0


if __name__ == "__main__":
    sys.exit(main())
