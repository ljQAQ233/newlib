/* textos override: use the Linux errno numbering, matching the textos
   kernel and its <bits/errno.h>.  newlib's own numbering (BSD/SVR4
   derived) does not agree with the Linux ABI that textos implements.

   The errno variable plumbing is kept from newlib's <sys/errno.h>; only
   the numeric values come from <bits/errno.h>.  */

#ifndef _SYS_ERRNO_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _SYS_ERRNO_H_

#include <sys/reent.h>

#ifdef _REENT_THREAD_LOCAL
#define errno (_tls_errno)
#else /* _REENT_THREAD_LOCAL */

#ifndef _REENT_ONLY
#define errno (*__errno())
extern int *__errno (void);
#endif

#endif /* _REENT_THREAD_LOCAL */

/* Please don't use these variables directly.
   Use strerror instead. */
extern __IMPORT const char * const _sys_errlist[];
extern __IMPORT int _sys_nerr;

#define __errno_r(ptr) _REENT_ERRNO(ptr)

#include <bits/errno.h>

/* newlib-only codes that are not part of the Linux errno set.  */
#ifndef EFTYPE
#define EFTYPE 79
#endif
#ifndef __ELASTERROR
#define __ELASTERROR 2000
#endif

#ifdef __cplusplus
}
#endif
#endif /* _SYS_ERRNO_H */
