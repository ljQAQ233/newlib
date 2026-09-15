# autoconf / automake 简单入门

这一篇讲构建脚本怎么生成, 面向没接触过 autotools 的读者. 例子都用这个仓库里的写法, 读完能看懂 `configure.ac`, `Makefile.am`, `configure.host` 三者的关系.

## 三个工具各管什么

Autotools 是一组工具, 核心是三个:

- **autoconf** - 读 `configure.ac`, 生成 `configure` (一个 shell 脚本). 它的工作是检测系统环境: 有没有某个编译器, 有没有某个头文件, 类型多大.
- **automake** - 读 `Makefile.am`, 生成 `Makefile.in` (一个 makefile 模板). 它的工作是展开 `xxx_SOURCES` 这类简写, 补全依赖规则, 生成 `make install`, `make clean` 等标准目标.
- **aclocal** - 收集 `*.m4` 宏文件, 生成 `aclocal.m4`, 供 autoconf 使用. 项目自己的宏通常放在 `acinclude.m4`, 由 aclocal 一并收进来.

`autoreconf` 是一个包装脚本, 按正确顺序调用上面这些工具, 一步到位重新生成所有产物.

## 一次构建的完整链路

```
configure.ac  --autoconf-->  configure
Makefile.am   --automake-->  Makefile.in
                             |
                             v
              configure  +  Makefile.in  -->  Makefile  (运行时生成)
```

也就是说, 维护者写的是 `configure.ac` 和 `Makefile.am`, 提交进仓库的是 `configure` 和 `Makefile.in`. 最终用户跑 `./configure` 时才结合两者生成能用的 `Makefile`.

## configure.ac 最小骨架

```m4
AC_INIT([foo], [1.0])
AC_CONFIG_SRCDIR([src/main.c])
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
```

几个要素:

- `AC_INIT` - 包名和版本
- `AC_CONFIG_SRCDIR` - 用某个源文件确认源码目录对
- `AC_PROG_CC` - 检测 C 编译器, 设置 `CC` 变量
- `AC_CONFIG_FILES` - 列出要从 `*.in` 生成的 `Makefile` 等文件
- `AC_OUTPUT` - 收尾

自定义宏用 `AC_DEFUN([名字], [宏体])` 定义, 例如 newlib 的 `NEWLIB_CONFIGURE`.

## Makefile.am 最小骨架

```makefile
bin_PROGRAMS = hello
hello_SOURCES = main.c util.c
```

约定是 `<primary>_<目标>` 这种命名:

- `bin_PROGRAMS` - 要安装到 bin 目录的可执行文件
- `hello_SOURCES` - 目标 `hello` 的源文件列表

automake 会据此生成编译 `hello` 的规则, 以及默认的 `all`, `install`, `clean` 等目标. 静态库用 `lib_LIBRARIES`, 头文件用 `include_HEADERS`, 以此类推.

条件编译用 `AM_CONDITIONAL` 配合 `if` 块:

```makefile
if HAVE_SYS_FOO_DIR
include foo/Makefile.inc
endif
```

`if HAVE_SYS_FOO_DIR` 的真假由 `configure.ac` 里的 `AM_CONDITIONAL([HAVE_SYS_FOO_DIR], [test ...])` 决定. 这就是 newlib 里 `HAVE_LIBC_SYS_XXX_DIR` 那套条件的来历.

## 和 configure 里的变量对接

automake 生成的 `Makefile.in` 里会留下 `@变量@` 占位符, `configure` 运行时把它们替换成实际值. 例如 `@CC@` 换成编译器路径, `@prefix@` 换成安装前缀. 所以 `configure.host` 里设置的 `sys_dir`, `machine_dir` 这类变量, 最终都会通过 `AC_SUBST` 传进 `Makefile`.

## newlib 里的对应

对照这个仓库:

- `newlib/configure.ac` 调 `NEWLIB_CONFIGURE`, 后者 source `newlib/configure.host`
- `newlib/configure.host` 按目标三元组设置 `sys_dir`, `machine_dir`, `newlib_cflags` 等
- `newlib/libc/acinclude.m4` 里用 `m4_foreach_w` 为每个 OS/架构目录生成一个 `AM_CONDITIONAL`
- `newlib/libc/sys/<os>/Makefile.inc` 声明该 OS 的 `libc_a_SOURCES`

`Makefile.inc` 不是 automake 原生概念, 而是 newlib 自己用 `include` 把源文件清单拆散到各目录的约定.

## 版本要固定

autoconf 和 automake 的新版本会改变生成结果. newlib 明确 pin 在 autoconf 2.69 / automake 1.15.1, 换版本会重写 `configure` / `Makefile.in` 产生大量无关 diff, 甚至破坏构建 (automake 1.16 改了 per-target 对象命名). 所以重新生成时务必用对版本:

```
autoreconf -f
```

在 `newlib/` 目录里执行即可.

---

本文档由 deepseek-v4-pro 撰写.
