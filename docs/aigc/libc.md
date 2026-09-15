# libc 结构

libc 是 newlib 的核心, 编译出 `libc.a`. 它的源码散在 `newlib/libc/` 下十几个子目录里, 但最终会汇聚成一个归档文件. 这里说明这些目录怎么分工, 它们之间是什么关系, 以及为什么会有 `libc/syscalls/syskill.c` 这种文件.

## 目录布局

```
newlib/libc/
  stdio/        printf, scanf, fopen 等标准 I/O
  stdlib/       malloc, exit, atexit, 环境变量
  string/       memcpy, strcpy 等字符串函数
  ctype/        字符分类
  search/       排序, 查找, 哈希
  reent/        可重入 (_r) 版本, 以及 struct _reent 的操作
  syscalls/     connector 层, 见下文
  sys/<os>/     系统层, 每个 OS 一个目录
  machine/<arch>/  架构相关, 如 setjmp.S, memcpy.S
  posix/ unix/  额外的 POSIX / Unix 接口
  time/ signal/ locale/ errno/ 等
```

前五个目录是纯算法, 不碰硬件. `reent/`, `syscalls/`, `sys/<os>/` 三层是 newlib 与操作系统打交道的核心. `machine/<arch>/` 是性能优化和架构必备代码.

## 怎么构建成 libc.a

所有目录的源文件通过各自的 `Makefile.inc` 声明, 最终汇总到 automake 的 `libc_a_SOURCES` 变量里. 构建时:

1. 每个 `.c` 编译成 `libc_a-<名字>.o` 这样的 per-target 对象 (前缀 `libc_a-` 用于和 libm 的对象区分)
2. `ar` 把这些对象归档成 `libc.a`
3. 特殊的 `MATHOBJS_IN_LIBC` 机制: `ldexp`, `frexp`, `modf` 等函数传统上既在 `libc.a` 也在 `libm.a` 里. 它们只在 libm 里编译一次, 归档时再从 `libm.a` 提取出来塞进 `libc.a`

哪些源文件参与构建由 `configure.host` 决定: 它设置 `sys_dir`, `machine_dir`, `syscall_dir` 等变量, 对应 `libc/sys/<os>/`, `libc/machine/<arch>/`, `libc/syscalls/` 是否编进来.

## 文件之间的关系: 三层调用链

newlib 把每一个依赖系统的函数拆成三层, 每层是一个独立的 `.c` 文件. 以 `kill` 为例:

```
用户调用 kill(pid, sig)
   |
   v
libc/syscalls/syskill.c          connector 层
   kill() -> _kill_r(_REENT, pid, sig)
   |
   v
libc/reent/signalr.c             可重入层
   _kill_r() -> _kill(pid, sig)
   (处理 errno 传播)
   |
   v
libc/sys/<os>/syscalls.c         系统层
   _kill() -> 真正的系统调用
```

每一层的职责不同:

- **connector 层** (`syscalls/syskill.c`): 提供干净的公共 API. 它做两件事: 一是把函数名从公共的 `kill` 接到下划线的 `_kill_r`, 二是插入 `_REENT` 指针
- **可重入层** (`reent/signalr.c`): 负责 errno 的按线程传递. 它先清 errno, 调用底层, 再把结果写回 `_REENT_ERRNO(ptr)`
- **系统层** (`sys/<os>/`): 唯一真正发系统调用的地方, 移植时只写这一层

其它函数遵循同样的模式, 对应关系一目了然: `sysread.c` -> `reent/readr.c` -> `_read`, `sysopen.c` -> `reent/openr.c` -> `_open`, 以此类推.

## 为什么需要 syscalls/ 这一层

`syscalls/syskill.c` 看起来多余: 为什么不能直接让用户调 `_kill_r`, 省掉一个文件?

原因有三个:

1. **公共 API 必须没有下划线和 `_r` 后缀.** 标准规定用户写 `kill(pid, sig)`, 而 newlib 内部的可重入约定要求 `_kill_r(reent, ...)`. connector 就是这两个命名约定之间的桥梁

2. **抹平差异.** 有些函数原型有可变参数. 例如 `open` 的 mode 参数只在带 `O_CREAT` 时才出现, `sysopen.c` 用 `va_arg` 把可变的公共原型收束成固定的 `_open_r(reent, path, flags, mode)`

3. **目标可以覆盖.** 如果某个目标自己实现了 `_kill_r` (定义了 `REENTRANT_SYSCALLS_PROVIDED`), 那 `reent/signalr.c` 里的默认 `_kill_r` 就不会编译, 而 connector 不需要改, 照样把 `kill` 接到 `_kill_r`. 同样, 如果目标只提供无下划线的 `kill` (定义了 `MISSING_SYSCALL_NAMES`), 头文件里的宏会把 `_kill` 映射成 `kill`, connector 层依然原样工作

所以 `syscalls/` 这一层的作用是隔离命名约定和原型的差异, 让可重入层和系统层都能各自独立地替换, 而不影响用户看到的接口.

---

本文档由 deepseek-v4-pro 撰写.
