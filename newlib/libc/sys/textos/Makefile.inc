libc_a_SOURCES += %D%/syscalls.c

toollib_DATA += %D%/crt1.o %D%/crti.o %D%/crtn.o
%D%/crt1.o %D%/crti.o %D%/crtn.o: stmp-targ-include
CLEANFILES += %D%/crt1.o %D%/crti.o %D%/crtn.o
