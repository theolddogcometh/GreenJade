/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/epoll.h (subset). Not GNU glibc.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EPOLLIN      0x001
#define EPOLLPRI     0x002
#define EPOLLOUT     0x004
#define EPOLLERR     0x008
#define EPOLLHUP     0x010
#define EPOLLRDHUP   0x2000
#define EPOLLET      (1u << 31)
#define EPOLLONESHOT (1u << 30)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

#define EPOLL_CLOEXEC 02000000

typedef union epoll_data {
    void    *ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;
    epoll_data_t data;
} __attribute__((packed));

int epoll_create(int nSize);
int epoll_create1(int nFlags);
int epoll_ctl(int nEpfd, int nOp, int nFd, struct epoll_event *pEvent);
int epoll_wait(int nEpfd, struct epoll_event *pEvents, int nMaxevents,
               int nTimeout);
int epoll_pwait(int nEpfd, struct epoll_event *pEvents, int nMaxevents,
                int nTimeout, const void *pSigmask);
struct timespec;
struct __sigset_t;
int epoll_pwait2(int nEpfd, struct epoll_event *pEvents, int nMaxevents,
                 const struct timespec *pTimeout, const void *pSigmask);

#ifdef __cplusplus
}
#endif
