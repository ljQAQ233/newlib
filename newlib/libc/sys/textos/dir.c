/**
 * @file
 * @brief POSIX directory interface for textos.
 *
 * Extracted from the textos C library (<misc/dir.c>).  The textos readdir
 * syscall fills a buffer with as many `dir_t` records as fit and reports the
 * number of bytes used; when the buffer overflows the stream is marked
 * exhausted.  `readdir` therefore caches the whole listing and hands out one
 * entry per call.
 *
 * Unlike the original, the number of valid bytes is tracked so a partially
 * filled buffer does not leak stale entries, and `seekdir` invalidates the
 * cache so `rewinddir` really rewinds.
 *
 * @author maouai233
 * @author deepseek-v4-flash
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* textos kernel directory entry ABI (see <bits/dirent.h> in the textos tree).
   `name` is a flexible array member; `siz` covers the header and the name.  */
typedef struct
{
  int idx;
  int type;
  ino_t ino;
  size_t siz;
  char name[];
} textos_dir_t;

extern ssize_t __readdir(int fd, void* buf, size_t mx);
extern int __seekdir(int fd, size_t* pos);

DIR*
opendir(const char* dirname)
{
  int fd = open(dirname, O_RDONLY | O_DIRECTORY);
  if (fd < 0)
    return NULL;

  DIR* d = fdopendir(fd);
  if (!d)
    close(fd);
  return d;
}

DIR*
fdopendir(int fd)
{
  if (fd < 0) {
    errno = EBADF;
    return NULL;
  }

  DIR* d = malloc(sizeof(DIR));
  if (!d)
    return NULL;

  d->fd = fd;
  d->tell = 0;
  d->boff = 0;
  d->blen = 0;
  return d;
}

int
closedir(DIR* dirp)
{
  if (!dirp) {
    errno = EBADF;
    return -1;
  }

  int ret = close(dirp->fd);
  free(dirp);
  return ret;
}

struct dirent*
readdir(DIR* dirp)
{
  if (dirp->boff >= dirp->blen) {
    ssize_t n = __readdir(dirp->fd, dirp->buf, sizeof(dirp->buf));
    if (n <= 0)
      return NULL;
    dirp->boff = 0;
    dirp->blen = (unsigned)n;
  }

  textos_dir_t* dir = (textos_dir_t*)(dirp->buf + dirp->boff);
  struct dirent* cur = &dirp->cur;

  cur->d_ino = dir->ino;
  cur->d_off = dir->idx;
  cur->d_reclen = sizeof(struct dirent);
  cur->d_type = dir->type;

  size_t nlen = strlen(dir->name);
  if (nlen > NAME_MAX)
    nlen = NAME_MAX;
  memcpy(cur->d_name, dir->name, nlen);
  cur->d_name[nlen] = '\0';

  dirp->boff += dir->siz;
  dirp->tell = dir->idx;
  return cur;
}

int
readdir_r(DIR* restrict dirp, struct dirent* restrict entry,
          struct dirent** restrict result)
{
  struct dirent* d = readdir(dirp);
  if (!d) {
    *result = NULL;
    return 0;
  }
  memcpy(entry, d, sizeof(struct dirent));
  *result = entry;
  return 0;
}

void
rewinddir(DIR* dirp)
{
  seekdir(dirp, 0);
}

long
telldir(DIR* dirp)
{
  return (long)dirp->tell;
}

void
seekdir(DIR* dirp, long loc)
{
  size_t pos = (size_t)loc;
  __seekdir(dirp->fd, &pos);
  dirp->boff = 0;
  dirp->blen = 0;
  dirp->tell = loc;
}

int
dirfd(DIR* dirp)
{
  return dirp->fd;
}
