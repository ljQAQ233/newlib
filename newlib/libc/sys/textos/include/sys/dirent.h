/**
 * @file
 * @brief textos <sys/dirent.h>.
 *
 * Extracted from the textos C library.  The `DIR` stream keeps a whole
 * directory listing in a buffer because the textos readdir syscall fills as
 * many entries as fit in one call and marks the stream exhausted when the
 * buffer overflows.
 *
 * @author deepseek-v4-flash
 */

#ifndef _SYS_DIRENT_H_
#define _SYS_DIRENT_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/syslimits.h>

__BEGIN_DECLS

/* textos returns the directory listing in one go, so the buffer has to be
   large enough to hold it.  */
#define __DIR_BUFSZ 4096

struct dirent
{
  ino_t d_ino;                /* inode number */
  off_t d_off;                /* directory position (textos idx) */
  unsigned short d_reclen;    /* length of this record */
  unsigned char d_type;       /* file type, see DT_* below */
  char d_name[NAME_MAX + 1];  /* null-terminated filename */
};

typedef struct __dirstream
{
  int fd;                     /* directory file descriptor */
  off_t tell;                 /* position of the last entry returned */
  unsigned boff;              /* offset into buf */
  unsigned blen;              /* number of valid bytes in buf */
  char buf[__DIR_BUFSZ];
  struct dirent cur;
} DIR;

/* File types returned in d_type.  */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LNK     10
#define DT_SOCK    12
#define DT_WHT     14

#define __dirfd(dp) ((dp)->fd)

__END_DECLS

#endif /* _SYS_DIRENT_H_ */
