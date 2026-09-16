/**
 * @file
 * @brief textos <sys/uio.h>.
 *
 * Extracted from the textos C library; `struct iovec` keeps the textos
 * layout and the declarations use newlib's type definitions.
 *
 * @author deepseek-v4-flash
 */

#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

#ifndef __DEFINED_struct_iovec
struct iovec
{
  void* iov_base;
  size_t iov_len;
};
#define __DEFINED_struct_iovec
#endif

ssize_t readv(int __fd, const struct iovec* __iov, int __iovcnt);
ssize_t writev(int __fd, const struct iovec* __iov, int __iovcnt);

__END_DECLS

#endif /* _SYS_UIO_H */
