/**
 * @file
 * @brief textos override: Linux sysconf values.
 *
 * Match textos <unistd.h>.  The declarations are kept from newlib's
 * <sys/unistd.h>.
 *
 * @author deepseek-v4-flash
 */

#include <sys/unistd.h>

#undef _SC_PAGESIZE
#undef _SC_PAGE_SIZE
#define _SC_PAGESIZE  30
#define _SC_PAGE_SIZE 30
