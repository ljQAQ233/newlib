#include <stdlib.h>
#include <sys/unistd.h>

extern char **environ;
extern void __init_wrapper();
extern int main(int, char **, char **);
extern void __libc_init_array(void);
extern void __libc_fini_array(void);

void __textos_crt1(long *sp) __attribute__((noreturn));

void __textos_crt1(long *sp) {
  int argc = (int)sp[0];
  char **argv = (char **)&sp[1];
  char **envp = (char **)&sp[1 + argc + 1];

  environ = envp;

  __init_wrapper();

  /* _WANT_REGISTER_FINI is not enabled for textos, so register the
     finalization routine ourselves.  It runs after any atexit handler
     the program (or its constructors) may install.  */
  atexit(__libc_fini_array);

  __libc_init_array();
  exit(main(argc, argv, envp));
}

__attribute__((naked, noreturn)) void _start(void) {
  __asm__ volatile("movq %rsp, %rdi\n\t"
                   "andq $-16, %rsp\n\t"
                   "call __textos_crt1");
}
