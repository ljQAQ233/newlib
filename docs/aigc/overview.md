# 项目架构

这个仓库是一个 sourceware 组合树, 它包含三个共享同一份顶层 `configure` 的项目 (GNU 组合树 configure, 与 GCC 保持同步):

- `newlib/` - 面向裸机/嵌入式目标的 C 库 (`libc`, `libm`)
- `libgloss/` - 板级支持包: 启动代码, 系统调用存根, 链接脚本
- `winsup/` - Cygwin, Windows 上的 POSIX 层 (`cygwin1.dll`, 工具, 测试)

前两者与 Cygwin 基本独立, 大部分改动只会落在 `newlib/` 或者 `winsup/` 其中之一.

## newlib

newlib 让裸机程序在没有操作系统的情况下也能使用标准 C. 它的设计目标是把平台差异压缩到最小的一个地方, 因此移植一个新目标基本就是写一个目录的系统调用存根.

### 目录布局

```
newlib/libc/
  stdio/ stdlib/ string/ ctype/ search/   与平台无关的实现
  reent/                                  可重入 (_r) 版本
  syscalls/                               系统调用 connector
  sys/<os>/                               系统层, 每个 OS 一个目录
  machine/<arch>/                         各架构的汇编/优化
  posix/ unix/ time/ signal/ locale/ errno/
newlib/libm/
  math/ common/ complex/ fenv/ ...        数学库
newlib/doc/
  makedoc.c doc.str                       手册页生成器
```

### 三层调用链

newlib 把每个依赖系统调用的函数拆成三层. `read` 是最典型的例子:

```
read(1, buf, n)              libc/syscalls/sysread.c   (connector)
  -> _read_r(_REENT, ...)    libc/reent/readr.c        (可重入层)
       -> _read(fd, buf, n)  libc/sys/<os>/syscalls.c  (系统层)
```

connector 提供干净的公共 API, 并吸收可变参数 (例如 `open` 可选的 mode 参数). 可重入层负责按线程传递 errno. 系统层是唯一与硬件或内核打交道的层, 也是移植时唯一要写的部分.

### 可重入性

每线程状态 (errno, stdio 缓冲区) 放在 `struct _reent` 里. 每个 `_r` 函数都把它的指针作为第一个参数. 单线程构建时 `_impure_ptr` 指向静态的 `impure_data`, 多线程时指向线程局部存储. 如果目标自己提供了 `_read_r` 等函数, 就定义 `REENTRANT_SYSCALLS_PROVIDED` 来跳过 `reent/` 里的默认实现.

### 移植需要提供的最小集合

`libc/include/_syslist.h` 列出了 newlib 期望系统层提供的下划线函数: `_exit`, `_open`, `_close`, `_read`, `_write`, `_lseek`, `_fstat`, `_stat`, `_isatty`, `_sbrk`, `_kill`, `_getpid`, `_times`, `_gettimeofday`, `_unlink`, `_link`, `_fork`, `_wait`, `_execve`, `_fcntl`.

### 配置

`newlib/configure.host` 是 per-target 的特性矩阵. 它是一个纯 shell 脚本 (改动无需重新运行 autoconf), 负责把目标三元组映射到:

- `machine_dir` / `libm_machine_dir` - 使用哪些架构目录
- `sys_dir` - 使用哪个系统层目录
- `syscall_dir` - 是否构建 `syscalls/` 里的 connector
- `newlib_cflags` - 目标特性开关 (`-DMISSING_SYSCALL_NAMES`, ...)

`config.sub` / `config.guess` 是 GNU 共享文件, 新 OS 名必须先加进 `config.sub` 才会被接受.

## 构建系统

构建在源码树外进行:

```
mkdir build && cd build
../configure --target=<triple> --prefix=$PWD/install
make
make install
```

`newlib/configure` 和 `newlib/Makefile.in` 会提交进 git; 修改 `configure.ac`, `acinclude.m4` 或任意 `Makefile.am` / `Makefile.inc` 之后要用 `autoreconf` (autoconf 2.69, automake 1.15.1) 重新生成.

## Cygwin (winsup)

Cygwin 是另一个世界. `winsup/cygwin/` 主要是 C++, 编译时禁用异常和 RTTI, 并开启 `-Werror`. 它产出 `cygwin1.dll` 和导入库; 导出的符号声明在 `cygwin.din` 里, DLL 版本号在 `winsup/cygwin/include/cygwin/version.h`.

与 newlib 不同, Cygwin 的 `configure` 和 `Makefile.in` 不提交进 git. 它们由 `winsup/autogen.sh` 生成, 且被 gitignore.

Cygwin 只支持 `x86_64-pc-cygwin` 目标, 需要 mingw-w64 交叉工具链 (或用 `--with-cross-bootstrap`), 以及 docbook/xmlto (或用 `--disable-doc`). 它的测试只能在 Windows 上原生运行.

## 许可证

许可证是混合的. newlib 和 libgloss 使用逐文件的许可证 (`COPYING.NEWLIB`); Cygwin 是 LGPL (`winsup/CYGWIN_LICENSE`).

---

本文档由 deepseek-v4-pro 撰写.
