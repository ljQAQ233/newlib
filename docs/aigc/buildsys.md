# 构建系统组成

这个仓库的构建系统分两层: 顶层是一个 GNU 组合树的 `configure` (与 GCC 共享), 它负责把 `--target` 分发给下面的子项目; 每个子项目 (newlib, libgloss, Cygwin) 又有自己的一套 autoconf/automake 脚本.

## 顶层

根目录的 `configure`, `configure.ac`, `Makefile.in` 是 GNU 工具链的组合树脚本, 与 GCC 仓库保持一致 (改动要通知 binutils/gdb, 见 `MAINTAINERS`). 它的作用是根据 `--target` 决定递归配置哪些子目录: `newlib/`, `libgloss/`, 以及目标为 Cygwin 时的 `winsup/`.

配套的 `config.sub` 和 `config.guess` 也是 GNU 共享文件, 负责校验和规范化目标三元组. 新 OS 名要先加进 `config.sub` 才会被接受.

## newlib

newlib 的构建输入是三类文件, 分工如下:

### 生成脚本 (autoconf / automake 的输入)

- `newlib/configure.ac` - 顶层 autoconf 脚本, 由 autoconf 2.69 生成 `configure`
- `newlib/acinclude.m4` - newlib 自己的 autoconf 宏, 核心是 `NEWLIB_CONFIGURE`, 它调用 `AC_CANONICAL_HOST` 并 source `configure.host`
- `newlib/Makefile.am` - 顶层 automake 模板, 由 automake 1.15.1 生成 `Makefile.in`

### per-target 配置

`newlib/configure.host` 是一个纯 shell 脚本, 在 configure 运行时被直接 source (不是生成物). 它按目标三元组设置变量, 决定构建哪些目录:

- `machine_dir` / `libm_machine_dir` - 架构相关目录
- `sys_dir` - 系统层目录
- `syscall_dir` - 是否构建 connector
- `newlib_cflags` - 目标特性开关

因为它是纯 shell, 改动它不用重新运行 autoconf, 立即生效.

### 源文件清单

各目录的 `Makefile.inc` 声明要编译的源文件, 被顶层 `Makefile.am` 通过 `include` 拉进来. 例如:

- `newlib/libc/Makefile.inc` 和 `newlib/libm/Makefile.inc` - libc/libm 主体
- `newlib/libc/sys/Makefile.inc` - 汇总各 OS 目录, 用 `if HAVE_LIBC_SYS_XXX_DIR` 条件包含
- `newlib/libc/sys/<os>/Makefile.inc` - 单个 OS 的源文件 (`libc_a_SOURCES += ...`)
- `newlib/libc/machine/<arch>/Makefile.inc` - 单个架构的源文件

`newlib/libc/acinclude.m4` 里有一张 `SYS_DIR` 和 `MACHINE` 的列表, 用 `m4_foreach_w` 为每个目录生成一个 `AM_CONDITIONAL` (即 `HAVE_LIBC_SYS_XXX_DIR` 之类的条件). 新增一个 OS 或架构目录要同时改这张列表和对应的 `Makefile.inc`.

### 生成物

`newlib/configure`, `newlib/Makefile.in`, `newlib/aclocal.m4` 是生成物, 但会提交进 git. 改完上面任意输入后, 在 `newlib/` 目录里跑 `autoreconf` (autoconf 2.69 + automake 1.15.1) 重新生成.

## Cygwin (winsup)

Cygwin 与 newlib 走不同的生成方式:

- `winsup/configure.ac` 是 autoconf 输入
- `winsup/autogen.sh` 硬编码调用 `/usr/bin/aclocal`, `autoconf -f`, `automake -ac`
- `winsup/Makefile.am` 及各子目录的 `Makefile.am` 声明目标

与 newlib 相反, Cygwin 的生成物 (`winsup/configure`, `winsup/**/Makefile.in`, `winsup/aclocal.m4`) 被 gitignore, 不提交. 修改后跑 `winsup/autogen.sh` 本地重新生成.

## 构建流程

构建在源码树外进行:

```
mkdir build && cd build
../configure --target=<triple> --prefix=$PWD/install
make
make install
```

顶层 `configure` 探测目标工具链 (gcc, ar, as, ld 等), 递归配置子目录, 生成各自的 `Makefile`. `make` 默认开启 silent rules, 用 `make V=1` 看真实命令. newlib 默认开启 multilib, 会为每个 ABI 变体各构建一份库.

---

本文档由 deepseek-v4-pro 撰写.
