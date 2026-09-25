typedef long long __sc_type;

#define __syscall(num, a1, a2, a3, a4, a5, a6)                                 \
  ({                                                                           \
    register long __r0 __asm__("rax") = (long)(num);                           \
    register long __r1 __asm__("rdi") = (long)(a1);                            \
    register long __r2 __asm__("rsi") = (long)(a2);                            \
    register long __r3 __asm__("rdx") = (long)(a3);                            \
    register long __r4 __asm__("r10") = (long)(a4);                            \
    register long __r5 __asm__("r8") = (long)(a5);                             \
    register long __r6 __asm__("r9") = (long)(a6);                             \
                                                                               \
    __asm__ volatile(                                                          \
      "syscall"                                                                \
      : "+r"(__r0)                                                             \
      : "r"(__r1), "r"(__r2), "r"(__r3), "r"(__r4), "r"(__r5), "r"(__r6)       \
      : "memory", "rcx", "r11");                                               \
    __r0;                                                                      \
  })

