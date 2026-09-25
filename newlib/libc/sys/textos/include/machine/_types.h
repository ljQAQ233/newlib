#ifndef _MACHINE__TYPES_H
#define _MACHINE__TYPES_H

#include <machine/_default_types.h>

typedef int __key_t;
#define __machine_key_t_defined

typedef unsigned __id_t;
#define __machine_id_t_defined

typedef unsigned __uid_t;
#define __machine_uid_t_defined

typedef unsigned __gid_t;
#define __machine_gid_t_defined

typedef unsigned __mode_t;
#define __machine_mode_t_defined

typedef unsigned __nlink_t;
#define __machine_nlink_t_defined

typedef __int64_t _off_t;
#define __machine_off_t_defined

typedef __int64_t __ino_t;
#define __machine_ino_t_defined

typedef __int64_t __dev_t;
#define __machine_dev_t_defined

typedef __int64_t __blksize_t;
#define __machine_blksize_t_defined

typedef __int64_t __blkcnt_t;
#define __machine_blkcnt_t_defined

typedef __uint64_t __fsblkcnt_t;
#define __machine_fsblkcnt_t_defined

typedef __uint64_t __fsfilcnt_t;
#define __machine_fsfilcnt_t_defined

// time_t is always 64-bit

#define _CLOCK_T_ __int64_t
#define __machine_clock_t_defined

#define _CLOCKID_T_ int
#define __machine_clockid_t_defined

typedef __int64_t __suseconds_t;
#define __machine_suseconds_t_defined

typedef __uint32_t __useconds_t;
#define __machine_useconds_t_defined


#endif
