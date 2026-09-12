/*
 * System call stubs for the textos operating system.
 *
 * textos exposes a Linux x86_64 compatible syscall ABI (the `syscall`
 * instruction, syscall number in %rax, args in %rdi/%rsi/%rdx/%r10/%r8/%r9,
 * return value in %rax where -4095..-1 encodes -errno).  The syscall numbers
 * follow Linux x86_64 with a few textos-specific additions in the 5xx range
 * (see <bits/syscall.h> in the textos source tree).
 *
 * These are the non-reentrant (_-prefixed) primitives that newlib's
 * reentrant layer (libc/reent) and connectors (libc/syscalls) build on.
 */

#include <_ansi.h>
#include <errno.h>
#include <fcntl.h>
#include <reent.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* textos syscall numbers (Linux x86_64 compatible).  */
#define SYS_read 0
#define SYS_write 1
#define SYS_open 2
#define SYS_close 3
#define SYS_stat 4
#define SYS_fstat 5
#define SYS_lseek 8
#define SYS_brk 12
#define SYS_ioctl 16
#define SYS_getpid 39
#define SYS_fork 57
#define SYS_execve 59
#define SYS_exit 60
#define SYS_wait4 61
#define SYS_kill 62
#define SYS_fcntl 72
#define SYS_rename 82
#define SYS_unlink 87
#define SYS_gettimeofday 96
#define SYS_times 100
#define SYS_time 201

/* ioctl request for isatty, must match textos <bits/ioctl.h>.  */
#define TIOCGWINSZ 0x5413

/* textos open(2) flag values, from <bits/fcntl.h>.  These are *not* the
   same bit layout as newlib's O_* constants, so open flags must be
   translated.  */
#define T_O_CREAT 0x40        /* 0100  */
#define T_O_EXCL 0x80         /* 0200  */
#define T_O_TRUNC 0x200       /* 01000 */
#define T_O_APPEND 0x400      /* 02000 */
#define T_O_NONBLOCK 0x800    /* 04000 */
#define T_O_DIRECTORY 0x20000 /* 0200000 */
#define T_O_NOFOLLOW 0x40000  /* 0400000 */

static inline long
__syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
  long ret;
  register long r10 __asm__("r10") = a4;
  register long r8 __asm__("r8") = a5;
  register long r9 __asm__("r9") = a6;

  __asm__ volatile(
    "syscall"
    : "=a"(ret)
    : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10), "r"(r8), "r"(r9)
    : "rcx", "r11", "memory");
  return ret;
}

#define __syscall0(n) __syscall6(n, 0, 0, 0, 0, 0, 0)
#define __syscall1(n, a1) __syscall6(n, a1, 0, 0, 0, 0, 0)
#define __syscall2(n, a1, a2) __syscall6(n, a1, a2, 0, 0, 0, 0)
#define __syscall3(n, a1, a2, a3) __syscall6(n, a1, a2, a3, 0, 0, 0)
#define __syscall4(n, a1, a2, a3, a4) __syscall6(n, a1, a2, a3, a4, 0, 0)

/* Convert a raw syscall result into the C library convention: -1 with
   errno set on error, the value itself on success.  */
static long
__sysret(long ret)
{
  if (ret < 0 && ret >= -4095) {
    errno = (int)-ret;
    return -1;
  }
  return ret;
}

/* textos' on-wire struct stat (from <bits/stat.h>) is not layout
   compatible with newlib's struct stat, so stat/fstat marshal through a
   private copy.  */
struct _textos_stat
{
  long long dev;
  long long ino;
  unsigned int nlink;
  unsigned int mode;
  unsigned int uid;
  unsigned int gid;
  long long rdev;
  long long size;
  long long blksize;
  long long blocks;
  long long atime;
  long long mtime;
  long long ctime;
};

static void
_textos_to_newlib_stat(const struct _textos_stat* ks, struct stat* st)
{
  memset(st, 0, sizeof(*st));
  st->st_dev = ks->dev;
  st->st_ino = ks->ino;
  st->st_mode = ks->mode;
  st->st_nlink = ks->nlink;
  st->st_uid = ks->uid;
  st->st_gid = ks->gid;
  st->st_rdev = ks->rdev;
  st->st_size = ks->size;
  st->st_blksize = ks->blksize;
  st->st_blocks = ks->blocks;
  st->st_atim.tv_sec = ks->atime;
  st->st_mtim.tv_sec = ks->mtime;
  st->st_ctim.tv_sec = ks->ctime;
}

/* Translate newlib O_* flags to textos' open flag bit layout.  textos has
   no equivalent for O_CLOEXEC/O_NOCTTY/O_SYNC/O_DIRECT, so those are
   silently dropped (textos' exec does not implement close-on-exec).  */
static int
_textos_openflags(int flags)
{
  int t = flags & O_ACCMODE;

  if (flags & O_CREAT)
    t |= T_O_CREAT;
  if (flags & O_EXCL)
    t |= T_O_EXCL;
  if (flags & O_TRUNC)
    t |= T_O_TRUNC;
  if (flags & O_APPEND)
    t |= T_O_APPEND;
  if (flags & O_NONBLOCK)
    t |= T_O_NONBLOCK;
  if (flags & _FDIRECTORY)
    t |= T_O_DIRECTORY;
  if (flags & _FNOFOLLOW)
    t |= T_O_NOFOLLOW;
  return t;
}

