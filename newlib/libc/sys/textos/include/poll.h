/**
 * @file
 * @brief textos <poll.h>.
 *
 * Extracted from the textos C library; the request bits match the textos
 * kernel and the poll timeout is in milliseconds.
 *
 * @author deepseek-v4-flash
 */

#ifndef _POLL_H
#define _POLL_H

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef unsigned long nfds_t;

#define POLLIN     0x001 /* There is data to read.  */
#define POLLPRI    0x002 /* There is urgent data to read.  */
#define POLLOUT    0x004 /* Writing now will not block.  */
#define POLLERR    0x008 /* Error condition (output only).  */
#define POLLHUP    0x010 /* Hang up (output only).  */
#define POLLNVAL   0x020 /* Invalid request: fd not open (output only).  */
#define POLLRDNORM 0x040 /* Normal data may be read.  */
#define POLLRDBAND 0x080 /* Priority data may be read.  */
#define POLLWRNORM 0x100 /* Writing now will not block.  */
#define POLLWRBAND 0x200 /* Priority data may be written.  */

struct pollfd
{
  int fd;
  short events;
  short revents;
};

int poll(struct pollfd __fds[], nfds_t __nfds, int __timeout /* ms */);

__END_DECLS

#endif /* _POLL_H */
