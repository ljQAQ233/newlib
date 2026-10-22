libc_a_SOURCES += %D%/syscalls.c %D%/termios.c %D%/dir.c %D%/signal.c %D%/sysconf.c
libc_a_SOURCES += %D%/wrapper/winit.c %D%/wrapper/wutil.c

libc_a_SOURCES += \
    libc/unix/getcwd.c \
    libc/unix/pread.c \
    libc/unix/pwrite.c \
    libc/unix/sigset.c \
    libc/unix/ttyname.c \
    libc/unix/ttyname_r.c \
    libc/unix/basename.c \
    libc/unix/dirname.c \
    libc/unix/getlogin.c \
    libc/unix/getpwent.c \
    libc/unix/getut.c

toollib_DATA += %D%/crt1.o %D%/crti.o %D%/crtn.o
%D%/crt1.o %D%/crti.o %D%/crtn.o: stmp-targ-include
CLEANFILES += %D%/crt1.o %D%/crti.o %D%/crtn.o
