/**
 * @file
 * @brief textos ioctl interface.
 *
 * Pulls in the textos request constants from <bits/ioctl.h>.
 *
 * @author deepseek-v4-flash
 */

#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <bits/ioctl.h>

int ioctl(int __fd, int __req, ...);

__END_DECLS

#endif /* _SYS_IOCTL_H */
