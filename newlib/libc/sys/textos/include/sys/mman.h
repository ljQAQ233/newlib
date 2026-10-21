#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_

#define MAP_SHARED 0x1
#define MAP_PRIVATE 0x2
#define MAP_TYPE 0xf
#define MAP_FIXED 0x10
#define MAP_ANON 0x20
#define MAP_ANONYMOUS MAP_ANON

#define MAP_POPULATE 0x8000

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_FAILED ((void*)-1)

void*
mmap(void* addr, size_t len, int prot, int flgs, int fd, off_t off);

int
mprotect(void* addr, size_t len, int prot);

int
munmap(void* addr, size_t len);

#endif
