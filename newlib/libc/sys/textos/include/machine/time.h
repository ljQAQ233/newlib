#ifndef	_MACHTIME_H_
#define	_MACHTIME_H_

#define CLOCKS_PER_SEC 1000000L

#include <sys/_timespec.h>
int nanosleep (const struct timespec *, struct timespec *);

#endif	/* _MACHTIME_H_ */
