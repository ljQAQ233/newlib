libc_a_SOURCES += %D%/syscalls.c %D%/termios.c %D%/dir.c %D%/signal.c
libc_a_SOURCES += %D%/wrapper/winit.c %D%/wrapper/wutil.c

toollib_DATA += %D%/crt1.o %D%/crti.o %D%/crtn.o
%D%/crt1.o %D%/crti.o %D%/crtn.o: stmp-targ-include
CLEANFILES += %D%/crt1.o %D%/crti.o %D%/crtn.o
