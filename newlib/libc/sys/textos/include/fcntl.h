/**
 * @file
 * @brief textos override: Linux open(2)/fcntl(2) flag values.
 *
 * Match the textos kernel.  newlib's own values (BSD derived) do not
 * agree with the Linux ABI that textos implements.
 *
 * The rest of <sys/fcntl.h> (struct flock, AT_* flags, ...) is kept from
 * newlib.
 *
 * @author deepseek-v4-flash
 */

#include <sys/fcntl.h>

#undef O_ACCMODE
#undef O_RDONLY
#undef O_WRONLY
#undef O_RDWR
#undef O_CREAT
#undef O_EXCL
#undef O_TRUNC
#undef O_APPEND
#undef O_NONBLOCK
#undef O_DIRECTORY
#undef O_NOFOLLOW

#define O_ACCMODE   0003
#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_EXCL      0200
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000

#undef F_GETOWN
#undef F_SETOWN
#undef F_GETLK
#undef F_SETLK
#undef F_SETLKW
#undef F_RDLCK
#undef F_WRLCK
#undef F_UNLCK

#define F_GETOWN 9
#define F_SETOWN 8
#define F_GETLK  5
#define F_SETLK  6
#define F_SETLKW 7
#define F_RDLCK  0
#define F_WRLCK  1
#define F_UNLCK  2
