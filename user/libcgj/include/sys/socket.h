/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sys/socket.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * socket/socketpair/bind/listen/accept/connect/send/recv family, sockaddr
 * shapes, SOL_SOCKET options, MSG_* flags, and CMSG_* macros for SCM_RIGHTS
 * control messages. AF_*/SOCK_* numbers are Linux-shaped.
 *
 * Design notes
 * ------------
 * Backed by netstackd / net doors on GreenJade; bring-up may support loopback
 * and virtio-net paths first. sockaddr_storage is large enough for IPv6.
 *
 * Non-goals
 * ---------
 * Every obscure SOL_* protocol option; raw packet sockets full feature set.
 * See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AF_UNSPEC 0
#define AF_UNIX   1
#define AF_INET   2
#define AF_INET6  10

#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define SOCK_RAW    3
#define SOCK_CLOEXEC 02000000
#define SOCK_NONBLOCK 04000

#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define SO_ERROR     4
#define SO_RCVBUF    8
#define SO_SNDBUF    7
#define SO_KEEPALIVE 9
#define SO_TYPE      3
#define SO_PEERCRED  17

#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2

#define MSG_PEEK         0x2
#define MSG_DONTWAIT     0x40
#define MSG_TRUNC        0x20
#define MSG_CTRUNC       0x8
#define MSG_NOSIGNAL     0x4000
#define MSG_WAITALL      0x100
#define MSG_EOR          0x80
#define MSG_CONFIRM      0x800
#define MSG_ERRQUEUE     0x2000
#define MSG_WAITFORONE   0x10000
#define MSG_CMSG_CLOEXEC 0x40000000

/* SCM / control message helpers (glibc-shaped layout) */
#define SCM_RIGHTS      1
#define SCM_CREDENTIALS 2

#ifndef CMSG_ALIGN
#define CMSG_ALIGN(nLen) \
    (((size_t)(nLen) + sizeof(size_t) - 1u) & ~(sizeof(size_t) - 1u))
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(nLen) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(nLen))
#endif
#ifndef CMSG_LEN
#define CMSG_LEN(nLen) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (size_t)(nLen))
#endif

typedef unsigned short sa_family_t;
typedef unsigned int   socklen_t;

struct sockaddr {
    sa_family_t sa_family;
    char        sa_data[14];
};

struct sockaddr_storage {
    sa_family_t ss_family;
    char        __ss_pad[126];
};

struct msghdr {
    void         *msg_name;
    socklen_t     msg_namelen;
    struct iovec *msg_iov;
    size_t        msg_iovlen;
    void         *msg_control;
    size_t        msg_controllen;
    int           msg_flags;
};

struct cmsghdr {
    size_t cmsg_len;
    int    cmsg_level;
    int    cmsg_type;
};

struct mmsghdr {
    struct msghdr msg_hdr;
    unsigned int  msg_len;
};

#ifndef CMSG_DATA
#define CMSG_DATA(pCmsg) \
    ((unsigned char *)(pCmsg) + CMSG_ALIGN(sizeof(struct cmsghdr)))
#endif
#ifndef CMSG_FIRSTHDR
#define CMSG_FIRSTHDR(pMsg)                                           \
    ((size_t)(pMsg)->msg_controllen >= sizeof(struct cmsghdr)         \
         ? (struct cmsghdr *)(pMsg)->msg_control                      \
         : (struct cmsghdr *)0)
#endif
#ifndef CMSG_NXTHDR
#define CMSG_NXTHDR(pMsg, pCmsg)                                         \
    (((pCmsg) == 0)                                                      \
         ? CMSG_FIRSTHDR(pMsg)                                           \
         : ((((unsigned char *)(pCmsg) + CMSG_ALIGN((pCmsg)->cmsg_len) +  \
              sizeof(struct cmsghdr)) >                                  \
             ((unsigned char *)((pMsg)->msg_control) +                   \
              (pMsg)->msg_controllen))                                   \
                ? (struct cmsghdr *)0                                    \
                : (struct cmsghdr *)((unsigned char *)(pCmsg) +          \
                                     CMSG_ALIGN((pCmsg)->cmsg_len))))
#endif

/* Forward decl for recvmmsg timeout (full def in time.h) */
struct timespec;

struct cmsghdr *__cmsg_nxthdr(struct msghdr *pMsg, struct cmsghdr *pCmsg);

int     socket(int nDomain, int nType, int nProtocol);
int     socketpair(int nDomain, int nType, int nProtocol, int aSv[2]);
int     bind(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr);
int     listen(int nFd, int nBacklog);
int     accept(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr);
int     accept4(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr,
                int nFlags);
int     connect(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr);
ssize_t send(int nFd, const void *pBuf, size_t cb, int nFlags);
ssize_t recv(int nFd, void *pBuf, size_t cb, int nFlags);
ssize_t sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
               const struct sockaddr *pAddr, socklen_t cbAddr);
ssize_t recvfrom(int nFd, void *pBuf, size_t cb, int nFlags,
                 struct sockaddr *pAddr, socklen_t *pCbAddr);
ssize_t sendmsg(int nFd, const struct msghdr *pMsg, int nFlags);
ssize_t recvmsg(int nFd, struct msghdr *pMsg, int nFlags);
int     sendmmsg(int nFd, struct mmsghdr *pMsgvec, unsigned int uVlen,
                 int nFlags);
int     recvmmsg(int nFd, struct mmsghdr *pMsgvec, unsigned int uVlen,
                 int nFlags, struct timespec *pTimeout);
int     shutdown(int nFd, int nHow);
int     setsockopt(int nFd, int nLevel, int nOpt, const void *pVal,
                   socklen_t cbVal);
int     getsockopt(int nFd, int nLevel, int nOpt, void *pVal,
                   socklen_t *pCbVal);
int     getsockname(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr);
int     getpeername(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr);

#ifdef __cplusplus
}
#endif
