/**
 * @file
 * @brief textos termios definitions.
 *
 * struct termios and the flag values match the textos kernel and its
 * <termios.h>.
 *
 * @author deepseek-v4-flash
 */

#ifndef _SYS_TERMIOS_H
#define _SYS_TERMIOS_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

#define NCCS 32

struct termios
{
    tcflag_t c_iflag;
    tcflag_t c_oflag;
    tcflag_t c_cflag;
    tcflag_t c_lflag;
    cc_t c_line;
    cc_t c_cc[NCCS];
    speed_t __c_ispeed;
    speed_t __c_ospeed;
};

/* input flags */
#define IGNCR   0000200
#define ICRNL   0000400
#define IXON    0002000
#define IXOFF   0010000

/* output flags */
#define OPOST  0000001
#define OLCUC  0000002
#define ONLCR  0000004

/* control flags */
#define CSIZE  0000060
#define CS5    0000000
#define CS6    0000020
#define CS7    0000040
#define CS8    0000060
#define CREAD  0000200
#define CLOCAL 0004000

/* local flags */
#define ISIG   0000001
#define ICANON 0000002
#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400
#define IEXTEN 0100000

/* c_cc indices */
#define VINTR     0
#define VQUIT     1
#define VERASE    2
#define VKILL     3
#define VEOF      4
#define VTIME     5
#define VMIN      6
#define VSWTC     7
#define VSTART    8
#define VSTOP     9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* optional actions */
#define TCSANOW   0
#define TCSADRAIN 1
#define TCSAFLUSH 2

int tcgetattr(int __fd, struct termios *__tio);
int tcsetattr(int __fd, int __act, const struct termios *__tio);
int tcsetpgrp(int __fd, pid_t __pgrp);
pid_t tcgetpgrp(int __fd);

__END_DECLS

#endif /* _SYS_TERMIOS_H */