static int
_newlib_openflags(int t)
{
  int flags = t & 3; /* O_RDONLY/O_WRONLY/O_RDWR */

  if (t & T_O_CREAT)
    flags |= O_CREAT;
  if (t & T_O_EXCL)
    flags |= O_EXCL;
  if (t & T_O_TRUNC)
    flags |= O_TRUNC;
  if (t & T_O_APPEND)
    flags |= O_APPEND;
  if (t & T_O_NONBLOCK)
    flags |= O_NONBLOCK;
  if (t & T_O_DIRECTORY)
    flags |= _FDIRECTORY;
  if (t & T_O_NOFOLLOW)
    flags |= _FNOFOLLOW;
  return flags;
}

_READ_WRITE_RETURN_TYPE
_read(int fd, void* buf, size_t cnt)
{
  return __sysret(__syscall3(SYS_read, fd, (long)buf, cnt));
}

_READ_WRITE_RETURN_TYPE
_write(int fd, const void* buf, size_t cnt)
{
  return __sysret(__syscall3(SYS_write, fd, (long)buf, cnt));
}

int
_open(const char* name, int flags, ...)
{
  int mode = 0;
  va_list ap;

  va_start(ap, flags);
  mode = va_arg(ap, int);
  va_end(ap);

  return __sysret(
    __syscall3(SYS_open, (long)name, _textos_openflags(flags), mode));
}

int
_close(int fd)
{
  return __sysret(__syscall1(SYS_close, fd));
}

off_t
_lseek(int fd, off_t pos, int whence)
{
  return __sysret(__syscall3(SYS_lseek, fd, pos, whence));
}

int
_fstat(int fd, struct stat* st)
{
  struct _textos_stat ks;

  long r = __sysret(__syscall2(SYS_fstat, fd, (long)&ks));
  if (r != 0)
    return r;
  _textos_to_newlib_stat(&ks, st);
  return 0;
}

int
_stat(const char* file, struct stat* st)
{
  struct _textos_stat ks;

  long r = __sysret(__syscall2(SYS_stat, (long)file, (long)&ks));
  if (r != 0)
    return r;
  _textos_to_newlib_stat(&ks, st);
  return 0;
}

int
_isatty(int fd)
{
  struct
  {
    unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel;
  } wsz;

  return __sysret(__syscall3(SYS_ioctl, fd, TIOCGWINSZ, (long)&wsz)) == 0;
}

void*
_sbrk(ptrdiff_t incr)
{
  char* cur = (char*)__syscall1(SYS_brk, 0);
  char* new = (char*)__syscall1(SYS_brk, (long)(cur + incr));

  if (new < cur + incr) {
    errno = ENOMEM;
    return (void*)-1;
  }
  return cur;
}

int
_kill(int pid, int sig)
{
  return __sysret(__syscall2(SYS_kill, pid, sig));
}

pid_t
_getpid(void)
{
  return __sysret(__syscall0(SYS_getpid));
}

clock_t
_times(struct tms* buf)
{
  return __sysret(__syscall1(SYS_times, (long)buf));
}

int
_gettimeofday(struct timeval* tp, void* tzp)
{
  return __sysret(__syscall2(SYS_gettimeofday, (long)tp, (long)tzp));
}

int
_unlink(const char* name)
{
  return __sysret(__syscall1(SYS_unlink, (long)name));
}

int
_link(const char* oldpath, const char* newpath)
{
  (void)oldpath;
  (void)newpath;
  errno = ENOSYS;
  return -1;
}

int
_rename(const char* oldpath, const char* newpath)
{
  return __sysret(__syscall2(SYS_rename, (long)oldpath, (long)newpath));
}

pid_t
_fork(void)
{
  return __sysret(__syscall0(SYS_fork));
}

pid_t
_wait(int* status)
{
  return __sysret(__syscall4(SYS_wait4, -1, (long)status, 0, 0));
}

int
_execve(const char* name, char* const argv[], char* const envp[])
{
  return __sysret(__syscall3(SYS_execve, (long)name, (long)argv, (long)envp));
}

int
_fcntl(int fd, int cmd, ...)
{
  int arg;
  va_list ap;
  long r;

  va_start(ap, cmd);
  arg = va_arg(ap, int);
  va_end(ap);

  r = __sysret(__syscall3(SYS_fcntl, fd, cmd, arg));
  if (r < 0)
    return r;
  if (cmd == F_GETFL)
    return _newlib_openflags((int)r);
  return r;
}

int
_getentropy(void* buf, size_t buflen)
{
  /* textos has no getrandom syscall.  Returning ENOSYS makes newlib's
     arc4random fall back to its weaker, non-cryptographic path.  */
  (void)buf;
  (void)buflen;
  errno = ENOSYS;
  return -1;
}

void
_exit(int status)
{
  __syscall1(SYS_exit, status);
  __builtin_unreachable();
}
