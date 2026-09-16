# porting

这一节说明移植流程

你可以将 下面所说 `textos` 换成 **你自己的 os** 名字

## config.sub

> config.sub 是 autotool 构建系统中的一个辅助脚本, 用于规范化 目标平台的名称.

总而言之, 在这一堆系统中加上我们的大名 (/へ＼*)

```diff
--- a/config.sub
+++ b/config.sub
@@ -1741,7 +1741,7 @@ case $os in
             | uxpv* | beos* | mpeix* | udk* | moxiebox* \
             | interix* | uwin* | mks* | rhapsody* | darwin* \
             | openstep* | oskit* | conix* | pw32* | nonstopux* \
-            | storm-chaos* | tops10* | tenex* | tops20* | its* \
+            | storm-chaos* | textos* | tops10* | tenex* | tops20* | its* \
             | os2* | vos* | palmos* | uclinux* | nucleus* | morphos* \
```

## configure.host

```diff
--- a/newlib/configure.host
+++ b/newlib/configure.host
@@ -564,6 +564,10 @@ case "${host}" in
        default_newlib_io_long_long="yes"
        default_newlib_atexit_dynamic_alloc="no"
        ;;
+  *-*-textos)
+       sys_dir=textos
+       have_crt0="no"
+       ;;
   tic6x*)
        newlib_cflags="${newlib_cflags} -DMISSING_SYSCALL_NAMES"
        have_crt0="no"
@@ -879,6 +883,9 @@ newlib_cflags="${newlib_cflags} -DCLOCK_PROVIDED -DMALLOC_PROVIDED -DEXIT_PROVID
        syscall_dir=syscalls    
        newlib_cflags="${newlib_cflags} -DSMALL_DTOA -DSMALL_MEMORY"
        ;;
+  *-*-textos)
+       syscall_dir=syscalls
+       ;;
```

## newlib/libc/sys/Makefile.inc

```diff
--- a/newlib/libc/sys/Makefile.inc
+++ b/newlib/libc/sys/Makefile.inc
@@ -55,6 +55,9 @@ endif
 if HAVE_LIBC_SYS_SYSVNECV70_DIR
 include %D%/sysvnecv70/Makefile.inc
 endif
+if HAVE_LIBC_SYS_TEXTOS_DIR
+include %D%/textos/Makefile.inc
+endif
 if HAVE_LIBC_SYS_TIC80_DIR
 include %D%/tic80/Makefile.inc
 endif
```

## newlib/libc/sys/textos/Makefile.inc

这个需要自己创建, 用来描述这个 target 有什么文件, 怎么编译, 怎么安装

```makefile
libc_a_SOURCES += %D%/syscalls.c
```

## newlib/libc/sys/textos/syscalls.c

这时候就需要写 适配层 了, 需要至少实现:

`libc/include/_syslist.h`
- `_exit`
- `_open`
- `_close`
- `_read`
- `_write`
- `_lseek`
- `_fstat`
- `_stat`
- `_isatty`
- `_sbrk`
- `_kill`
- `_getpid`
- `_times`
- `_gettimeofday`
- `_unlink`
- `_link`
- `_fork`
- `_wait`
- `_execve`
- `_fcntl`.

除了带个下划线, 与原来的系统调用原型没有区别.

这还说什么, 直接重定向不就好了 (・・*)ノ ⌒ξ
