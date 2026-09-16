/* sys/signal.h */

#ifndef _SYS_SIGNAL_H
#define _SYS_SIGNAL_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <_ansi.h>
#include <stdint.h>
#include <sys/_sigset.h>
#include <sys/_timespec.h>
#include <sys/cdefs.h>
#include <sys/features.h>
#include <sys/types.h>

#if !defined(_SIGSET_T_DECLARED)
#define _SIGSET_T_DECLARED
  typedef __sigset_t sigset_t;
#endif

#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_ONSTACK 0x08000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000
#define SA_RESTORER 0x04000000

  typedef void (*_sig_func_ptr)(int);

  struct sigaction
  {
    union
    {
      _sig_func_ptr sa_handler;
    };
    unsigned long sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask;
  };

  typedef struct siginfo
  {
    int _;
  } siginfo_t;

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
/*
 * Minimum and default signal stack constants. Allow for target overrides
 * from <sys/features.h>.
 */
#ifndef MINSIGSTKSZ
#define MINSIGSTKSZ 2048
#endif
#ifndef SIGSTKSZ
#define SIGSTKSZ 8192
#endif

/*
 * Possible values for ss_flags in stack_t below.
 */
#define SS_ONSTACK 0x1
#define SS_DISABLE 0x2

#endif

  /*
   * Structure used in sigaltstack call.
   */
  typedef struct sigaltstack
  {
    void* ss_sp;    /* Stack base or pointer.  */
    int ss_flags;   /* Flags.  */
    size_t ss_size; /* Stack size.  */
  } stack_t;

#if __POSIX_VISIBLE
#define SIG_SETMASK 0 /* set mask with sigprocmask() */
#define SIG_BLOCK 1   /* set of signals to block */
#define SIG_UNBLOCK 2 /* set of signals to, well, unblock */

  int sigprocmask(int, const sigset_t*, sigset_t*);
#endif

#if __POSIX_VISIBLE >= 199506
  int pthread_sigmask(int, const sigset_t*, sigset_t*);
#endif

#ifdef _LIBC
  int _kill(pid_t, int);
#endif /* _LIBC */

#if __POSIX_VISIBLE
  int kill(pid_t, int);
#endif

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4
  int killpg(pid_t, int);
#endif
#if __POSIX_VISIBLE
  int sigaction(int, const struct sigaction*, struct sigaction*);
  int sigaddset(sigset_t*, const int);
  int sigdelset(sigset_t*, const int);
  int sigismember(const sigset_t*, int);
  int sigfillset(sigset_t*);
  int sigemptyset(sigset_t*);
  int sigpending(sigset_t*);
  int sigsuspend(const sigset_t*);
  int sigwait(const sigset_t*, int*);
#endif /* __POSIX_VISIBLE */

/* There are two common sigpause variants, both of which take an int argument.
   If you request _XOPEN_SOURCE or _GNU_SOURCE, you get the System V version,
   which removes the given signal from the process's signal mask; otherwise
   you get the BSD version, which sets the process's signal mask to the given
   value. */
#if __XSI_VISIBLE && !defined(__INSIDE_CYGWIN__)
#ifdef __GNUC__
  int sigpause(int) __asm__(__ASMNAME("__xpg_sigpause"));
#else
  int __xpg_sigpause(int);
#define sigpause __xpg_sigpause
#endif
#elif __BSD_VISIBLE
int
sigpause(int);
#endif

#if __BSD_VISIBLE || __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
  int sigaltstack(const stack_t* __restrict, stack_t* __restrict);
#endif

#if __POSIX_VISIBLE >= 199506
  int pthread_kill(pthread_t, int);
#endif

#if __POSIX_VISIBLE >= 199309

  /*  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
      NOTE: P1003.1c/D10, p. 39 adds sigwait().  */

  int sigwaitinfo(const sigset_t*, siginfo_t*);
  int sigtimedwait(const sigset_t*, siginfo_t*, const struct timespec*);
  /*  3.3.9 Queue a Signal to a Process, P1003.1b-1993, p. 78 */
  int sigqueue(pid_t, int, const union sigval);

#endif /* __POSIX_VISIBLE >= 199309 */

#define _NSIG 32
#define NSIG _NSIG
#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGIOT SIGABRT
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL 29
#define SIGPWR 30
#define SIGSYS 31
#define SIGUNUSED SIGSYS

#ifdef __cplusplus
}
#endif

#if defined(__CYGWIN__)
#if __XSI_VISIBLE >= 4 || __POSIX_VISIBLE >= 200809
#include <sys/ucontext.h>
#endif
#endif

#ifndef _SIGNAL_H_
/* Some applications take advantage of the fact that <sys/signal.h>
 * and <signal.h> are equivalent in glibc.  Allow for that here.  */
#include <signal.h>
#endif
#endif /* _SYS_SIGNAL_H */
