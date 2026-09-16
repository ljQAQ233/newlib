#ifndef	_SYS_SOCKET_H
#define	_SYS_SOCKET_H

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

#ifndef _SA_FAMILY_T_DECLARED
typedef unsigned short sa_family_t;
#define _SA_FAMILY_T_DECLARED
#endif

#ifndef _SOCKLEN_T_DECLARED
typedef unsigned socklen_t;
#define _SOCKLEN_T_DECLARED
#endif

struct iovec;

/* address family */
#define AF_UNSPEC 0
#define AF_INET   2
#define AF_PACKET 17
#define AF_MAX    45

/* procotol */
#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define SOCK_RAW    3
#define SOCK_PACKET 10

struct sockaddr
{
    sa_family_t sa_family;
    char sa_data[14];
};

struct msghdr
{
    void *msg_name;
    socklen_t msg_namelen;
    struct iovec *msg_iov;
    size_t msg_iovlen;
};

int socket(int domain, int type, int proto);

int bind(int fd, struct sockaddr *addr, socklen_t len);
int listen(int fd, int backlog);
int accept(int fd, struct sockaddr *addr, socklen_t *len);
int connect(int fd, struct sockaddr *addr, socklen_t len);

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

int shutdown(int fd, int how);

int getsockname(int fd, struct sockaddr *addr, socklen_t *len);
int getpeername(int fd, struct sockaddr *addr, socklen_t *len);

ssize_t sendmsg(int fd, struct msghdr *msg, int flags);
ssize_t recvmsg(int fd, struct msghdr *msg, int flags);

ssize_t sendto(int fd, void *buf, size_t len, int flags, struct sockaddr *dst, socklen_t dlen);
ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *src, socklen_t *slen);

ssize_t send(int fd, void *buf, size_t len, int flags);
ssize_t recv(int fd, void *buf, size_t len, int flags);

__END_DECLS

#endif


