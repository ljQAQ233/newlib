/**
 * @file
 * @brief textos <sys/utsname.h>.
 *
 * Extracted from the textos C library; the field sizes match the struct
 * returned by the textos uname syscall.
 *
 * @author deepseek-v4-flash
 */

#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H 1

#include <sys/cdefs.h>

__BEGIN_DECLS

struct utsname
{
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
};

int uname(struct utsname* name);

__END_DECLS

#endif /* _SYS_UTSNAME_H */
